//
// Created by li on 8/14/24.
//

#include "rtc_base/logging.h"
#include "sdl2_sdk.h"

namespace mRTC {
    bool SDL2Sdk::init() {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            RTC_LOG(LS_ERROR) << " SDL_Init error ";
            return false;
        }

        return true;
    }

    void SDL2Sdk::unInit() {
        SDL_Quit();
    }

    bool SDL2Sdk::set_window(const void *hwnd) {
        detach_window();
        _win = SDL_CreateWindowFrom(hwnd);
        if (!_win) {
            RTC_LOG(LS_ERROR) << " SDL_CreateWindowFrom hwnd: " << hwnd;
            return false;
        }

        _render = SDL_CreateRenderer(_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        /*
        if (!_render) {
            RTC_LOG(LS_ERROR) << " SDL_CreateRenderer ";
            detach_window();
            return false;
        }
         */

        return true;
    }

    bool SDL2Sdk::set_pix_fmt(size_t fmt, int w, int h) {
        clear_pix_fmt();
        if (!_render) {
            RTC_LOG(LS_ERROR) << " _render is nullptr";
            return false;
        }



        _width = w;
        _height = h;
        _texture = SDL_CreateTexture(_render, fmt, SDL_TEXTUREACCESS_STREAMING, w, h);
        if (!_texture) {
            RTC_LOG(LS_ERROR) << " _texture is nullptr";
            return false;
        }

        _renderRect = renderRect();
        return true;
    }

    SDL_Rect SDL2Sdk::renderRect() {
        // 获取窗口的当前宽高
        int windowWidth, windowHeight;
        SDL_GetWindowSize(_win, &windowWidth, &windowHeight);

        // 计算比例
        float scaleX = static_cast<float>(windowWidth) / _width;
        float scaleY = static_cast<float>(windowHeight) / _height;
        float scale = scaleX < scaleY ? scaleX : scaleY; // 按比例缩放

        // 计算目标矩形
        SDL_Rect dstRect;
        dstRect.w = static_cast<int>(_width * scale);
        dstRect.h = static_cast<int>(_height * scale);
        dstRect.x = (windowWidth - dstRect.w) / 2; // 居中显示
        dstRect.y = (windowHeight - dstRect.h) / 2;
        return dstRect;
    }

    bool SDL2Sdk::render(const uint8_t *y_plane, int Ypitch,
                         const uint8_t *u_plane, int Upitch,
                         const uint8_t *v_plane, int Vpitch,
                         const double angle, const SDL_Point *center, const SDL_RendererFlip flip) {
        if (_texture == nullptr || _render == nullptr) {
            RTC_LOG(LS_ERROR) << " _texture or _render is nullptr";
            return false;
        }

        if (SDL_UpdateYUVTexture(_texture, NULL,
                                 y_plane, Ypitch,
                                 u_plane, Upitch,
                                 v_plane, Vpitch) != 0) {
            RTC_LOG(LS_ERROR) << " SDL_UpdateYUVTexture ";
            return false;
        }


        if (SDL_RenderClear(_render) != 0) {
            RTC_LOG(LS_ERROR) << " SDL_RenderClear ";
            return false;
        }


        if (SDL_RenderCopyEx(_render, _texture, nullptr, &_renderRect, angle, center, flip) != 0) {
            RTC_LOG(LS_ERROR) << " SDL_RenderCopyEx ";
            return false;
        }
        SDL_RenderPresent(_render);
        return true;
    }

    bool SDL2Sdk::clear_pix_fmt() {
        if (_texture) {
            SDL_DestroyTexture(_texture);
            _texture = nullptr;
        }
        return true;
    }

    bool SDL2Sdk::detach_window() {
        if (_render) {
            SDL_DestroyRenderer(_render);
            _render = nullptr;
        }

        if (_win) {
            SDL_DestroyWindow(_win);
            _win = nullptr;
        }
        return true;
    }

    bool
    SDL2Sdk::set_audio_fmt(int freq, SDL_AudioFormat fmt, Uint8 channels, Uint16 samples, SDL_AudioCallback callback,
                           void *userdata) {
        reqspec_ = {0};
        recspec_ = {0};
        reqspec_.freq = freq;
        reqspec_.format = fmt;
        reqspec_.channels = channels;
        reqspec_.samples = samples;
        reqspec_.callback = callback;
        reqspec_.userdata = userdata;
        if (SDL_OpenAudio(&reqspec_, &recspec_) != 0) {
            RTC_LOG(LS_ERROR) << " SDL_OpenAudio ";
            return false;
        }
        return true;
    }

    bool SDL2Sdk::start_audio() {
        SDL_PauseAudio(0);
        return true;
    }

    bool SDL2Sdk::stop_audio() {
        SDL_PauseAudio(1);
        return true;
    }

    int SDL2Sdk::freeSdk() {
        SDL_DestroyRenderer(_render);
        SDL_DestroyTexture(_texture);
        SDL_DestroyWindow(_win);
        _render = nullptr;
        _texture = nullptr;
        _win = nullptr;
        SDL_Quit();
        return 0;
    }

}
