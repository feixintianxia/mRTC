//
// Created by li on 8/15/24.
//

#include "rtc_base/logging.h"

#include "rtc_stream/push_stream.h"

namespace mRTC {

    PushStream::PushStream(const std::string uid, const std::string streamName)
            :
            RtcStream(uid, streamName) {
    }

    PushStream::~PushStream() {
    }

    void PushStream::addAudioTrack() {
        if (!_pc || !_pcFactory) {
            RTC_LOG(LS_ERROR) << " _pc or _pcFactory is nullptr ";
            return;
        }

        cricket::AudioOptions options;

        rtc::scoped_refptr<webrtc::AudioTrackInterface> track =
                _pcFactory->CreateAudioTrack("xchat_audio_track",
                                             _pcFactory->CreateAudioSource(options).get());
        auto retOrError = _pc->AddTrack(track, {"xchat_auido_stream_id"});
        if (!retOrError.ok()) {
            RTC_LOG(LS_ERROR) << " _pc->AddTrack failed: "
                              << retOrError.error().message();
        }
    }

    void PushStream::addVideoTrack(
            rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSrc) {
        if (!_pc || !_pcFactory) {
            RTC_LOG(LS_ERROR) << " _pc or _pcFactory is nullptr ";
            return;
        }

        rtc::scoped_refptr<webrtc::VideoTrackInterface> track =
                _pcFactory->CreateVideoTrack(videoSrc, "xchat_video_track");
        auto retOrError = _pc->AddTrack(track, {"xchat_vidoe_stream_id"});
        if (!retOrError.ok()) {
            RTC_LOG(LS_ERROR) << " _pc->AddTrack failed: "
                              << retOrError.error().message();
        }
    }

}

