#pragma once
#include <QColor>
#include <QtCore/QtTypes>

struct DrawCommand {
    enum Type  : quint8 { 
        Start, 
        Move, 
        End, 
        Clear, 
        Fill 
    };
    Type type;
    int x, y;
    QColor color;
    int width;
};