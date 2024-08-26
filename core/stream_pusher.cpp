//
// Created by li on 8/16/24.
//

#include "json/json.h"

#include "core/xchat_http.h"
#include "core/stream_pusher.h"
#include "core/xchat_global.h"
#include "media/cam_video_track_source.h"

namespace mRTC {
    StreamPusher::StreamPusher()
            : _currentThread(rtc::Thread::Current()) {
    }

    StreamPusher::~StreamPusher() {
    }

    bool StreamPusher::startPush(const std::string url, std::map<std::string, std::string>& reqBody, IVideoSource::Ptr ptr) {
        _host = url;
        _uid = reqBody["uid"];
        _streamId = reqBody["streamid"];
        auto fun = [this, url, &reqBody, ptr]() {

            std::string fullUrl = "https://" + url + "/signal/startpush";

            HttpRequest::OnReplyFunc replyFunc = [this, &reqBody, ptr](HttpRequest::Ptr request) {
                auto content = request->_outputStream.str();
                auto url = request->_handler.get_info<CURLINFO_EFFECTIVE_URL>();
                auto response_code = request->_handler.get_info<CURLINFO_RESPONSE_CODE>();
                auto content_type = request->_handler.get_info<CURLINFO_CONTENT_TYPE>();
                auto http_code = request->_handler.get_info<CURLINFO_HTTP_CODE>();

                RTC_LOG(LS_INFO) << "CODE: " << response_code.get() << "\n"
                                 << "TYPE: " << content_type.get() << "\n"
                                 << ", HTTP_CODE: " << http_code.get() << "\n"
                                 << ", URL: " << url.get() << "\n"
                                 << ", CONTENT: " << content;

                if (http_code.get() != 200) {
                    RTC_LOG(LS_WARNING) << "http response error, status: " << http_code.get();
                    return;
                }

                int parseRet = -1;
                std::string rtcType;
                std::string sdpDesp;

                auto mapContent = jsonToMap(content);
                if (mapContent.count("err")) {
                    parseRet = std::stoi(mapContent["err"]);
                }
                if (mapContent.count("type")) {
                    rtcType = mapContent["type"];
                }
                if (mapContent.count("sdp")) {
                    sdpDesp = mapContent["sdp"];
                }
                if (parseRet != 0 || rtcType.empty() || sdpDesp.empty()) {
                    RTC_LOG(LS_ERROR) << "jsonToMap failed. ret: "
                                      << parseRet << " type: " << rtcType
                                      << " sdp_desp: " << sdpDesp;
                    sendStartPushEvent(mRTCError::ErrStartPush );
                    return;
                }


                _pushStream = std::make_shared<PushStream>(_uid, _streamId);
                _pushStream->_sigAnswerReady.connect(this, &StreamPusher::onAnswerReady);
                _pushStream->_sigPcStateChange.connect(this, &StreamPusher::onPcStateChange);
                _pushStream->init();
                _pushStream->addAudioTrack();
                auto videoTrack = CamVideoTrackSource::create(dynamic_cast<rtc::VideoSourceInterface<webrtc::VideoFrame>*>(ptr.get()));
                _pushStream->addVideoTrack(videoTrack);
                _pushStream->setRemoteOffer(sdpDesp);
            };

            Json::Value jsonValue; // 创建一个Json::Value对象

            // 将map内容添加到jsonValue中
            for (const auto& pair : reqBody) {
                jsonValue[pair.first] = pair.second; // 将键值对存入JSON对象
            }

            Json::StreamWriterBuilder writer; // 创建一个写入器
            std::string jsonString = Json::writeString(writer, jsonValue); // 转换为字符串

            mRTCGlobal::getInstance()->httpSvc()->post(fullUrl, jsonString, this, replyFunc);

        };

        _currentThread->PostTask(fun);

        return true;
    }

