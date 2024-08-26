//
// Created by li on 8/15/24.
//

#include "rtc_stream/pull_stream.h"

namespace mRTC {
    PullStream::PullStream(const std::string uid, const std::string streamName)
            : RtcStream(uid, streamName) {
    }

}