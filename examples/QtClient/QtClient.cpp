//
// Created by li on 8/11/24.
//

#include <QScreen>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <math.h>
#include <sstream>
#include "QtClient.h"

const double mainWindowScale = 0.8;
const int maxWindowsCount = 4;

const QString backGroundBlue = "background-color: lightblue;";
const QString backGroundRed = "background-color: red;";

const QString styleSheetRed = "color: red;";
const QString styleSheetBlack = "color: black;";
const QString styleSheetGreen = "color: green;";


class ButtonController {
public:
    ButtonController(QPushButton *btn) : button(btn) {
        button->setDisabled(true); // 构造函数中禁用按钮
    }

    ~ButtonController() {
        button->setEnabled(true); // 析构函数中启用按钮
    }

private:
    QPushButton *button; // 持有按钮指针
};

QtClient::QtClient(QWidget *parent) : QWidget(parent) {
    init();
}

QtClient::~QtClient() {
    if (_preview) {
        //关闭
        _preview->stop();
        _xchatSvc->destroyPreview(_preview.get());
        _preview = nullptr;
    }

    if (_camVideoSrc) {
        //关闭
        _camVideoSrc->stop();
        _xchatSvc->destroyVideoSouce(_camVideoSrc.get());
        _camVideoSrc = nullptr;
    }

    if (_pusher) {
        _pusher->stopPush();
        _xchatSvc->destroyPusher(_pusher.get());
        _pusher = nullptr;
    }

    XChat::destroyXChatService(_xchatSvc.release());

}

void QtClient::init() {
    _xchatSvc = XChat::CreateXChatService(this);
    initUI();
    initSignalSlots();
    setWindowTitle("QtClient客户端");

}

void QtClient::initUI() {
    _ui.setupUi(this);
    // 获取屏幕尺寸
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QSize screenSize = screen->size();

        // 计算 0.6 倍的尺寸
        int width = static_cast<int>(screenSize.width() * mainWindowScale);
        int height = static_cast<int>(screenSize.height() * mainWindowScale);

        // 设置主窗口的大小
        this->resize(width, height);
    }


    //设置主布局
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(_ui.top_video);
    vLayout->addWidget(_ui.middle_setting);
    vLayout->addWidget(_ui.buttom_status);
    setLayout(vLayout);

    _ui.middle_setting->setMaximumHeight(250);
    _ui.buttom_status->setMaximumHeight(40);

    // 创建一个网格布局
    QHBoxLayout *status_layout = new QHBoxLayout();
    // 将标签添加到布局中
    status_layout->addWidget(_ui.status_tip1);
    status_layout->addWidget(_ui.status_tip2);
    status_layout->addWidget(_ui.status_tip3);
    // 指定标签在布局中的扩展和对齐方式
    //status_layout->setStretch( 0, 1);
    //status_layout->setStretch( 1, 1);
    //status_layout->setStretch( 2, 1);
    // 设置布局的边距
    status_layout->setContentsMargins(0, 0, 0, 0);

    // 将布局应用到按钮上
    _ui.buttom_status->setLayout(status_layout);

    initVideoWindows(maxWindowsCount);
    initDevicesInfo();
}

void QtClient::initVideoWindows(int num) {
    _childWindows = new QWidget *[maxWindowsCount];
    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(1);
    layout->setContentsMargins(1, 1, 1, 1);
    _ui.top_video->setLayout(layout);

    int cols = sqrt(num);
    for (int i = 0; i < num; ++i) {
        _childWindows[i] = new QWidget();
        _childWindows[i]->setStyleSheet("background-color: rgb(50, 50, 50)");
        layout->addWidget(_childWindows[i], i / cols, i % cols);
    }
}

void QtClient::initDevicesInfo() {
    std::vector<XChat::DeviceInfo> videoDevices =
            _xchatSvc->getVideoDevices();
    for (auto info: videoDevices) {
        _ui.cam_devices->addItem(QString(info._name.c_str()),
                                 info._id.c_str());
    }

    std::vector<XChat::DeviceInfo> audioDevices = _xchatSvc->getAudioDevices();
    for (auto info: audioDevices) {
        _ui.mic_devices->addItem(QString(info._name.c_str()),
                                 info._id.c_str());
    }

}

void QtClient::setStatus(const std::string info, bool isSucc) {
    if (info.empty()) {
        _ui.status_tip2->clear();
        return;
    }

    if (isSucc) {
        _ui.status_tip2->setStyleSheet(styleSheetGreen);
    } else {
        _ui.status_tip2->setStyleSheet(styleSheetRed);
    }

    _ui.status_tip2->setText(QString::fromUtf8(info.c_str()));
}

void QtClient::resizeEvent(QResizeEvent *event) {
    if (_preview) {
        _preview->resize();
    }
}


void QtClient::initSignalSlots() {
    //connect(_ui.btn_devices, SIGNAL(clicked()), this, SLOT(onClickedBtnDevices));
    connect(_ui.btn_devices, &QPushButton::clicked, this, &QtClient::onClickedBtnDevices);
    connect(_ui.btn_preview, &QPushButton::clicked, this, &QtClient::onClickedBtnPreview);
    connect(_ui.btn_start_push, &QPushButton::clicked, this, &QtClient::onClickedBtnPush);
    connect(_ui.btn_start_pull_1, &QPushButton::clicked, this, &QtClient::onClickedBtnPull);
}


