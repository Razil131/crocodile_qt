#include "NetworkManager.hpp"
#include "NetworkTypes.hpp"
#include "GameState.hpp"
#include "GameController.hpp"
#include <QtNetwork/QHostAddress>
#include <QtCore/QDebug>
#include <QtCore/QDataStream>
#include <QNetworkInterface>

void NetworkManager::startServer(quint16 port, const QString& hostNickname) {
    hostNickname_ = hostNickname;
    server_ = new QTcpServer(this);

    if (!server_->listen(QHostAddress::Any, port)) {
        return;
    }

    connect(server_, &QTcpServer::newConnection, this, &NetworkManager::newConnection);
}

void NetworkManager::connectToServer(QString ip_adress, quint16 port) {
    if (socket_) {
        socket_->disconnectFromHost();
        socket_->deleteLater();
        socket_ = nullptr;
    }
    socket_ = new QTcpSocket(this);
    connect(socket_, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    QTcpSocket* currentSocket = socket_;
    connect(socket_, &QTcpSocket::disconnected, this, [this, currentSocket]() {
        nextBlockSizes_.remove(currentSocket);
    });

    connect(socket_, &QTcpSocket::connected, this, &NetworkManager::connectionEstablished);
    connect(socket_, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        if (error == QAbstractSocket::RemoteHostClosedError) {
            emit serverDisconnected();
        } else {
            emit connectionFailed(socket_->errorString());
        }
    });

    socket_->connectToHost(ip_adress, port);
}

void NetworkManager::stopNetwork() {
    if (server_) {
        for (QTcpSocket* client : clients_) {
            if (client && client->state() == QAbstractSocket::ConnectedState) {
                client->disconnectFromHost();
            }
        }
        server_->close();
    }
    if (socket_) {
        socket_->disconnectFromHost();
    }
}

void NetworkManager::sendDraw(const DrawCommand& cmd){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << quint16(0) << static_cast<quint8>(NetworkTypes::Draw_);
    out << cmd;
    out.device()->seek(0);
    quint16 actualSize = static_cast<quint16>(block.size() - sizeof(quint16));
    out << actualSize;
    if (server_) {
        broadcast(block, nullptr);
    } else if (socket_ && socket_->state() == QAbstractSocket::ConnectedState) {
        socket_->write(block);
        socket_->flush();
    }
}


void NetworkManager::sendMessage(const QString& text) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << quint16(0) << static_cast<quint8>(NetworkTypes::Message_);
    out << text;
    out.device()->seek(0);
    quint16 actualSize = static_cast<quint16>(block.size() - sizeof(quint16));
    out << actualSize;

    if (!server_) {
        if (socket_ && socket_->state() == QAbstractSocket::ConnectedState) {
            socket_->write(block);
            socket_->flush();
        }
    } else {
        broadcast(block, nullptr);
    }
}

void NetworkManager::sendState(const GameState& state){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << quint16(0) << static_cast<quint8>(NetworkTypes::State_);
    out << state;
    out.device()->seek(0);
    quint16 actualSize = static_cast<quint16>(block.size() - sizeof(quint16));
    out << actualSize;
    if(!server_){
        if(socket_ && socket_->state() == QAbstractSocket::ConnectedState) {
            socket_->write(block);
            socket_->flush();
        }
    }
    else{
        broadcast(block, nullptr);
    }
}

void NetworkManager::sendNickname(const QString& nickname){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << quint16(0) << static_cast<quint8>(NetworkTypes::Nickname_) << nickname;
    out.device()->seek(0);
    quint16 actualSize = static_cast<quint16>(block.size() - sizeof(quint16));
    out << actualSize;
    if (!server_) {
        if (socket_) {
            if (socket_->state() == QAbstractSocket::ConnectedState) {
                socket_->write(block);
                socket_->flush();
            } else {
                connect(socket_, &QTcpSocket::connected, this, [this, block]() {
                    socket_->write(block);
                    socket_->flush();
                }, Qt::SingleShotConnection);
            }
        }
    }
}

void NetworkManager::broadcast(const QByteArray &bytes, QTcpSocket* sender){
    if(!server_){
        return;
    }
    for(auto client : clients_){
        if(client == sender){
            continue;
        }
        if (client->state() == QAbstractSocket::ConnectedState) {
            client->write(bytes);
            client->flush();
        }
    }
}

void NetworkManager::sendBroadcastMessage(const QString& senderName, const QString& text) {
    if (!server_) return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << quint16(0) << static_cast<quint8>(NetworkTypes::Message_) << senderName << text;
    out.device()->seek(0);
    quint16 actualSize = static_cast<quint16>(block.size() - sizeof(quint16));
    out << actualSize;

    broadcast(block, nullptr);
}

void NetworkManager::sendSelectedWord(const QString& word) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << quint16(0) << static_cast<quint8>(NetworkTypes::WordSelected_) << word;
    out.device()->seek(0);
    quint16 actualSize = static_cast<quint16>(block.size() - sizeof(quint16));
    out << actualSize;

    if (!server_) {
        if (socket_ && socket_->state() == QAbstractSocket::ConnectedState) {
            socket_->write(block);
            socket_->flush();
        }
    }
}

