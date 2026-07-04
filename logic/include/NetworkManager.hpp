#pragma once
#include <QtCore/QObject>
#include <QtCore/QtTypes>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "drawCommand.hpp"
#include "GameState.hpp"


class NetworkManager : public QObject
{
    Q_OBJECT

private:
QTcpServer* server_ = nullptr;
QTcpSocket* socket_ = nullptr;
QHash<QTcpSocket*, quint16> nextBlockSizes_;
QVector<QTcpSocket*> clients_;

public:
explicit NetworkManager(QObject *parent = nullptr) : QObject(parent) {}
void startServer(quint16 port);
void connectToServer(QString ip_adress, quint16 port);
void sendDraw(const DrawCommand& cmd);
void sendMessage(const QString& text);
void sendState(const GameState& state);
void broadcast(const QByteArray &bytes, QTcpSocket* sender);

signals:
void drawCommandReceived(DrawCommand cmd); //для передачи рисования
void messageReceived(int senderId, const QString& senderName, const QString& text); //сообщение
void gameStateReceived(const GameState& state); //геймстет

private slots:
void newConnection();
void onReadyRead();
};