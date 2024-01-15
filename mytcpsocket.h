#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QIODevice>
#include <QObject>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <QVariant>
#include <QByteArray>
#include <QHostAddress>

class MyUdpSocket;

class MyTcpSocket : public QIODevice
{

    friend class MyUdpSocket;
    typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ptr;

    Q_OBJECT
    Q_DISABLE_COPY(MyTcpSocket)
private:
    explicit MyTcpSocket(socket_ptr sock_ptr, quint64 read_buffer_size = 1024*1024);
public:
    explicit MyTcpSocket(quint64 read_buffer_size = 1024*1024, QObject *parent = nullptr);
    virtual ~MyTcpSocket();
    void disconnectFromHost();
    bool connectToHost(const QString &hostName, quint16 port);
    bool bind(const QHostAddress &address, quint16 port);
    void setReadBufferSize(quint64 buf_size);

    QString peerAddress() const;
    quint16 peerPort() const;

    QString localAddress() const;
    quint16 localPort() const;

signals:
    void socketErrorOccurred(const std::error_code &ec);
    void newConnectionIncoming(MyTcpSocket *new_connection);
    void disconnectedFromHost();
    void connectFinished(bool connected);

    // QIODevice interface
public:
    bool isSequential() const override;
    void close() override;
    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;
    [[deprecated("this TCP socket use async io, do not support block read.")]] bool waitForReadyRead(int msecs) override;
    [[deprecated("this TCP socket use async io, do not support block write.")]] bool waitForBytesWritten(int msecs) override;

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 readLineData(char *data, qint64 maxlen) override;
    qint64 skipData(qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 len) override;


private:
    void asyncConnectCallback(const std::error_code &ec);
    void asyncReadCallback(const std::error_code &ec, size_t size);
    void asyncWriteCallback(const std::error_code &ec, size_t size);
    void asyncAcceptCallback(socket_ptr sock,const std::error_code &ec);

protected:
    socket_ptr m_asio_socket;
    acceptor_ptr m_asio_acceptor;
    QByteArray m_recv_buffer;
    std::mutex m_socket_mutex;
    char *m_asio_read_buf;
    quint64 m_read_buffer_size;
private:
    class MyIOContext
    {
    private:
        MyIOContext(){}
        static boost::asio::io_context *io_context;
        static std::thread *io_thread;
        static std::mutex io_mutex;
    public:
        static boost::asio::io_context *getIOContext();
    };
};

#endif // MYTCPSOCKET_H
