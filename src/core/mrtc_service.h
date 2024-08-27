//
// Created by li on 8/12/24.
//

#pragma  once
#include <memory>
#include "api/imrtc_service.h"

#include "modules/video_capture/video_capture.h"
#include "modules/audio_device/include/audio_device.h"

namespace mRTC {
    class mRTCService : public ImRTCService {
    public:
        mRTCService();
        ~mRTCService() override;

        //获取视频设备信息
        std::vector<DeviceInfo> getVideoDevices() override;

        //获取音频设备信息
        std::vector<DeviceInfo> getAudioDevices() override;

        //视频
        IVideoSource::Ptr createVideoSource(const VideoCfg& cfg) override;
        void destroyVideoSouce(IVideoSource* ptr) override;

        //预览
        IPreview::Ptr createPreview(const PlayerCfg cfg, IVideoSource* ptr) override;
        void destroyPreview(IPreview* ptr) override;

        //推流
        IStreamPusher::Ptr createPusher() override;
        void destroyPusher(IStreamPusher* ptr) override;

        //拉流
        IStreamPuller::Ptr createPuller() override;
        void destroyPuller(IStreamPuller* ptr) override;

    private:
        std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> _videoDevices;
        rtc::scoped_refptr<webrtc::AudioDeviceModule> _audioModule;

    };

}



