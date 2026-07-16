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
    quint16 port_ = 8888;
    int max_clients_ = 2;

public:
    explicit NetworkManager(QObject *parent = nullptr) 
        : QObject(parent) {}
    void startServer(quint16 port, const QString& hostNickname);
    void connectToServer(QString ip_adress, quint16 port);
    void stopNetwork();
    void sendDraw(const DrawCommand& cmd);
    void sendMessage(const QString& text);
    void sendState(const GameState& state);
    void sendNickname(const QString& nickname);
    void broadcast(const QByteArray &bytes, QTcpSocket* sender);
    void sendBroadcastMessage(const QString& senderName, const QString& text);
    void sendSelectedWord(const QString& word);
    QString getIP();
    quint16 getPort();
    void setPort(quint16 port);
    void setMaxClients(int maxClients);

signals:
    void nicknameReceivedFromNetwork(QTcpSocket* clientSocket, const QString& nickname);
    void messageReceivedFromNetwork(QTcpSocket* clientSocket, const QString& text);
    void drawCommandReceivedFromNetwork(const DrawCommand& cmd);
    void gameStateReceivedFromNetwork(const GameState& newState);
    void clientIdAssigned(int assignedId);
    void connectionEstablished();
    void connectionFailed(const QString& errorString);
    void serverDisconnected();
    void connectionRejected(const QString& reason);

private slots:
    void newConnection();
    void onReadyRead();
};