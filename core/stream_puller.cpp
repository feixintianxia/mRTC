//
// Created by li on 8/24/24.
//

#include "json/json.h"

#include "core/xchat_http.h"
#include "core/stream_puller.h"
#include "core/xchat_global.h"
#include "media/cam_video_track_source.h"

namespace mRTC {
    StreamPuller::StreamPuller()
            : _currentThread(rtc::Thread::Current()) {

    }
    StreamPuller::~StreamPuller() {

    }

    //IStreamPuller
    bool StreamPuller::startPull(const std::string url, std::map<std::string, std::string> &reqBody, PlayerCfg cfg) {
        _host = url;
        _uid = reqBody["uid"];
        _streamId = reqBody["streamid"];
        _cfg = cfg;
        auto fun = [this, url, &reqBody]() {

            std::string fullUrl = "https://" + url + "/signal/startpull";

            HttpRequest::OnReplyFunc replyFunc = [this, &reqBody](HttpRequest::Ptr request) {
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
                    sendStartPullEvent(mRTCError::ErrStartPull );
                    return;
                }


                _pullStream = std::make_shared<PullStream>(_uid, _streamId);
                _pullStream->_sigAnswerReady.connect(this, &StreamPuller::onAnswerReady);
                _pullStream->_sigPcStateChange.connect(this, &StreamPuller::onPcStateChange);
                _pullStream->_sigAddNewTrack.connect(this, &StreamPuller::onAddNewTrack);
                _pullStream->init();
                _pullStream->setRemoteOffer(sdpDesp);
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
    bool StreamPuller::stopPull() {
        auto fun = [this]() {
            _player->stop();
            _player.reset();
            _pullStream.reset();

            std::string fullUrl = "https://" + _host + "/signal/stoppull";

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
                    sendStopPullEvent(mRTCError::ErrStopPull);
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


    //信号槽
    void StreamPuller::onAnswerReady(RtcStream::Ptr stream, const std::string& answerSdp) {
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
                    sendStartPullEvent(mRTCError::ErrSendAnswer );
                    return;
                }

            };

            Json::Value jsonValue; // 创建一个Json::Value对象
            jsonValue["uid"] = _uid;
            jsonValue["streamid"] = _streamId;
            jsonValue["type"] = "pull";
            jsonValue["answer"] = answerSdp;

            Json::StreamWriterBuilder writer; // 创建一个写入器
            std::string jsonString = Json::writeString(writer, jsonValue); // 转换为字符串

            mRTCGlobal::getInstance()->httpSvc()->post(fullUrl, jsonString, this, replyFunc);

        };

        _currentThread->PostTask(fun);

    }
    void StreamPuller::onPcStateChange(RtcStream::Ptr stream, RtcStreamState state) {
        auto observer = mRTCGlobal::getInstance()->xchatSvcObserver();
        if (observer) {
            observer->onPullStreamSate(shared_from_this(), state);
        }

    }

    void StreamPuller::onAddNewTrack(RtcStream::Ptr stream,
                                rtc::scoped_refptr<webrtc::RtpTransceiverInterface> receiver) {

        auto* track = receiver->receiver()->track().release();
        if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
            auto* video_track =
                    static_cast<webrtc::VideoTrackInterface*>(track);
            startPlayer(video_track);
        }

        track->Release();

    }

    void StreamPuller::sendStartPullEvent(mRTCError err ) {
        auto observer = mRTCGlobal::getInstance()->xchatSvcObserver();
        if (observer) {
            observer->onStartPullEvent(shared_from_this(), err );
        }

    }

    void StreamPuller::sendStopPullEvent(mRTCError err ) {
        auto observer = mRTCGlobal::getInstance()->xchatSvcObserver();
        if (observer) {
            observer->onStopPullEvent(shared_from_this(), err );
        }
    }

    bool StreamPuller::startPlayer(webrtc::VideoTrackInterface* video_track) {
        if (!_cfg._hwnd) {
            return false;
        }

        _player.reset(new SDL2Player(_cfg._hwnd, video_track));
        _player->start();
        return true;
    }

}