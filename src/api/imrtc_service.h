//
// Created by li on 8/12/24.
//

#pragma once
#include <vector>
#include <memory>

#include "common.h"
#include "imedia_source.h"
#include "ipreview.h"
#include "istream_pusher.h"
#include "istream_puller.h"

namespace mRTC {
    class mRTCServiceObserver : public std::enable_shared_from_this<mRTCServiceObserver> {
    public:
        using Ptr = std::shared_ptr<mRTCServiceObserver>;
        virtual ~mRTCServiceObserver() {}

        //摄像头监控事件
        virtual void onCamVideoStartEvent(IVideoSource::Ptr videoSource, mRTCError err) {}
        virtual void onCamVideoStopEvent(IVideoSource::Ptr videoSource, mRTCError err) {}

        //推流事件
        virtual void onPushStreamSate(IStreamPusher::Ptr pusher, RtcStreamState state ) {}
        virtual void onStartPushEvent(IStreamPusher::Ptr pusher, mRTCError err ) {}
        virtual void onStopPushEvent(IStreamPusher::Ptr pusher, mRTCError err ) {}

        //拉流事件
        virtual void onPullStreamSate(IStreamPuller::Ptr puller, RtcStreamState state ) {}
        virtual void onStartPullEvent(IStreamPuller::Ptr puller, mRTCError err ) {}
        virtual void onStopPullEvent(IStreamPuller::Ptr puller, mRTCError err ) {}
    };

    class ImRTCService {
    public:
        using UPtr = std::unique_ptr<ImRTCService>;
        virtual ~ImRTCService() {}

        //获取视频设备信息
        virtual std::vector<DeviceInfo> getVideoDevices() = 0;

        //获取音频设备信息
        virtual std::vector<DeviceInfo> getAudioDevices() = 0;

        //视频
        virtual IVideoSource::Ptr createVideoSource(const VideoCfg& cfg) = 0;
        virtual void destroyVideoSouce(IVideoSource* ptr) = 0;

        //预览
        virtual IPreview::Ptr createPreview(const PlayerCfg cfg, IVideoSource* ptr) = 0;
        virtual void destroyPreview(IPreview* ptr) = 0;

        //推流
        virtual IStreamPusher::Ptr createPusher() = 0;
        virtual void destroyPusher(IStreamPusher* ptr) = 0;

        //拉流
        virtual IStreamPuller::Ptr createPuller() = 0;
        virtual void destroyPuller(IStreamPuller* ptr) = 0;



    };

    ImRTCService::UPtr CreatemRTCService(mRTCServiceObserver* observer);
    void destroymRTCService(ImRTCService* ptr);



}

