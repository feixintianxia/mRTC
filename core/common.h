//
// Created by li on 8/13/24.
//

#pragma once

#include <memory>

#include "rtc_base/thread.h"
#include "rtc_base/logging.h"
#include "api/task_queue/default_task_queue_factory.h"


namespace mRTC {
    // 使用 using 创建别名
    using ThreadPtr = std::shared_ptr<rtc::Thread>;
    using TaskQueueFactorySPtr = std::shared_ptr<webrtc::TaskQueueFactory>;

}