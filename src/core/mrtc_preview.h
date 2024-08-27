//
// Created by li on 8/14/24.
//

#pragma once

#include "api/media_stream_interface.h"

#include "api/ipreview.h"
#include "api/common.h"
#include "common.h"
#include "player/sdl2_player.h"

namespace mRTC {
    class mRTCPreview :public IPreview {
    public:
        mRTCPreview(PlayerCfg cfg, rtc::VideoSourceInterface<webrtc::VideoFrame>* videoSrc);
        ~mRTCPreview() override;

        void start() override;
        void stop() override;
        void resize() override;

    private:

        std::unique_ptr<SDL2Player> _player;

    };

}

