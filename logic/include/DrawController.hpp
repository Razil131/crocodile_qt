#pragma once

#include <QObject>
#include "drawCommand.hpp"


class CROCODILE_BACK_EXPORT DrawController: public QObject{
    Q_OBJECT
signals:
    void drawCommandReceived(DrawCommand cmd);

public slots:
    void broadcastCommand(DrawCommand cmd) {
        emit drawCommandReceived(cmd); 
    }
};