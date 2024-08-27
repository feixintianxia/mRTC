//
// Created by li on 8/14/24.
//
#pragma once
#include <api/video/video_sink_interface.h>
#include <api/video/video_source_interface.h>
#include <api/video/video_frame.h>

#include "sdl2_sdk.h"

namespace mRTC {
    class SDL2Player : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
    public:
        SDL2Player(void *hwnd, rtc::VideoSourceInterface<webrtc::VideoFrame>* videoSrc);
        ~SDL2Player();

        void start();
        void stop();
        void resize();

        //rtc::VideoSinkInterface
        void OnFrame(const webrtc::VideoFrame& frame) override;

    private:
        bool initSdk(const webrtc::VideoFrame& frame);
        void render(const webrtc::VideoFrame& frame);

    private:
        void *_hwnd;
        rtc::VideoSourceInterface<webrtc::VideoFrame>* _videoSrc;

        SDL2Sdk _sdl2Sdk;
        bool _running = false;

        int _width;
        int _height;
    };

}