#pragma once
#include <QtCore/QtTypes>
#include "drawCommand.hpp"
#include <QDataStream>

enum class NetworkTypes : quint8{
    Message_,
    Draw_,
    State_,
    Nickname_,
    IdAssignment_,
    WordSelected_,
    ConnectionRejected_
};

inline QDataStream& operator<<(QDataStream &out, const DrawCommand& cmd){
    out << cmd.x << cmd.y << cmd.color << cmd.width << static_cast<quint8>(cmd.type);
    return out;
}

inline QDataStream &operator>>(QDataStream &in, DrawCommand &cmd) {
    quint8 rawType;
    in >> cmd.x >> cmd.y >> cmd.color >> cmd.width >> rawType;
    cmd.type = static_cast<DrawCommand::Type>(rawType);
    return in;
}

