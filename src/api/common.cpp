//
// Created by li on 8/12/24.
//
#include "rtc_base/logging.h"

#include "common.h"


namespace mRTC {

    class Initializer {
    public:
        Initializer() {
            /*显示时间戳*/
            rtc::LogMessage::LogTimestamps(true);

            /*显示线程id*/
            rtc::LogMessage::LogThreads(true);

            RTC_LOG(LS_INFO) << "mRTC Initializer: Startup code executed before main.";
        }
    };

    // 创建一个静态对象，以确保构造器在 main 之前被调用
    static Initializer initializer;

    // 将 mRTCError 枚举值转换为 std::string
    std::string xChatErrorToString(mRTCError error) {
        switch (error) {
            case mRTCError::SUCC:
                return "SUCC";
            case mRTCError::ErrVideoCamNotInit:
                return "ErrVideoCamNotInit";
            case mRTCError::ErrVideoCamNotStart:
                return "ErrVideoCamNotStart";
            case mRTCError::ErrVideoCamNotStop:
                return "ErrVideoCamNotStop";
            default:
                return "Unknown Error";
        }
    }

    // 将 RtcStreamState 转换为字符串
    std::string rtcStreamStateToString(RtcStreamState state) {
        switch (state) {
            case RtcStreamState::Connecting:
                return "Connecting";
            case RtcStreamState::Connected:
                return "Connected";
            case RtcStreamState::Disconnected:
                return "Disconnected";
            case RtcStreamState::Failed:
                return "Failed";
            default:
                return "Unknown State";
        }
    }

    std::map<std::string, std::string> jsonToMap(const std::string& jsonString) {
        Json::Value jsonData;
        Json::CharReaderBuilder readerBuilder;
        std::string errs;

        // 创建 JSON 解析器
        std::istringstream jsonStream(jsonString);
        if (!Json::parseFromStream(readerBuilder, jsonStream, &jsonData, &errs)) {
            throw std::runtime_error("Failed to parse JSON: " + errs);
        }

        std::map<std::string, std::string> dataMap;

        // 遍历 JSON 对象并填充 map
        for (const auto& key : jsonData.getMemberNames()) {
            if (jsonData[key].isString()) {
                dataMap[key] = jsonData[key].asString();
            } else {
                dataMap[key] = jsonData[key].toStyledString(); // 处理非字符串类型
            }
        }

        return dataMap;
    }

}
