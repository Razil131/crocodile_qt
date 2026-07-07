#pragma once
#include <QtCore/QObject>
#include <QtCore/QtTypes>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QHash>
#include <QtCore/QVector>

#include "drawCommand.hpp"
#include "GameState.hpp"

class NetworkManager : public QObject
{
    Q_OBJECT

private:
    QTcpServer* server_ = nullptr;
    QTcpSocket* socket_ = nullptr;
    QHash<QTcpSocket*, quint16> nextBlockSizes_;
    QHash<QTcpSocket*, QString> clientNames_;
    QHash<QTcpSocket*, int> clientIds_;
    QVector<QTcpSocket*> clients_;
    QString hostNickname_;

public:
    explicit NetworkManager(QObject *parent = nullptr) : QObject(parent) {}
    void startServer(quint16 port, const QString& hostNickname);
    void connectToServer(QString ip_adress, quint16 port);
    void sendDraw(const DrawCommand& cmd);
    void sendMessage(const QString& text);
    void sendState(const GameState& state);
    void sendNickname(const QString& nickname);
    void broadcast(const QByteArray &bytes, QTcpSocket* sender);
    void sendBroadcastMessage(const QString& senderName, const QString& text);

signals:
    void nicknameReceivedFromNetwork(QTcpSocket* clientSocket, const QString& nickname);
    void messageReceivedFromNetwork(QTcpSocket* clientSocket, const QString& text);
    void drawCommandReceivedFromNetwork(const DrawCommand& cmd);
    void gameStateReceivedFromNetwork(const GameState& newState);
    void clientIdAssigned(int assignedId);

private slots:
    void newConnection();
    void onReadyRead();
};