//slots
void QtClient::onClickedBtnDevices() {
    //防止多次点击按钮
    ButtonController btnCtrl(_ui.btn_devices);

    if (_camVideoSrc) {
        //关闭
        _camVideoSrc->stop();
        _xchatSvc->destroyVideoSouce(_camVideoSrc.get());
        _camVideoSrc = nullptr;
    } else {
        XChat::VideoCfg cfg;
        cfg._deviceId = _ui.cam_devices->currentData().toString().toUtf8();
        cfg._width = 640;
        cfg._height = 480;
        cfg._fps = 24;
        _camVideoSrc = _xchatSvc->createVideoSource(cfg);
        _camVideoSrc->start();
    }

}

void QtClient::onClickedBtnPreview() {
    //防止多次点击按钮
    ButtonController btnCtrl(_ui.btn_devices);

    if (!_camVideoSrc) {
        setStatus("还没有启动音视频设备！", false);
        return;
    }

    if (_preview) {
        //关闭
        _preview->stop();
        _xchatSvc->destroyPreview(_preview.get());
        _preview = nullptr;
        _ui.btn_preview->setStyleSheet(backGroundBlue);
        _ui.btn_preview->setText(QString::fromUtf8("开启预览"));
        setStatus("关闭预览成功。");
    } else {
        XChat::PlayerCfg cfg;
        cfg._hwnd = (void *) _childWindows[0]->winId();
        _preview = _xchatSvc->createPreview(cfg, _camVideoSrc.get());
        _preview->start();
        _ui.btn_preview->setStyleSheet(backGroundRed);
        _ui.btn_preview->setText(QString::fromUtf8("关闭预览"));
        setStatus("开启预览成功。");
    }

}

void QtClient::onClickedBtnPush() {
    //防止多次点击按钮
    ButtonController btnCtrl(_ui.btn_start_push);

    if (!_camVideoSrc) {
        setStatus("还没有启动音视频设备！", false);
        return;
    }

    if (_pusher) {
        //关闭
        _pusher->stopPush();
    } else {
        _pusher = _xchatSvc->createPusher();

        std::string host = _ui.edit_server_url->text().toStdString();
        std::string uid = _ui.edit_uid->text().toStdString();
        std::string streamId = _ui.edit_streamid->text().toStdString();
        std::map<std::string, std::string> reqBody;
        reqBody["uid"] = uid;
        reqBody["streamid"] = streamId;
        reqBody["video"] = "1";
        reqBody["audio"] = "1";

        _pusher->startPush(host, reqBody, _camVideoSrc);
    }


}

void QtClient::onClickedBtnPull() {
    //防止多次点击按钮
    ButtonController btnCtrl(_ui.btn_start_pull_1);


    if (_puller) {
        //关闭
        _puller->stopPull();
    } else {
        _puller = _xchatSvc->createPuller();

        std::string host = _ui.edit_server_url->text().toStdString();
        std::string uid = _ui.edit_uid->text().toStdString();
        std::string streamId = _ui.edit_pull_stream_1->text().toStdString();
        std::map<std::string, std::string> reqBody;
        reqBody["uid"] = uid;
        reqBody["streamid"] = streamId;
        reqBody["video"] = "1";
        reqBody["audio"] = "1";

        XChat::PlayerCfg cfg;
        cfg._hwnd = (void *) _childWindows[1]->winId();

        _puller->startPull(host, reqBody, cfg);
    }

}

//observer
void QtClient::onCamVideoStartEvent(XChat::IVideoSource::Ptr videoSource, XChat::XChatError err) {
    auto fun = [=]() {
        if (err == XChat::XChatError::SUCC) {
            _ui.btn_devices->setStyleSheet(backGroundRed);
            _ui.btn_devices->setText(QString::fromUtf8("关闭音视频设备"));
            setStatus("开启音视频设备成功。");
        } else {
            std::ostringstream os;
            os << "开启音视频设备失败。错误码： " << XChat::xChatErrorToString(err);
            setStatus(os.str(), false);

        }

    };

    //回到主线程执行
    QMetaObject::invokeMethod(this, fun);
}

void QtClient::onCamVideoStopEvent(XChat::IVideoSource::Ptr videoSource, XChat::XChatError err) {
    auto fun = [=]() {
        if (err == XChat::XChatError::SUCC) {
            _ui.btn_devices->setStyleSheet(backGroundBlue);
            _ui.btn_devices->setText(QString::fromUtf8("开启音视频设备"));
            setStatus("关闭音视频设备成功。");
        } else {
            std::ostringstream os;
            os << "关闭音视频设备失败。错误码： " << XChat::xChatErrorToString(err);
            setStatus(os.str(), false);

        }

    };

    //回到主线程执行
    QMetaObject::invokeMethod(this, fun);

}

