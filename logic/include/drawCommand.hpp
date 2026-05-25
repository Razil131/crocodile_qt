#pragma once
#include <QColor>

struct DrawCommand {
    enum Type { Start, Move, End, Clear, Fill };
    Type type;
    int x, y;
    QColor color;
    int width;
};