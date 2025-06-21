#include "../include/networkmanager.h"
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDataStream>

// Packet types
const char PACKET_TYPE_AUDIO = 'A';
const char PACKET_TYPE_PING = 'P';
const char PACKET_TYPE_PONG = 'O';

/**
 * @brief Constructor for NetworkManager.
 * @param parent The parent object.
 */
NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , server(new QTcpServer(this))
    , clientSocket(nullptr)
    , pingTimer(new QTimer(this))
    , sendQueueTimer(new QTimer(this))
    , currentLatency(0)
    , isServer(false)
    , connected(false)
{
    // Set up ping timer
    pingTimer->setInterval(1000); // Send ping every second
    connect(pingTimer, &QTimer::timeout, this, &NetworkManager::sendPing);
    
    // Set up send queue timer
    sendQueueTimer->setInterval(10); // Process send queue every 10ms
    connect(sendQueueTimer, &QTimer::timeout, this, &NetworkManager::processSendQueue);
    
    // Connect server signals
    connect(server, &QTcpServer::newConnection, this, &NetworkManager::handleNewConnection);
}

/**
 * @brief Destructor for NetworkManager.
 */
NetworkManager::~NetworkManager()
{
    disconnect();
}

/**
 * @brief Starts the network server (for receiver mode).
 * @param port The port to listen on.
 * @return True if started successfully, false otherwise.
 */
bool NetworkManager::startServer(int port)
{
    // Stop any existing connections
    disconnect();
    
    // Start listening
    if (!server->listen(QHostAddress::Any, port)) {
        emit error(tr("Failed to start server: %1").arg(server->errorString()));
        return false;
    }
    
    isServer = true;
    emit connectionStatusChanged(false, tr("Listening on port %1...").arg(port));
    
    return true;
}

/**
 * @brief Connects to a server (for sender mode).
 * @param address The server address.
 * @param port The server port.
 * @return True if connection was initiated, false otherwise.
 */
bool NetworkManager::connectToServer(const QString &address, int port)
{
    // Stop any existing connections
    disconnect();
    
    // Create new socket
    clientSocket = new QTcpSocket(this);
    
    // Connect socket signals
    connect(clientSocket, &QTcpSocket::connected, [this]() {
        connected = true;
        emit connectionStatusChanged(true, tr("Connected to server"));
        pingTimer->start();
        sendQueueTimer->start();
    });
    
    connect(clientSocket, &QTcpSocket::disconnected, this, &NetworkManager::handleDisconnect);
    connect(clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, &NetworkManager::handleSocketError);
    connect(clientSocket, &QTcpSocket::stateChanged, this, &NetworkManager::handleSocketStateChange);
    connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkManager::readData);
    
    // Connect to server
    clientSocket->connectToHost(address, port);
    
    isServer = false;
    emit connectionStatusChanged(false, tr("Connecting to %1:%2...").arg(address).arg(port));
    
    return true;
}

/**
 * @brief Disconnects from the server or stops the server.
 */
void NetworkManager::disconnect()
{
    // Stop timers
    pingTimer->stop();
    sendQueueTimer->stop();
    
    // Clear send queue
    QMutexLocker locker(&sendQueueMutex);
    sendQueue.clear();
    
    if (isServer) {
        // Stop server
        if (clientSocket) {
            clientSocket->disconnectFromHost();
            clientSocket->deleteLater();
            clientSocket = nullptr;
        }
        
        server->close();
    } else {
        // Disconnect from server
        if (clientSocket) {
            clientSocket->disconnectFromHost();
            clientSocket->deleteLater();
            clientSocket = nullptr;
        }
    }
    
    connected = false;
    emit connectionStatusChanged(false, tr("Disconnected"));
}

/**
 * @brief Sends audio data to the connected peer.
 * @param data The audio data to send.
 * @return True if data was queued for sending, false otherwise.
 */
bool NetworkManager::sendAudioData(const QByteArray &data)
{
    if (!connected || !clientSocket) {
        return false;
    }
    
    // Create audio packet
    QByteArray packet = createPacket(PACKET_TYPE_AUDIO, data);
    
    // Add to send queue
    QMutexLocker locker(&sendQueueMutex);
    sendQueue.enqueue(packet);
    
    return true;
}

/**
 * @brief Gets the current latency.
 * @return The current latency in milliseconds.
 */
int NetworkManager::getLatency() const
{
    return currentLatency;
}

/**
 * @brief Checks if connected to a peer.
 * @return True if connected, false otherwise.
 */
bool NetworkManager::isConnected() const
{
    return connected;
}

/**
 * @brief Handles a new incoming connection.
 */
void NetworkManager::handleNewConnection()
{
    // Accept only one connection
    if (clientSocket) {
        QTcpSocket *socket = server->nextPendingConnection();
        socket->disconnectFromHost();
        socket->deleteLater();
        return;
    }
    
    // Get the client socket
    clientSocket = server->nextPendingConnection();
    
    // Connect socket signals
    connect(clientSocket, &QTcpSocket::disconnected, this, &NetworkManager::handleDisconnect);
    connect(clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, &NetworkManager::handleSocketError);
    connect(clientSocket, &QTcpSocket::stateChanged, this, &NetworkManager::handleSocketStateChange);
    connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkManager::readData);
    
    // Update status
    connected = true;
    emit connectionStatusChanged(true, tr("Client connected from %1").arg(clientSocket->peerAddress().toString()));
    
    // Start timers
    pingTimer->start();
    sendQueueTimer->start();
}

/**
 * @brief Handles socket disconnection.
 */
