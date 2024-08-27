//
// Created by li on 8/21/24.
//

#include "cam_video_track_source.h"

namespace mRTC {
    rtc::scoped_refptr<CamVideoTrackSource>
    CamVideoTrackSource::create(rtc::VideoSourceInterface<webrtc::VideoFrame> *camVideoSrc) {
        return rtc::make_ref_counted<CamVideoTrackSource>(camVideoSrc);
    }

    CamVideoTrackSource::CamVideoTrackSource(rtc::VideoSourceInterface<webrtc::VideoFrame> *camVideoSrc)
            : webrtc::VideoTrackSource(false),
              _camVideoSource(camVideoSrc) {

    }

    //webrtc::VideoTrackSource
    rtc::VideoSourceInterface<webrtc::VideoFrame> *CamVideoTrackSource::source() {
        return _camVideoSource;
    }


}