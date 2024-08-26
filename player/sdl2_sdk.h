//
// Created by li on 8/14/24.
//

#pragma once
#include <SDL.h>

namespace mRTC {
    class SDL2Sdk {
    public:
        ~SDL2Sdk() {
            freeSdk();
        }


        // 全局的初始化
        bool init();
        // 全局的反初始化
        void unInit();

        // 设置(windows)窗口
        bool set_window(const void* hwnd);
        // 设置图像格式(SDL_PIXELFORMAT_???)
        bool set_pix_fmt(size_t fmt, int w, int h);
        // 计算实际渲染视频的大小
        SDL_Rect renderRect();
        // 渲染数据，pitch是图像一行的字节大小，rect是渲染目标矩形，angle旋转角度，center旋转中心(在rect，{0,0}为左上)，flip翻转
        bool render(const uint8_t* y_plane, int Ypitch,
                    const uint8_t* u_plane, int Upitch,
                    const uint8_t* v_plane, int Vpitch,
                    const double angle = 0.0, const SDL_Point* center = NULL, const SDL_RendererFlip flip = SDL_FLIP_NONE);
        // 清理图像格式资源
        bool clear_pix_fmt();
        // 销毁关联资源
        bool detach_window();

        // 设置音频格式和处理回调
        bool set_audio_fmt(int freq, SDL_AudioFormat fmt, Uint8 channels, Uint16 samples, SDL_AudioCallback callback, void* userdata);
        // 开始音频播放
        bool start_audio();
        // 停止音频播放
        bool stop_audio();

        int freeSdk();

        bool isWindowNull() {
            return _win == nullptr;
        }

        void setRenderRect() {
            _renderRect = renderRect();
        }


    private:
        //这三个变量要在同一个线程上。
        SDL_Window* _win = nullptr;
        SDL_Renderer* _render = nullptr;
        SDL_Texture* _texture = nullptr;
        SDL_Rect _renderRect;
        int _width = 0;
        int _height = 0;

        SDL_AudioSpec reqspec_ = { 0 };
        SDL_AudioSpec recspec_ = { 0 };
    };

}

