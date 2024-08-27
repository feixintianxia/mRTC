//
// Created by li on 8/12/24.
//

#include "mrtc_global.h"

namespace mRTC {

    mRTCGlobal::mRTCGlobal() :
            _apiThread(rtc::Thread::Create()),
            _workThread(rtc::Thread::Create()),
            _netThread(rtc::Thread::CreateWithSocketServer()),
            _taskQueueFactory(webrtc::CreateDefaultTaskQueueFactory()) {
        _apiThread->SetName("api_thread", nullptr);
        _apiThread->Start();

        _workThread->SetName("worker_thread", nullptr);
        _workThread->Start();

        _netThread->SetName("network_thread", nullptr);
        _netThread->Start();

        _httpSvc = std::make_shared<HttpService>();
        _httpSvc->start();
    }

    mRTCGlobal::~mRTCGlobal() {
        _apiThread->Stop();
        _workThread->Stop();
        _netThread->Stop();
    }

}