void NetworkManager::handleDisconnect()
{
    // Stop timers
    pingTimer->stop();
    sendQueueTimer->stop();
    
    // Clean up
    if (clientSocket) {
        clientSocket->deleteLater();
        clientSocket = nullptr;
    }
    
    // Update status
    connected = false;
    emit connectionStatusChanged(false, isServer ? tr("Client disconnected") : tr("Disconnected from server"));
    
    // Clear send queue
    QMutexLocker locker(&sendQueueMutex);
    sendQueue.clear();
}

/**
 * @brief Handles socket errors.
 * @param socketError The socket error.
 */
void NetworkManager::handleSocketError(QAbstractSocket::SocketError socketError)
{
    if (!clientSocket) {
        return;
    }
    
    emit error(tr("Network error: %1").arg(clientSocket->errorString()));
    
    // Disconnect on error
    if (connected) {
        handleDisconnect();
    }
}

/**
 * @brief Handles socket state changes.
 * @param state The new socket state.
 */
void NetworkManager::handleSocketStateChange(QAbstractSocket::SocketState state)
{
    if (!clientSocket) {
        return;
    }
    
    switch (state) {
        case QAbstractSocket::ConnectingState:
            emit connectionStatusChanged(false, tr("Connecting..."));
            break;
        case QAbstractSocket::ConnectedState:
            emit connectionStatusChanged(true, tr("Connected"));
            break;
        case QAbstractSocket::ClosingState:
            emit connectionStatusChanged(false, tr("Disconnecting..."));
            break;
        case QAbstractSocket::UnconnectedState:
            if (connected) {
                handleDisconnect();
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Reads data from the socket.
 */
void NetworkManager::readData()
{
    if (!clientSocket) {
        return;
    }
    
    // Read all available data
    QByteArray data = clientSocket->readAll();
    
    // Process data
    char type;
    QByteArray payload;
    
    if (parsePacket(data, type, payload)) {
        switch (type) {
            case PACKET_TYPE_AUDIO:
                handleAudioPacket(payload);
                break;
            case PACKET_TYPE_PING:
                handlePingPacket(payload);
                break;
            case PACKET_TYPE_PONG:
                handlePongPacket(payload);
                break;
            default:
                qDebug() << "Unknown packet type:" << type;
                break;
        }
    }
}

/**
 * @brief Sends a ping to measure latency.
 */
void NetworkManager::sendPing()
{
    if (!connected || !clientSocket) {
        return;
    }
    
    // Create ping packet with current timestamp
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream << QDateTime::currentMSecsSinceEpoch();
    
    // Send ping packet
    QByteArray packet = createPacket(PACKET_TYPE_PING, payload);
    clientSocket->write(packet);
    
    // Start latency timer
    latencyTimer.restart();
}

/**
 * @brief Processes the send queue.
 */
void NetworkManager::processSendQueue()
{
    if (!connected || !clientSocket) {
        return;
    }
    
    // Process up to 10 packets at a time
    QMutexLocker locker(&sendQueueMutex);
    int count = qMin(10, sendQueue.size());
    
    for (int i = 0; i < count; i++) {
        if (sendQueue.isEmpty()) {
            break;
        }
        
        QByteArray packet = sendQueue.dequeue();
        clientSocket->write(packet);
    }
}

/**
 * @brief Handles a ping packet.
 * @param data The ping packet data.
 */
void NetworkManager::handlePingPacket(const QByteArray &data)
{
    // Send pong packet with the same data
    QByteArray packet = createPacket(PACKET_TYPE_PONG, data);
    clientSocket->write(packet);
}

/**
 * @brief Handles a pong packet.
 * @param data The pong packet data.
 */
void NetworkManager::handlePongPacket(const QByteArray &data)
{
    // Extract timestamp
    QDataStream stream(data);
    qint64 timestamp;
    stream >> timestamp;
    
    // Calculate latency
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    int latency = static_cast<int>(now - timestamp);
    
    // Update latency
    if (latency > 0) {
        currentLatency = latency;
        emit latencyChanged(currentLatency);
    }
}

/**
 * @brief Handles an audio packet.
 * @param data The audio packet data.
 */
void NetworkManager::handleAudioPacket(const QByteArray &data)
{
    // Emit audio data received signal
    emit audioDataReceived(data);
}

/**
 * @brief Creates a packet with the specified type and data.
 * @param type The packet type.
 * @param data The packet data.
 * @return The complete packet.
 */
QByteArray NetworkManager::createPacket(char type, const QByteArray &data) const
{
    QByteArray packet;
    
    // Add packet type
    packet.append(type);
    
    // Add data size (4 bytes)
    quint32 size = data.size();
    packet.append(reinterpret_cast<const char*>(&size), sizeof(size));
    
    // Add data
    packet.append(data);
    
    return packet;
}

/**
 * @brief Parses a packet.
 * @param packet The packet to parse.
 * @param type The packet type (output).
 * @param data The packet data (output).
 * @return True if parsing was successful, false otherwise.
 */
bool NetworkManager::parsePacket(const QByteArray &packet, char &type, QByteArray &data) const
{
    // Check minimum packet size
    if (packet.size() < 5) {
        return false;
    }
    
    // Extract packet type
    type = packet.at(0);
    
    // Extract data size
    quint32 size;
    memcpy(&size, packet.constData() + 1, sizeof(size));
    
    // Check packet size
    if (packet.size() < static_cast<int>(5 + size)) {
        return false;
    }
    
    // Extract data
    data = packet.mid(5, size);
    
    return true;
}
