//
// Created by li on 8/14/24.
//

#pragma once
#include <memory>

namespace mRTC {
    class IPreview {
    public:
        using Ptr = std::shared_ptr<IPreview>;

        virtual ~IPreview() {}

        virtual void start() = 0;

        virtual void stop() = 0;

        virtual void resize() = 0;
    };
}