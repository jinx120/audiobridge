#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QtCore/QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QByteArray>
#include <QtCore/QTimer>
#include <QtCore/QElapsedTimer>
#include <QtCore/QQueue>
#include <QtCore/QMutex>

/**
 * @brief The NetworkManager class handles network communication.
 * 
 * This class is responsible for managing network connections, sending and
 * receiving audio data, handling connection status and errors, and measuring latency.
 */
class NetworkManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for NetworkManager.
     * @param parent The parent object.
     */
    explicit NetworkManager(QObject *parent = nullptr);
    
    /**
     * @brief Destructor for NetworkManager.
     */
    ~NetworkManager();
    
    /**
     * @brief Starts the network server (for receiver mode).
     * @param port The port to listen on.
     * @return True if started successfully, false otherwise.
     */
    bool startServer(int port);
    
    /**
     * @brief Connects to a server (for sender mode).
     * @param address The server address.
     * @param port The server port.
     * @return True if connection was initiated, false otherwise.
     */
    bool connectToServer(const QString &address, int port);
    
    /**
     * @brief Disconnects from the server or stops the server.
     */
    void disconnect();
    
    /**
     * @brief Sends audio data to the connected peer.
     * @param data The audio data to send.
     * @return True if data was queued for sending, false otherwise.
     */
    bool sendAudioData(const QByteArray &data);
    
    /**
     * @brief Gets the current latency.
     * @return The current latency in milliseconds.
     */
    int getLatency() const;
    
    /**
     * @brief Checks if connected to a peer.
     * @return True if connected, false otherwise.
     */
    bool isConnected() const;

signals:
    /**
     * @brief Signal emitted when the connection status changes.
     * @param connected Whether the connection is established.
     * @param message Status message.
     */
    void connectionStatusChanged(bool connected, const QString &message);
    
    /**
     * @brief Signal emitted when audio data is received.
     * @param data The received audio data.
     */
    void audioDataReceived(const QByteArray &data);
    
    /**
     * @brief Signal emitted when the latency changes.
     * @param latencyMs The current latency in milliseconds.
     */
    void latencyChanged(int latencyMs);
    
    /**
     * @brief Signal emitted when an error occurs.
     * @param errorMessage The error message.
     */
    void error(const QString &errorMessage);

private slots:
    /**
     * @brief Handles a new incoming connection.
     */
    void handleNewConnection();
    
    /**
     * @brief Handles socket disconnection.
     */
    void handleDisconnect();
    
    /**
     * @brief Handles socket errors.
     * @param socketError The socket error.
     */
    void handleSocketError(QAbstractSocket::SocketError socketError);
    
    /**
     * @brief Handles socket state changes.
     * @param state The new socket state.
     */
    void handleSocketStateChange(QAbstractSocket::SocketState state);
    
    /**
     * @brief Reads data from the socket.
     */
    void readData();
    
    /**
     * @brief Sends a ping to measure latency.
     */
    void sendPing();
    
    /**
     * @brief Processes the send queue.
     */
    void processSendQueue();

private:
    /**
     * @brief Handles a ping packet.
     * @param data The ping packet data.
     */
    void handlePingPacket(const QByteArray &data);
    
    /**
     * @brief Handles a pong packet.
     * @param data The pong packet data.
     */
    void handlePongPacket(const QByteArray &data);
    
    /**
     * @brief Handles an audio packet.
     * @param data The audio packet data.
     */
    void handleAudioPacket(const QByteArray &data);
    
    /**
     * @brief Creates a packet with the specified type and data.
     * @param type The packet type.
     * @param data The packet data.
     * @return The complete packet.
     */
    QByteArray createPacket(char type, const QByteArray &data) const;
    
    /**
     * @brief Parses a packet.
     * @param packet The packet to parse.
     * @param type The packet type (output).
     * @param data The packet data (output).
     * @return True if parsing was successful, false otherwise.
     */
    bool parsePacket(const QByteArray &packet, char &type, QByteArray &data) const;

    QTcpServer *server;
    QTcpSocket *clientSocket;
    QTimer *pingTimer;
    QTimer *sendQueueTimer;
    QElapsedTimer latencyTimer;
    QQueue<QByteArray> sendQueue;
    QMutex sendQueueMutex;
    int currentLatency;
    bool isServer;
    bool connected;
};

#endif // NETWORKMANAGER_H
