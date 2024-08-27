//
// Created by li on 8/12/24.
//

#include <memory>

#include "api/media_stream_interface.h"
#include "modules/video_capture/video_capture_factory.h"

#include "api/imrtc_service.h"
#include "mrtc_global.h"
#include "mrtc_service.h"
#include "mrtc_preview.h"
#include "stream_pusher.h"
#include "stream_puller.h"
#include "media/cam_video_source.h"


namespace mRTC{
    ImRTCService::UPtr CreatemRTCService(mRTCServiceObserver* observer) {
        auto fun = [=]() {
            mRTCGlobal::getInstance()->registermRTCServiceObserver(observer);
            return std::make_unique<mRTCService>();
        };
        return mRTCGlobal::getInstance()->apiThread()->BlockingCall(fun);
    }
    void destroymRTCService(ImRTCService* ptr) {
        auto fun = [=]() {
            if (ptr) {
                delete ptr;
            }
        };
        return mRTCGlobal::getInstance()->apiThread()->BlockingCall(fun);

    }
    mRTCService::mRTCService() {
        _videoDevices.reset(webrtc::VideoCaptureFactory::CreateDeviceInfo());
        _audioModule = webrtc::AudioDeviceModule::Create(
                webrtc::AudioDeviceModule::kPlatformDefaultAudio,
                mRTCGlobal::getInstance()->taskQueueFactory().get());
        if (_audioModule) {
            _audioModule->Init();
        }

        /*
         _audioModule = mRTCGlobal::getInstance()->workThread()->BlockingCall([](){
             return webrtc::AudioDeviceModule::Create(
                   webrtc::AudioDeviceModule::kPlatformDefaultAudio,
                   mRTCGlobal::getInstance()->taskQueueFactory().get());

         });

         auto fun = [this](){
             _audioModule->Init();
         };
        mRTCGlobal::getInstance()->workThread()->BlockingCall(fun);
         */





    }

    mRTCService::~mRTCService() {

    }

    std::vector<DeviceInfo> mRTCService::getVideoDevices() {
        auto fun = [=](){
            std::vector<DeviceInfo> devices;

            int32_t totalNum = _videoDevices->NumberOfDevices();
            if (0 == totalNum) {
                return devices;
            }

            char id[256] = {0};
            char name[256] = {0};
            for (int32_t index = 0; index < totalNum; ++index) {
                int32_t res = _videoDevices->GetDeviceName(index, name, sizeof(name), id, sizeof(id));
                if (res != 0) {
                    continue;
                }
                devices.emplace_back(DeviceInfo{._id = id, ._name = name});
            }

            return devices;

        };
        return mRTCGlobal::getInstance()->apiThread()->BlockingCall(fun);
    }

    std::vector<DeviceInfo> mRTCService::getAudioDevices() {
        auto fun = [=](){
            std::vector<DeviceInfo> devices;

            int32_t totalNum = _audioModule->RecordingDevices();
            if (0 == totalNum) {
                return devices;
            }

            char id[256] = {0};
            char name[256] = {0};
            for (int32_t index = 0; index < totalNum; ++index) {
                int32_t res = _audioModule->RecordingDeviceName(index, name, id);
                if (res != 0) {
                    continue;
                }
                devices.emplace_back(DeviceInfo{._id = id, ._name = name});
            }

            return devices;
        };

        return mRTCGlobal::getInstance()->apiThread()->BlockingCall(fun);
    }

    //视频
    IVideoSource::Ptr mRTCService::createVideoSource(const VideoCfg& cfg){
        auto fun = [&cfg](){
            return createCamVideoSource(cfg);
        } ;

        return mRTCGlobal::getInstance()->apiThread()->BlockingCall(fun);

    }

    void mRTCService::destroyVideoSouce(IVideoSource* ptr) {
        auto fun = [ptr]() {
            if (ptr) {
                delete ptr;
            }
        };
        return mRTCGlobal::getInstance()->apiThread()->PostTask(fun);
    }

    //预览
    IPreview::Ptr mRTCService::createPreview(const PlayerCfg cfg, IVideoSource* ptr){
        auto fun = [=](){
            return std::make_shared<mRTCPreview>(cfg,
                                                  dynamic_cast<rtc::VideoSourceInterface<webrtc::VideoFrame>*>(ptr));
        } ;

        return mRTCGlobal::getInstance()->apiThread()->BlockingCall(fun);

    }

    void mRTCService::destroyPreview(IPreview* ptr) {
        auto fun = [ptr]() {
            if (ptr) {
                delete ptr;
            }
        };
        return mRTCGlobal::getInstance()->apiThread()->PostTask(fun);
    }
    IStreamPusher::Ptr mRTCService::createPusher()  {
        auto fun = [=](){
            return std::make_shared<StreamPusher>();
        } ;

        return mRTCGlobal::getInstance()->apiThread()->BlockingCall(fun);

    }
    void mRTCService::destroyPusher(IStreamPusher* ptr) {
        auto fun = [ptr]() {
            if (ptr) {
                delete ptr;
            }
        };
        return mRTCGlobal::getInstance()->apiThread()->PostTask(fun);

    };

    IStreamPuller::Ptr mRTCService::createPuller()  {
        auto fun = [=](){
            return std::make_shared<StreamPuller>();
        } ;

        return mRTCGlobal::getInstance()->apiThread()->BlockingCall(fun);

    }
    void mRTCService::destroyPuller(IStreamPuller* ptr) {
        auto fun = [ptr]() {
            if (ptr) {
                delete ptr;
            }
        };
        return mRTCGlobal::getInstance()->apiThread()->PostTask(fun);

    };


}