void QtClient::onPushStreamSate(XChat::IStreamPusher::Ptr pusher, XChat::RtcStreamState state) {
    QMetaObject::invokeMethod(this, [=]() {
        if (state == XChat::RtcStreamState::Connecting) {
            setStatus("正在推流...");
        } else if (state == XChat::RtcStreamState::Connected) {
            _ui.btn_start_push->setStyleSheet(backGroundRed);
            _ui.btn_start_push->setText(QString::fromUtf8("停止推流"));
            setStatus("推流成功。");
        } else if (state == XChat::RtcStreamState::Disconnected) {
            _ui.btn_start_push->setStyleSheet(backGroundRed);
            _ui.btn_start_push->setText(QString::fromUtf8("停止推流"));
            setStatus("推流中断。", false);
        } else if (state == XChat::RtcStreamState::Failed) {
            _ui.btn_start_push->setStyleSheet(backGroundRed);
            _ui.btn_start_push->setText(QString::fromUtf8("停止推流"));
            setStatus("推流失败。", false);
        }
    });


}

void QtClient::onStartPushEvent(XChat::IStreamPusher::Ptr pusher, XChat::XChatError err) {
    auto fun = [=]() {
        if (err == XChat::XChatError::SUCC) {
            setStatus("成功发送推流指令。");
        } else {
            std::ostringstream os;
            os << "推流失败。错误码： " << XChat::xChatErrorToString(err);
            setStatus(os.str(), false);

            _ui.btn_start_push->setStyleSheet(backGroundBlue);
            _ui.btn_start_push->setText(QString::fromUtf8("开始推流"));

            _xchatSvc->destroyPusher(_pusher.get());
            _pusher = nullptr;
        }

    };

    //回到主线程执行
    QMetaObject::invokeMethod(this, fun);

}

void QtClient::onStopPushEvent(XChat::IStreamPusher::Ptr pusher, XChat::XChatError err) {
    auto fun = [=]() {
        if (err == XChat::XChatError::SUCC) {
            setStatus("成功停止推流。");
        } else {
            std::ostringstream os;
            os << "停止推流失败。错误码： " << XChat::xChatErrorToString(err);
            setStatus(os.str(), false);

        }

        _ui.btn_start_push->setStyleSheet(backGroundBlue);
        _ui.btn_start_push->setText(QString::fromUtf8("开始推流"));

        _xchatSvc->destroyPusher(_pusher.get());
        _pusher = nullptr;

    };

    //回到主线程执行
    QMetaObject::invokeMethod(this, fun);

}

void QtClient::onPullStreamSate(XChat::IStreamPuller::Ptr puller, XChat::RtcStreamState state ) {
    QMetaObject::invokeMethod(this, [=]() {
        if (state == XChat::RtcStreamState::Connecting) {
            setStatus("正在拉流...");
        } else if (state == XChat::RtcStreamState::Connected) {
            _ui.btn_start_pull_1->setStyleSheet(backGroundRed);
            _ui.btn_start_pull_1->setText(QString::fromUtf8("停止拉流"));
            setStatus("拉流成功。");
        } else if (state == XChat::RtcStreamState::Disconnected) {
            _ui.btn_start_pull_1->setStyleSheet(backGroundRed);
            _ui.btn_start_pull_1->setText(QString::fromUtf8("停止拉流"));
            setStatus("拉流中断。", false);
        } else if (state == XChat::RtcStreamState::Failed) {
            _ui.btn_start_pull_1->setStyleSheet(backGroundRed);
            _ui.btn_start_pull_1->setText(QString::fromUtf8("停止拉流"));
            setStatus("拉流失败。", false);
        }
    });


}
void QtClient::onStartPullEvent(XChat::IStreamPuller::Ptr puller, XChat::XChatError err ) {
    auto fun = [=]() {
        if (err == XChat::XChatError::SUCC) {
            setStatus("成功发送拉流指令。");
        } else {
            std::ostringstream os;
            os << "拉流失败。错误码： " << XChat::xChatErrorToString(err);
            setStatus(os.str(), false);

            _ui.btn_start_pull_1->setStyleSheet(backGroundBlue);
            _ui.btn_start_pull_1->setText(QString::fromUtf8("开始拉流"));

            _xchatSvc->destroyPuller(_puller.get());
            _puller = nullptr;
        }

    };

    //回到主线程执行
    QMetaObject::invokeMethod(this, fun);

}
void QtClient::onStopPullEvent(XChat::IStreamPuller::Ptr puller, XChat::XChatError err ) {
    auto fun = [=]() {
        if (err == XChat::XChatError::SUCC) {
            setStatus("成功停止拉流。");
        } else {
            std::ostringstream os;
            os << "停止拉流失败。错误码： " << XChat::xChatErrorToString(err);
            setStatus(os.str(), false);

        }

        _ui.btn_start_pull_1->setStyleSheet(backGroundBlue);
        _ui.btn_start_pull_1->setText(QString::fromUtf8("开始拉流"));

        _xchatSvc->destroyPuller(_puller.get());
        _puller = nullptr;

    };

    //回到主线程执行
    QMetaObject::invokeMethod(this, fun);

}

