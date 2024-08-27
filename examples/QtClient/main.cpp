//
// Created by li on 8/11/24.
//

#include <QApplication>
#include <QGridLayout>
#include <QWidget>
#include "mRTCClient.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    mRTCClient chat;
    chat.show();


    return app.exec();
}