//
// Created by li on 8/16/24.
//

#pragma once
#include "rtc_base/thread.h"

#include "api/istream_pusher.h"
#include "rtc_stream/push_stream.h"

#include "core/common.h"


namespace mRTC {
    class StreamPusher : public IStreamPusher,
                         public sigslot::has_slots<> {
    public:
        StreamPusher();
        ~StreamPusher() override;

        //IStreamPusher
        bool startPush(const std::string url, std::map<std::string, std::string>& reqBody, IVideoSource::Ptr ptr) override;
        bool stopPush() override;


        //信号槽
        void onAnswerReady(RtcStream::Ptr stream, const std::string& answerSdp);
        void onPcStateChange(RtcStream::Ptr stream, RtcStreamState state);

        void sendStartPushEvent(mRTCError err );
        void sendStopPushEvent(mRTCError err );

    private:
        ThreadPtr _currentThread;
        PushStream::Ptr _pushStream;
        std::string _host;
        std::string _uid;
        std::string _streamId;

    };


}