#pragma once
#include <QObject>
#include "drawCommand.hpp"
#include "exportMacro.hpp"

class CROCODILE_BACK_EXPORT DrawController: public QObject{
    Q_OBJECT
public:
    void broadcastCommand(DrawCommand cmd) { 
        emit drawCommandReceived(cmd);
    }
signals:
    void drawCommandReceived(DrawCommand cmd);
};