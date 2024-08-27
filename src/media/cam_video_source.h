//
// Created by li on 8/12/24.
//


#pragma once

#include "api/media_stream_interface.h"
#include "modules/video_capture/video_capture.h"
#include "media/base/video_broadcaster.h"
#include <pc/video_track_source.h>

#include "api/common.h"
#include "api/imedia_source.h"
#include "core/common.h"

namespace mRTC {
    class CamVideoSource : public IVideoSource,
                           public rtc::VideoSinkInterface<webrtc::VideoFrame>,
                           public rtc::VideoSourceInterface<webrtc::VideoFrame> {
    public:
        CamVideoSource();
        ~CamVideoSource() override;

        bool start() override;

        bool stop() override;

        bool init(size_t width, size_t height, size_t fps, std::string deviceId);

        void unInit();

        //rtc::VideoSinkInterface
        void OnFrame(const webrtc::VideoFrame& frame) override;

        //rtc::VideoSourceInterface
        void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
                             const rtc::VideoSinkWants& wants) override;
        void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) override;

    private:
         rtc::Thread* _currentThread;
         std::string _deviceId;

         rtc::scoped_refptr<webrtc::VideoCaptureModule> _videoCamModule;
         webrtc::VideoCaptureCapability _videoCamCap;
         rtc::VideoBroadcaster _broadcaster;

    };

    using CamVideoSourceUPtr = std::unique_ptr<CamVideoSource>;
    CamVideoSourceUPtr createCamVideoSource(const VideoCfg& cfg);

}


