//
// Created by li on 8/16/24.
//

#include "rtc_base/logging.h"

#include "core/xchat_http.h"
#include "core/xchat_global.h"

namespace mRTC {
    HttpRequest::HttpRequest(const std::string &url, const std::string &body,
                             void *caller, OnReplyFunc replyFunc)
            : _iosHandler(_outputStream),
              _handler(_iosHandler),
              _caller(caller),
              _replyFunc(replyFunc) {
        curl::curl_header header;
        header.add("Another: xchat");
        header.add("Accept: application/json");
        header.add("Content-Type: application/x-www-form-urlencoded");

        // Add the headers to the easy object.
        _handler.add<CURLOPT_HTTPHEADER>(header.get());

        _handler.add<CURLOPT_URL>(url.c_str());
        _handler.add<CURLOPT_FOLLOWLOCATION>(1L);
        _handler.add<CURLOPT_SSL_VERIFYPEER>(false);
        _handler.add<CURLOPT_CONNECTTIMEOUT_MS>(10 * 1000);
        _handler.add<CURLOPT_TIMEOUT>(15 * 1000);

        if (!body.empty()) {
            _handler.add<CURLOPT_POST>(1L);
            _handler.add<CURLOPT_POSTFIELDS>(body.c_str());
        }
    }

    HttpRequest::~HttpRequest() {
        _caller = nullptr;
    }

    HttpService::HttpService() {}

    HttpService::~HttpService() {}

    void HttpService::post(const std::string &url, const std::string &body, void *caller, HttpRequest::OnReplyFunc replyFunc) {
        HttpRequest::Ptr request = std::make_shared<HttpRequest>(url, body, caller, replyFunc);
        _requests[&(request->_handler)] = request;
    }

    void HttpService::start() {
        RTC_LOG(LS_INFO) << " HttpService start... ";
        if (_running) {
            return;
        }

        auto fun = [this]() {
            RTC_LOG(LS_INFO) << " HttpService thread begin... ";
            rtc::SetCurrentThreadName("HttpService thread");
            _running = true;

            try {
                _multi.perform();

                while (_running && _multi.get_active_transfers()) {
                    _multi.perform();

                    std::unique_ptr<curl_multi::curl_message> message = _multi.get_next_finished();
                    if (!message) {
                        continue;
                    }

                    const curl_easy *handler = message->get_handler();
                    HttpRequest::Ptr request =  _requests[handler] = request;

                    auto funReply = [request]() {
                        if (!request->_caller) {
                            return;
                        }

                        request->_replyFunc(request);

                    };
                    mRTCGlobal::getInstance()->workThread()->PostTask(funReply);
                } //while


            } catch (curl_easy_exception &error) {
                // If you want to print the last error.
                RTC_LOG(LS_ERROR) << " HttpService error detail: " << error.what();
            }

        };

        _httpThread = new std::thread(fun);
    }

    void HttpService::stop() {
        RTC_LOG(LS_INFO) << " HttpService stop... ";

        _running = false;

        if (_httpThread && _httpThread->joinable()) {
            _httpThread->join();
            RTC_LOG(LS_INFO) << "HttpService thread join end";
        }

        RTC_LOG(LS_INFO) << "HttpService stop end";

    }


}