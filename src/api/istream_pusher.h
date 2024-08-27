//
// Created by li on 8/16/24.
//

#pragma once

#include <map>
#include <memory>
#include "imedia_source.h"

namespace mRTC {

    class IStreamPusher : public std::enable_shared_from_this<IStreamPusher> {
    public:
        using Ptr = std::shared_ptr<IStreamPusher>;

        virtual ~IStreamPusher() = default;

        virtual bool startPush(const std::string url, std::map<std::string, std::string> &reqBody, IVideoSource::Ptr ptr) = 0;

        virtual bool stopPush() = 0;

    };


}