//
// Created by li on 8/24/24.
//

#pragma once
#include "rtc_base/thread.h"

#include "api/istream_puller.h"
#include "rtc_stream/pull_stream.h"

#include "common.h"
#include "player/sdl2_player.h"


namespace mRTC {
    class StreamPuller : public IStreamPuller,
                         public sigslot::has_slots<> {
    public:
        StreamPuller();
        ~StreamPuller() override;

        //IStreamPuller
        bool startPull(const std::string url, std::map<std::string, std::string> &reqBody, PlayerCfg cfg) override;
        bool stopPull() override;


        //信号槽
        void onAnswerReady(RtcStream::Ptr stream, const std::string& answerSdp);
        void onPcStateChange(RtcStream::Ptr stream, RtcStreamState state);
        void onAddNewTrack(RtcStream::Ptr stream,
                        rtc::scoped_refptr<webrtc::RtpTransceiverInterface> receiver);

        void sendStartPullEvent(mRTCError err );
        void sendStopPullEvent(mRTCError err );

        bool startPlayer(webrtc::VideoTrackInterface* video_track);

    private:
        ThreadPtr _currentThread;
        PullStream::Ptr _pullStream;
        std::string _host;
        std::string _uid;
        std::string _streamId;
        PlayerCfg _cfg;

        std::unique_ptr<SDL2Player> _player;

    };


}
