//
// Created by li on 8/16/24.
//

#pragma once

#include <mutex>
#include <thread>
#include <memory>
#include "core/common.h"
#include "curlcpp/curl_easy.h"
#include "curlcpp/curl_header.h"
#include "curlcpp/curl_multi.h"
#include "curlcpp/curl_ios.h"

using curl::curl_easy;
using curl::curl_multi;
using curl::curl_ios;
using curl::curl_easy_exception;
using curl::curlcpp_traceback;

namespace mRTC {
    class HttpRequest {
    public:
        using Ptr = std::shared_ptr<HttpRequest>;
        using OnReplyFunc = std::function<void(HttpRequest::Ptr request)>;
        HttpRequest(const std::string& url, const std::string& body,
                    void *caller, OnReplyFunc replyFunc);
        ~HttpRequest();

    public:
        std::ostringstream _outputStream;
        curl_ios<std::ostringstream> _iosHandler;
        curl_easy _handler;
        void* _caller = nullptr;
        OnReplyFunc _replyFunc;
    };
    class HttpService {
    public:
        using Ptr = std::shared_ptr<HttpService>;
        HttpService();
        ~HttpService();

        void start();
        void stop();
        void post(const std::string &url, const std::string &body, void *caller, HttpRequest::OnReplyFunc replyFunc);

    private:
        std::mutex _mutex;
        curl_multi _multi;
        std::thread* _httpThread = nullptr;
        std::atomic<bool> _running = false;
        std::map<const curl_easy*, HttpRequest::Ptr> _requests;

    };


}