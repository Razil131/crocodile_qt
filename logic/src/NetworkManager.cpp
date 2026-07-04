#include "NetworkManager.hpp"
#include "NetworkTypes.hpp"
#include "GameState.hpp"
#include <QtNetwork/QHostAddress>
#include <QtCore/QDebug>


void NetworkManager::startServer(quint16 port){
    server_ = new QTcpServer(this);

    if (!server_->listen(QHostAddress::Any, port)) {
        qDebug() << "Ошибка запуска сервера:" << server_->errorString();
        return;
    }

    connect(server_, &QTcpServer::newConnection, this, &NetworkManager::newConnection);
}

void NetworkManager::newConnection(){
    while (server_->hasPendingConnections()) {
        QTcpSocket* clientSocket = server_->nextPendingConnection();
        clients_.append(clientSocket);
        connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, [this, clientSocket]() {
            clients_.removeAll(clientSocket);
            clientSocket->deleteLater();
            nextBlockSizes_.remove(clientSocket);
        });
    }
}

void NetworkManager::onReadyRead() {
    auto* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_6_0);
    while (true) {
        quint16 &nextBlockSize = nextBlockSizes_[clientSocket];
        if (nextBlockSize == 0) {
            if (clientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> nextBlockSize;
        }
        if (clientSocket->bytesAvailable() < nextBlockSize) {
            break;
        }
        quint8 rawType;
        in >> rawType;
        NetworkTypes packetType = static_cast<NetworkTypes>(rawType);
        switch (packetType) {
            case NetworkTypes::Draw_: {
                DrawCommand cmd;
                in >> cmd; 
                emit drawCommandReceived(cmd);
                if(server_){
                QByteArray buffer;
                QDataStream out(&buffer, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Qt_6_0);
                out << quint16(0) << static_cast<quint8>(NetworkTypes::Draw_) << cmd;
                out.device()->seek(0);
                out << quint16(buffer.size() - sizeof(quint16));
                broadcast(buffer, clientSocket);
                }
            break;
            }
            case NetworkTypes::Message_: {
                QString text;
                in >> text;
                emit messageReceived(static_cast<int>(clientSocket->socketDescriptor()), "Player", text);
                if(server_){
                QByteArray buffer;
                QDataStream out(&buffer, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Qt_6_0);
                out << quint16(0) << static_cast<quint8>(NetworkTypes::Message_) << text;
                out.device()->seek(0);
                out << quint16(buffer.size() - sizeof(quint16));
                broadcast(buffer, clientSocket);                
                }
            break;
            }
            case NetworkTypes::State_: {
                GameState state;
                in >> state;
                emit gameStateReceived(state);
                if(server_){
                QByteArray buffer;
                QDataStream out(&buffer, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Qt_6_0);
                out << quint16(0) << static_cast<quint8>(NetworkTypes::State_) << state;
                out.device()->seek(0);
                out << quint16(buffer.size() - sizeof(quint16));
                broadcast(buffer, clientSocket);
                }
            break;
            }
            default: {
                clientSocket->readAll();
                nextBlockSize = 0;
                return;
            }
        }
        nextBlockSize = 0;
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
    
    connect(socket_, &QTcpSocket::errorOccurred, this, [](QAbstractSocket::SocketError error) {
        qDebug() << "Ошибка сокета клиента:" << error;
    });

    socket_->connectToHost(ip_adress, port);
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

void NetworkManager::sendMessage(const QString& text){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << quint16(0) << static_cast<quint8>(NetworkTypes::Message_);
    out << text;
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