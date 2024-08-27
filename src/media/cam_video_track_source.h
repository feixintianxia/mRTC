//
// Created by li on 8/21/24.
//

#pragma once

#include "cam_video_source.h"

#include "pc/video_track_source.h"

namespace mRTC {
    class CamVideoTrackSource : public webrtc::VideoTrackSource {
    public:
        static rtc::scoped_refptr<CamVideoTrackSource> create(rtc::VideoSourceInterface<webrtc::VideoFrame>* camVideoSrc);

    protected:
        CamVideoTrackSource(rtc::VideoSourceInterface<webrtc::VideoFrame> * camVideoSrc);
        //webrtc::VideoTrackSource
        rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override;

    private:
        rtc::VideoSourceInterface<webrtc::VideoFrame>* _camVideoSource;
    };

}