QString NetworkManager::getIP(){
    const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (const QHostAddress &address : ipAddressesList) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
            return address.toString();
        }
    }
    return QHostAddress(QHostAddress::LocalHost).toString(); 
}

quint16 NetworkManager::getPort(){
    return port_;
}

void NetworkManager::setPort(quint16 port){
    port_ = port;
}

void NetworkManager::setMaxClients(int maxClients){
    max_clients_ = maxClients;
}

void NetworkManager::newConnection(){
    while (server_->hasPendingConnections()) {
        QTcpSocket* clientSocket = server_->nextPendingConnection();
        if (clients_.size() >= max_clients_) {
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_6_0);
            out << quint16(0) << static_cast<quint8>(NetworkTypes::ConnectionRejected_) << QString(tr("Сервер переполнен"));
            out.device()->seek(0);
            out << static_cast<quint16>(block.size() - sizeof(quint16));
            clientSocket->write(block);
            clientSocket->flush();
            connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
            connect(clientSocket, &QTcpSocket::bytesWritten, clientSocket, [clientSocket]() {
                clientSocket->disconnectFromHost();
            }, Qt::SingleShotConnection);
            continue; 
        }
        clients_.append(clientSocket);
        connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, [this, clientSocket]() {

            int disconnectedId = clientIds_.value(clientSocket, -1);

            clients_.removeAll(clientSocket);
            clientSocket->deleteLater();
            nextBlockSizes_.remove(clientSocket);
            clientNames_.remove(clientSocket);
            clientIds_.remove(clientSocket);

            auto* gameController = qobject_cast<GameController*>(parent());
            if (gameController && disconnectedId != -1) {
                gameController->processPlayerDisconnect(disconnectedId);
            }
        });
    }
}

void NetworkManager::onReadyRead() {
    auto* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_6_0);
    quint16 &nextBlockSize = nextBlockSizes_[clientSocket];
    while (true) {
        if (nextBlockSize == 0) {
            if (clientSocket->bytesAvailable() < sizeof(quint16)) break;
            in >> nextBlockSize;
        }
        if (clientSocket->bytesAvailable() < nextBlockSize) break;
        quint8 rawType;
        in >> rawType;
        NetworkTypes type = static_cast<NetworkTypes>(rawType);
        auto* gameController = qobject_cast<GameController*>(parent());
        switch (type) {
            case NetworkTypes::Nickname_: {
                QString nickname;
                in >> nickname;
                clientNames_[clientSocket] = nickname;
                
                if (server_ && gameController) {
                    Player& newPlayer = gameController->players()->createAndAddPlayer(nickname);
                    int assignedId = newPlayer.id();
                    clientIds_[clientSocket] = assignedId;
                    QByteArray idBlock;
                    QDataStream idOut(&idBlock, QIODevice::WriteOnly);
                    idOut.setVersion(QDataStream::Qt_6_0);
                    idOut << quint16(0) << static_cast<quint8>(NetworkTypes::IdAssignment_) << qint32(assignedId);
                    idOut.device()->seek(0);
                    idOut << static_cast<quint16>(idBlock.size() - sizeof(quint16));
                    
                    clientSocket->write(idBlock);
                    clientSocket->flush();

                    gameController->sendCurrentGameState();
                }
                break;
            }
            case NetworkTypes::IdAssignment_: {
                qint32 assignedId;
                in >> assignedId;
                if (!server_ && gameController) {
                    emit clientIdAssigned(assignedId);
                }
                break;
            }
            case NetworkTypes::Message_: {
                if (server_) {
                    QString text;
                    in >> text;
                    int senderId = clientIds_.value(clientSocket, -1);

                    if (gameController) {
                        gameController->processNetworkChatMessage(senderId, text);
                    }
                } else {
                    QString senderName;
                    QString text;
                    in >> senderName >> text;
                    if (gameController) {
                        emit gameController->messageReceived(-1, senderName, text);
                    }
                }
                break;
            }
            case NetworkTypes::Draw_: {
                DrawCommand cmd;
                in >> cmd;
                if (gameController) {
                    emit gameController->drawCommandReceived(cmd);
                }
                if (server_) {
                    QByteArray drawBlock;
                    QDataStream drawOut(&drawBlock, QIODevice::WriteOnly);
                    drawOut.setVersion(QDataStream::Qt_6_0);
                    drawOut << quint16(0) << static_cast<quint8>(NetworkTypes::Draw_) << cmd;
                    drawOut.device()->seek(0);
                    drawOut << static_cast<quint16>(drawBlock.size() - sizeof(quint16));
                    broadcast(drawBlock, clientSocket); 
                }
                break;
            }
            case NetworkTypes::State_: {
                GameState newState;
                in >> newState;
                if (!server_) {
                    emit gameStateReceivedFromNetwork(newState);
                }
                break;
            }
            case NetworkTypes::WordSelected_: {
                QString word;
                in >> word;
                if (server_ && gameController) {
                    int senderId = clientIds_.value(clientSocket, -1);
                    gameController->processNetworkWordSelection(senderId, word);
                }
                break;
            }
            case NetworkTypes::ConnectionRejected_: {
            QString reason;
            in >> reason;
            if (!server_) {
                emit connectionRejected(reason);
                if (socket_) {
                    socket_->disconnectFromHost();
                }
            }
            break;
            }
        }
        nextBlockSize = 0;
    }
}