    bool StreamPusher::stopPush() {
        auto fun = [this]() {
            _pushStream.reset();

            std::string fullUrl = "https://" + _host + "/signal/stoppush";

            HttpRequest::OnReplyFunc replyFunc = [this](HttpRequest::Ptr request) {
                auto content = request->_outputStream.str();
                auto url = request->_handler.get_info<CURLINFO_EFFECTIVE_URL>();
                auto response_code = request->_handler.get_info<CURLINFO_RESPONSE_CODE>();
                auto content_type = request->_handler.get_info<CURLINFO_CONTENT_TYPE>();
                auto http_code = request->_handler.get_info<CURLINFO_HTTP_CODE>();

                RTC_LOG(LS_INFO) << "CODE: " << response_code.get() << "\n"
                                 << "TYPE: " << content_type.get() << "\n"
                                 << ", HTTP_CODE: " << http_code.get() << "\n"
                                 << ", URL: " << url.get() << "\n"
                                 << ", CONTENT: " << content;

                if (http_code.get() != 200) {
                    RTC_LOG(LS_WARNING) << "http response error, status: " << http_code.get();
                    return;
                }

                int parseRet = -1;
                auto mapContent = jsonToMap(content);
                if (mapContent.count("err")) {
                    parseRet = std::stoi(mapContent["err"]);
                }
                if (parseRet != 0) {
                    RTC_LOG(LS_ERROR) << "jsonToMap failed. ret: " << parseRet;
                    sendStopPushEvent(mRTCError::ErrStopPush);
                    return;
                }
            };

            Json::Value jsonValue; // 创建一个Json::Value对象

            jsonValue["uid"] = _uid;
            jsonValue["streamid"] = _streamId;

            Json::StreamWriterBuilder writer; // 创建一个写入器
            std::string jsonString = Json::writeString(writer, jsonValue); // 转换为字符串

            mRTCGlobal::getInstance()->httpSvc()->post(fullUrl, jsonString, this, replyFunc);

        };

        _currentThread->PostTask(fun);

        return true;
    }

    void StreamPusher::onAnswerReady(RtcStream::Ptr stream, const std::string& answerSdp) {
        auto fun = [this, &answerSdp]() {

            std::string fullUrl = "https://" + _host + "/signal/sendanswer";

            HttpRequest::OnReplyFunc replyFunc = [this](HttpRequest::Ptr request) {
                auto content = request->_outputStream.str();
                auto url = request->_handler.get_info<CURLINFO_EFFECTIVE_URL>();
                auto response_code = request->_handler.get_info<CURLINFO_RESPONSE_CODE>();
                auto content_type = request->_handler.get_info<CURLINFO_CONTENT_TYPE>();
                auto http_code = request->_handler.get_info<CURLINFO_HTTP_CODE>();

                RTC_LOG(LS_INFO) << "CODE: " << response_code.get() << "\n"
                                 << "TYPE: " << content_type.get() << "\n"
                                 << ", HTTP_CODE: " << http_code.get() << "\n"
                                 << ", URL: " << url.get() << "\n"
                                 << ", CONTENT: " << content;

                if (http_code.get() != 200) {
                    RTC_LOG(LS_WARNING) << "http response error, status: " << http_code.get();
                    return;
                }


                int parseRet = -1;
                auto mapContent = jsonToMap(content);
                if (mapContent.count("err")) {
                    parseRet = std::stoi(mapContent["err"]);
                }
                if (parseRet != 0) {
                    RTC_LOG(LS_ERROR) << "jsonToMap failed. ret: " << parseRet;
                    sendStartPushEvent(mRTCError::ErrSendAnswer );
                    return;
                }

            };

            Json::Value jsonValue; // 创建一个Json::Value对象
            jsonValue["uid"] = _uid;
            jsonValue["streamid"] = _streamId;
            jsonValue["type"] = "push";
            jsonValue["answer"] = answerSdp;

            Json::StreamWriterBuilder writer; // 创建一个写入器
            std::string jsonString = Json::writeString(writer, jsonValue); // 转换为字符串

            mRTCGlobal::getInstance()->httpSvc()->post(fullUrl, jsonString, this, replyFunc);

        };

        _currentThread->PostTask(fun);

    }
    void StreamPusher::onPcStateChange(RtcStream::Ptr stream, RtcStreamState state) {
        auto observer = mRTCGlobal::getInstance()->xchatSvcObserver();
        if (observer) {
            observer->onPushStreamSate(shared_from_this(), state);
        }

    }

    void StreamPusher::sendStartPushEvent(mRTCError err ) {
        auto observer = mRTCGlobal::getInstance()->xchatSvcObserver();
        if (observer) {
            observer->onStartPushEvent(shared_from_this(), err );
        }
    }
    void StreamPusher::sendStopPushEvent(mRTCError err ) {
        auto observer = mRTCGlobal::getInstance()->xchatSvcObserver();
        if (observer) {
            observer->onStopPushEvent(shared_from_this(), err );
        }

    }

}
