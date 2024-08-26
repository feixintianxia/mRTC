//
// Created by li on 8/15/24.
//

#pragma once

#include "rtc_stream/rtc_stream.h"

namespace mRTC {
    class PullStream : public RtcStream {
    public:
        PullStream(const std::string uid, const std::string streamName);
        ~PullStream(){}
    };

}