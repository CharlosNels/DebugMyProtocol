#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QByteArray>
#include <QHostAddress>
#include <QIODevice>
#include <QObject>
#include <asio.hpp>
#include <memory>
#include <mutex>
#include <thread>

class MyUdpSocket;
class MyTcpServer;

class MyTcpSocket : public QIODevice {

    friend class MyUdpSocket;
    friend class MyTcpServer;
    typedef std::shared_ptr<asio::ip::tcp::socket> socket_ptr;

    Q_OBJECT
    Q_DISABLE_COPY(MyTcpSocket)
  private:
    explicit MyTcpSocket(socket_ptr sock_ptr, quint64 read_buffer_size = 1024 * 1024);

  public:
    explicit MyTcpSocket(quint64 read_buffer_size = 1024 * 1024, QObject *parent = nullptr);
    virtual ~MyTcpSocket();
    void disconnectFromHost();
    void connectToHost(const QString &hostName, quint16 port);
    void setReadBufferSize(quint64 buf_size);

    QString peerAddress() const;
    quint16 peerPort() const;

    QString localAddress() const;
    quint16 localPort() const;

    QString getErrorString()const;

  signals:
    void socketErrorOccurred();
    void disconnectedFromHost();
    void connectFinished(bool connected);

    // QIODevice interface
  public:
    bool isSequential() const override;
    void close() override;
    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;
    [[deprecated("this TCP socket use async io, do not support block read.")]] bool
    waitForReadyRead(int msecs) override;
    [[deprecated("this TCP socket use async io, do not support block write.")]] bool
    waitForBytesWritten(int msecs) override;

  protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 readLineData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

  private:
    void asyncConnectCallback(const asio::error_code &ec);
    void asyncReadCallback(const asio::error_code &ec, size_t size);
    void asyncWriteCallback(const asio::error_code &ec, size_t size);

  protected:
    socket_ptr m_asio_socket;
    QByteArray m_recv_buffer;
    std::mutex m_socket_mutex;
    std::unique_ptr<char[]> m_asio_read_buf;
    quint64 m_read_buffer_size;
    QString m_error_info;

  private:
    class MyIOContext {
      private:
        MyIOContext() {}
        static asio::io_context *io_context;
        static std::thread *io_thread;
        static std::mutex io_mutex;

      public:
        static asio::io_context *getIOContext();
    };
};

#endif // MYTCPSOCKET_H
