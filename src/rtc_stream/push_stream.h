//
// Created by li on 8/15/24.
//

#pragma once

#include "rtc_stream.h"

namespace mRTC {

    class PushStream : public RtcStream {
    public:
        using Ptr = std::shared_ptr<PushStream>;
        PushStream(const std::string uid, const std::string streamName);

        ~PushStream() override;

        void addAudioTrack();

        void addVideoTrack( rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_source);
    };

}


