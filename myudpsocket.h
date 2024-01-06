#ifndef MYUDPSOCKET_H
#define MYUDPSOCKET_H

#include <QIODevice>
#include <boost/asio.hpp>
#include <mutex>
#include <QByteArray>
#include <QHostAddress>

class MyUdpSocket : public QIODevice
{
    Q_OBJECT

    typedef boost::shared_ptr<boost::asio::ip::udp::socket> socket_ptr;
    typedef boost::shared_ptr<boost::asio::ip::udp::resolver> acceptor_ptr;

public:
    explicit MyUdpSocket(quint64 read_buffer_size = 1024 * 1024, QObject *parent = nullptr);
    void setReadBufferSize(quint64 buf_size);
    bool connectTo(QHostAddress host, quint16 port);

signals:
    void socketErrorOccurred(const std::error_code &ec);

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
    qint64 skipData(qint64 maxSize) override;

private:
    void asyncSendCallback(const std::error_code &ec, int size);
    void asyncReceiveCallback(const std::error_code &ec, int size);

protected:
    socket_ptr m_asio_socket;
    QByteArray m_recv_buffer;
    std::mutex m_socket_mutex;
    char *m_asio_read_buf;
    quint64 m_read_buffer_size;
    boost::asio::ip::udp::endpoint m_remote_ep;
};

#endif // MYUDPSOCKET_H
