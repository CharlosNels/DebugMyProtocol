#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <asio.hpp>
#include <QHostAddress>
#include <memory>
#include <mutex>

class MyTcpSocket;

class MyTcpServer : public QObject
{
    typedef std::shared_ptr<asio::ip::tcp::socket> socket_ptr;
    typedef std::unique_ptr<asio::ip::tcp::acceptor> acceptor_ptr;

    Q_DISABLE_COPY(MyTcpServer)

    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();
    bool bind(const QHostAddress &address, quint16 port);
    void close();
    QString getErrorString()const;

signals:
    void newConnectionIncoming(MyTcpSocket *new_connection);
    void errorOccurred();

private:
    void asyncAcceptCallback(socket_ptr sock, const::std::error_code &ec);

private:
    acceptor_ptr m_acceptor;
    QString m_error_info;
    std::mutex m_sock_mutex;
    bool m_used;
};

#endif // MYTCPSERVER_H
