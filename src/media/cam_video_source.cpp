//
// Created by li on 8/12/24.
//

#include "modules/video_capture/video_capture_factory.h"
#include "cam_video_source.h"
#include "rtc_base/logging.h"

#include "cam_video_source.h"
#include "core/mrtc_global.h"

namespace mRTC {

    CamVideoSourceUPtr createCamVideoSource(const VideoCfg& cfg) {
        auto p = std::make_unique<CamVideoSource>();
        if (!p->init(cfg._width, cfg._height, cfg._fps, cfg._deviceId)) {
            return nullptr;
        }

        return p;
    }

    CamVideoSource::CamVideoSource() :
            _currentThread(rtc::Thread::Current()) {

    }
    CamVideoSource::~CamVideoSource() {
        unInit();
    }

    bool CamVideoSource::init(size_t width, size_t height, size_t fps, std::string deviceId) {
        _deviceId = deviceId;

        _videoCamModule = webrtc::VideoCaptureFactory::Create(_deviceId.c_str());
        if (!_videoCamModule) {
            RTC_LOG(LS_ERROR) << " Failed to create the video cam with device ID: " << _deviceId;
            return false;
        }

        _videoCamModule->RegisterCaptureDataCallback(this);

        std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> deviceInfo;
        deviceInfo.reset(webrtc::VideoCaptureFactory::CreateDeviceInfo());
        deviceInfo->GetCapability(_videoCamModule->CurrentDeviceName(), 0, _videoCamCap);

        _videoCamCap.width = width;
        _videoCamCap.height = height;
        _videoCamCap.maxFPS = fps;
        _videoCamCap.videoType = webrtc::VideoType::kI420;

        return true;
    }

    void CamVideoSource::unInit() {
        if(!_videoCamModule) {
            return;
        }

        _videoCamModule->StopCapture();
        _videoCamModule->DeRegisterCaptureDataCallback();
        _videoCamModule = nullptr;
    }

    bool CamVideoSource::start() {

        auto fun = [=]() {
            auto err = mRTCError::SUCC;

            do {
                if(!_videoCamModule) {
                    RTC_LOG(LS_ERROR) << " video cam fail to init with device ID: " << _deviceId;
                    err = mRTCError::ErrVideoCamNotInit;
                    break;
                }

                int32_t ret = _videoCamModule->StartCapture(_videoCamCap);
                if (ret != 0) {
                    RTC_LOG(LS_ERROR) << " video cam fail to start with device ID: " << _deviceId;
                    err = mRTCError::ErrVideoCamNotStart;
                    break;
                }

                RTC_LOG(LS_INFO) << " video cam  start with device ID: " << _deviceId;
            }while(false);

            auto observer = mRTCGlobal::getInstance()->xchatSvcObserver();
            if (observer) {
                observer->onCamVideoStartEvent(shared_from_this(), err);
            }

        };


        _currentThread->PostTask(fun);
        return true;
    }

    bool CamVideoSource::stop() {
        auto fun = [=](){
            auto err = mRTCError::SUCC;
            if(_videoCamModule) {
               if(0 != _videoCamModule->StopCapture()) {
                   err = mRTCError::ErrVideoCamNotStop;
               }
            }

            auto observer = mRTCGlobal::getInstance()->xchatSvcObserver();
            if (observer) {
                observer->onCamVideoStopEvent(shared_from_this(), err);
            }
        };

        _currentThread->PostTask(fun);
        return true;
    }


    //rtc::VideoSinkInterface
    void CamVideoSource::OnFrame(const webrtc::VideoFrame& frame) {
        _broadcaster.OnFrame(frame);
    }

    //rtc::VideoSourceInterface
    void CamVideoSource::AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
                         const rtc::VideoSinkWants& wants) {
        _broadcaster.AddOrUpdateSink(sink, wants);

    }

    void CamVideoSource::RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) {
        _broadcaster.RemoveSink(sink);
    }





}