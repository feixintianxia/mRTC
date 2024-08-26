//
// Created by li on 8/14/24.
//

#include "core/xchat_preview.h"

namespace mRTC {
    mRTCPreview::mRTCPreview(PlayerCfg cfg,
            rtc::VideoSourceInterface<webrtc::VideoFrame>* videoSrc) {
        _player.reset(new SDL2Player(cfg._hwnd, videoSrc));
    }

    mRTCPreview::~mRTCPreview() {
    }

    void mRTCPreview::start() {
        _player->start();
    }

    void mRTCPreview::stop() {
        _player->stop();
    }

    void mRTCPreview::resize() {
        _player->resize();
    }

}
