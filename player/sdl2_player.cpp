//
// Created by li on 8/14/24.
//

#include "rtc_base/logging.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_rotation.h"

#include "core/xchat_global.h"
#include "player/sdl2_player.h"

namespace mRTC {
    SDL2Player::SDL2Player(void *hwnd, rtc::VideoSourceInterface<webrtc::VideoFrame> *videoSrc)
            : _hwnd(hwnd),
              _videoSrc(videoSrc) {
        _sdl2Sdk.init();
        //_sdl2Sdk.set_window((_hwnd));
    }

    SDL2Player::~SDL2Player() {
        _running = false;
    }

    void SDL2Player::start() {
        auto fun = [=](){
            if (_running) {
                RTC_LOG(LS_ERROR) << " sdl2player already start, ignore";
                return;
            }

            _running = true;
            _videoSrc->AddOrUpdateSink(this, rtc::VideoSinkWants());
        };

        mRTCGlobal::getInstance()->workThread()->PostTask(fun);
    }

    void SDL2Player::stop() {
        auto fun = [=](){
            if (!_running) {
                return;
            }

            _running = false;
            _videoSrc->RemoveSink(this);
        };

        mRTCGlobal::getInstance()->workThread()->PostTask(fun);
    }

    void SDL2Player::resize() {
        auto fun = [=](){
            if (!_running) {
                return;
            }

            _width = 0;

        };

        mRTCGlobal::getInstance()->workThread()->PostTask(fun);
    }

    void SDL2Player::OnFrame(const webrtc::VideoFrame& frame) {
        auto fun = [=](){
            if (!_running) {
                return;
            }

            if (!initSdk(frame)) {
                return;
            }

            render(frame);

        };

        mRTCGlobal::getInstance()->workThread()->PostTask(fun);
    }

    bool SDL2Player::initSdk(const webrtc::VideoFrame& frame) {
        do {
            if (_sdl2Sdk.isWindowNull()) {
                break;
            }

            if (_width != frame.width() || _height != frame.height()) {
                break;
            }

            return true;

        } while (false);

        _width = frame.width();
        _height = frame.height();


        _sdl2Sdk.set_window((_hwnd));
        _sdl2Sdk.set_pix_fmt(SDL_PIXELFORMAT_IYUV, _width, _height);

        return true;
    }

    void SDL2Player::render(const webrtc::VideoFrame& frame) {
        rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(
                frame.video_frame_buffer()->ToI420());

        if (frame.rotation() != webrtc::kVideoRotation_0) {
            buffer = webrtc::I420Buffer::Rotate(*buffer, frame.rotation());
        }

        _sdl2Sdk.render(buffer->DataY(), buffer->StrideY(),
                        buffer->DataU(), buffer->StrideU(),
                        buffer->DataV(), buffer->StrideV());

    }



}
