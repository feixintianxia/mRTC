//
// Created by li on 8/12/24.
//

#pragma  once
#include <string>
#include <map>

#include "json/json.h"

namespace mRTC {


    struct DeviceInfo {
        std::string _id;
        std::string _name;
    };

    struct VideoCfg {
        std::string _deviceId;
        int _width;
        int _height;
        int _fps;
    };

    struct PlayerCfg {
        void* _hwnd;
    };



    enum class mRTCError {
        SUCC = 0,
        ErrVideoCamNotInit,
        ErrVideoCamNotStart,
        ErrVideoCamNotStop,
        ErrStartPush,
        ErrSendAnswer,
        ErrStopPush,
        ErrStartPull,
        ErrStopPull,
    };
    std::string xChatErrorToString(mRTCError error);

    enum class RtcStreamState {
        Connecting,   // 连接中
        Connected,    // 连接成功
        Disconnected, // 连接断开
        Failed,       // 连接失败
    };
    std::string RtcStreamStateToString(RtcStreamState state);

    std::map<std::string, std::string> jsonToMap(const std::string& jsonString);

}

