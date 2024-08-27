//
// Created by li on 8/12/24.
//

#pragma once

#include <memory>

namespace mRTC {
    class IMediaSource {
    public:
        virtual ~IMediaSource() {}

        virtual bool start() = 0;

        virtual bool stop() = 0;
    };

    class IVideoSource : public std::enable_shared_from_this<IVideoSource>,
                         public IMediaSource {
    public:
        using Ptr = std::shared_ptr<IVideoSource>;
    };

    class IAudioSource : public std::enable_shared_from_this<IAudioSource>, public IMediaSource {
    public:
        using Ptr = std::shared_ptr<IAudioSource>;

    };
}