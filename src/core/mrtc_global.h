//
// Created by li on 8/12/24.
//

#pragma once

#include "api/common.h"
#include "api/imrtc_service.h"
#include "common.h"
#include "mrtc_http.h"

namespace mRTC {

    class mRTCGlobal {
    public:
        // 获取单例实例的静态方法
        static mRTCGlobal *getInstance() {
            static mRTCGlobal *const instance = new mRTCGlobal();
            return instance;
        }

        ThreadPtr apiThread() { return _apiThread; }

        ThreadPtr workThread() { return _workThread; }

        ThreadPtr netThread() { return _netThread; }

        TaskQueueFactorySPtr taskQueueFactory() { return _taskQueueFactory; }

        mRTCServiceObserver* xchatSvcObserver() { return _xchatSvcObserver; }

        void registermRTCServiceObserver(mRTCServiceObserver* observer) {
            _xchatSvcObserver = observer;
        }

        HttpService::Ptr httpSvc() { return _httpSvc; }


    private:
        mRTCGlobal();

        ~mRTCGlobal();

        // 禁止拷贝构造和赋值
        mRTCGlobal(const mRTCGlobal &) = delete;

        mRTCGlobal &operator=(const mRTCGlobal &) = delete;

    private:
        ThreadPtr _apiThread;
        ThreadPtr _workThread;
        ThreadPtr _netThread;
        TaskQueueFactorySPtr _taskQueueFactory;

        mRTCServiceObserver* _xchatSvcObserver = nullptr;
        HttpService::Ptr _httpSvc = nullptr;


    };

}


