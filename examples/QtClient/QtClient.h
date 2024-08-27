//
// Created by li on 8/11/24.
//

#pragma once

#include <QDialog>
#include <QWidget>
#include <QtWidgets/QLabel>
#include "ui_QtWidget.h"

#include "mRTC/api/ixchat_service.h"

class mRTCClient : public QWidget,
                    public mRTC::mRTCServiceObserver {
Q_OBJECT

public:
    mRTCClient(QWidget *parent = nullptr);

    ~mRTCClient();

    //observer
    //摄像头监控事件
    void onCamVideoStartEvent(mRTC::IVideoSource::Ptr videoSource, mRTC::mRTCError err) override;
    void onCamVideoStopEvent(mRTC::IVideoSource::Ptr videoSource, mRTC::mRTCError err) override;
    //推流事件
    void onPushStreamSate(mRTC::IStreamPusher::Ptr pusher, mRTC::RtcStreamState state);
    void onStartPushEvent(mRTC::IStreamPusher::Ptr pusher, mRTC::mRTCError err);
    void onStopPushEvent(mRTC::IStreamPusher::Ptr pusher, mRTC::mRTCError err);
    //拉流事件
    void onPullStreamSate(mRTC::IStreamPuller::Ptr puller, mRTC::RtcStreamState state );
    void onStartPullEvent(mRTC::IStreamPuller::Ptr puller, mRTC::mRTCError err );
    void onStopPullEvent(mRTC::IStreamPuller::Ptr puller, mRTC::mRTCError err );

private:
    void init();

    void initUI();

    void initSignalSlots();

    void initVideoWindows(int maxCount);

    void initDevicesInfo();

    void setStatus(const std::string info, bool isSucc = true);

    void resizeEvent(QResizeEvent *event) override;


public slots:

    void onClickedBtnDevices();

    void onClickedBtnPreview();

    void onClickedBtnPush();

    void onClickedBtnPull();

private:
    Ui::mRTCClient _ui;

    QWidget **_childWindows = nullptr;

    mRTC::ImRTCService::UPtr _xchatSvc = nullptr;
    mRTC::IVideoSource::Ptr _camVideoSrc = nullptr;
    mRTC::IPreview::Ptr _preview = nullptr;
    mRTC::IStreamPusher::Ptr _pusher = nullptr;
    mRTC::IStreamPuller::Ptr _puller = nullptr;


};