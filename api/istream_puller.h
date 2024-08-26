//
// Created by li on 8/24/24.
//

#pragma once

#include <map>
#include <memory>
#include "api/imedia_source.h"
#include "api/common.h"

namespace mRTC {

    class IStreamPuller : public std::enable_shared_from_this<IStreamPuller> {
    public:
        using Ptr = std::shared_ptr<IStreamPuller>;

        virtual ~IStreamPuller() = default;

        virtual bool startPull(const std::string url, std::map<std::string, std::string> &reqBody, PlayerCfg cfg) = 0;

        virtual bool stopPull() = 0;

    };


}
