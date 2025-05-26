#ifndef MYUDPSOCKET_H
#define MYUDPSOCKET_H

#include <QByteArray>
#include <QHostAddress>
#include <QIODevice>
#include <asio.hpp>
#include <mutex>

class MyUdpSocket : public QIODevice {
    Q_OBJECT

    typedef std::shared_ptr<asio::ip::udp::socket> socket_ptr;
    typedef std::shared_ptr<asio::ip::udp::resolver> acceptor_ptr;

  public:
    explicit MyUdpSocket(quint64 read_buffer_size = 1024 * 1024, QObject *parent = nullptr);
    void setReadBufferSize(quint64 buf_size);
    bool connectTo(QHostAddress host, quint16 port);
    bool bind(const QHostAddress &address, quint16 port);

  signals:
    void socketErrorOccurred();

    // QIODevice interface
  public:
    bool isSequential() const override;
    void close() override;
    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;

  protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 readLineData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

  private:
    void asyncSendCallback(const std::error_code &ec, size_t size);
    void asyncReceiveCallback(const std::error_code &ec, size_t size);

  protected:
    socket_ptr m_asio_socket;
    QByteArray m_recv_buffer;
    std::mutex m_socket_mutex;
    std::unique_ptr<char[]> m_asio_read_buf;
    quint64 m_read_buffer_size;
    asio::ip::udp::endpoint m_local_ep;
    asio::ip::udp::endpoint m_remote_ep;
    QString m_error_info;
};

#endif // MYUDPSOCKET_H
