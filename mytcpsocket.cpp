#include "mytcpsocket.h"
#include <QEventLoop>
#include <QTimer>
#include <QHostAddress>
#include <QMap>
#include <QDebug>
#include <boost/make_shared.hpp>

using namespace boost::asio;

io_context *MyTcpSocket::MyIOContext::io_context = nullptr;
std::thread *MyTcpSocket::MyIOContext::io_thread = nullptr;
std::mutex MyTcpSocket::MyIOContext::io_mutex;

MyTcpSocket::MyTcpSocket(socket_ptr sock_ptr, quint64 read_buffer_size) : QIODevice(nullptr)
{
    m_asio_socket  = sock_ptr;
    m_asio_acceptor = boost::make_shared<ip::tcp::acceptor>(*MyIOContext::getIOContext());
    m_asio_read_buf = nullptr;
    setReadBufferSize(read_buffer_size);
    QIODevice::open(QIODevice::ReadWrite);
    m_asio_socket->async_read_some(buffer(m_asio_read_buf,m_read_buffer_size),std::bind(&MyTcpSocket::asyncReadCallback,this,std::placeholders::_1,std::placeholders::_2));
}

MyTcpSocket::MyTcpSocket(quint64 read_buffer_size, QObject *parent)
    : QIODevice(parent)
{
    m_asio_socket = boost::make_shared<ip::tcp::socket>(*MyIOContext::getIOContext());
    m_asio_acceptor = boost::make_shared<ip::tcp::acceptor>(*MyIOContext::getIOContext());
    m_asio_read_buf = nullptr;
    setReadBufferSize(read_buffer_size);
}

MyTcpSocket::~MyTcpSocket()
{
    std::unique_lock<std::mutex> lock(m_socket_mutex);
    try
    {
        m_asio_socket->cancel();
    }
    catch(boost::wrapexcept<boost::system::system_error> &error)
    {
        delete []m_asio_read_buf;
        return;
    }
    if(m_asio_socket->is_open())
    {
        try
        {
            m_asio_socket->shutdown(ip::tcp::socket::shutdown_type::shutdown_both);
        }
        catch (boost::wrapexcept<boost::system::system_error> error)
        {
            delete[] m_asio_read_buf;
            return;
        }

        m_asio_socket->close();
    }
    if(m_asio_acceptor)
    {
        m_asio_acceptor->close();
    }
    delete[] m_asio_read_buf;
}


bool MyTcpSocket::isSequential() const
{
    return true;
}

void MyTcpSocket::close()
{

    std::unique_lock<std::mutex> lock(m_socket_mutex);
    if(m_asio_socket->is_open())
    {
        try
        {
            m_asio_socket->shutdown(ip::tcp::socket::shutdown_type::shutdown_both);
        }
        catch (boost::wrapexcept<boost::system::system_error> error)
        {
            emit socketErrorOccurred(error.code());
            return;
        }
        m_asio_socket->close();
    }
    if(m_asio_acceptor)
    {
        m_asio_acceptor->close();
    }

}

qint64 MyTcpSocket::bytesAvailable() const
{
    return m_recv_buffer.size();
}

qint64 MyTcpSocket::bytesToWrite() const
{
    return 0;
}

bool MyTcpSocket::waitForReadyRead(int msecs)
{
    return false;
}

bool MyTcpSocket::waitForBytesWritten(int msecs)
{
    return false;
}

qint64 MyTcpSocket::readData(char *data, qint64 maxlen)
{

    std::unique_lock<std::mutex> lock(m_socket_mutex);
    maxlen = maxlen == 0 ? UINT64_MAX : maxlen;
    int read_size = m_recv_buffer.size() <= maxlen ? m_recv_buffer.size() : maxlen;
    memcpy(data,m_recv_buffer.data(),read_size);
    m_recv_buffer.remove(0,read_size);

    return read_size;
}

qint64 MyTcpSocket::readLineData(char *data, qint64 maxlen)
{

    std::unique_lock<std::mutex> lock(m_socket_mutex);
    maxlen = maxlen == 0 ? UINT64_MAX : maxlen;
    int read_size = 0;
    if(m_recv_buffer.contains('\n'))
    {
        int line_size = m_recv_buffer.indexOf('\n') + 1;
        read_size = line_size <= maxlen ? line_size : maxlen;
    }
    else
    {
        read_size = m_recv_buffer.size() <= maxlen ? m_recv_buffer.size() : maxlen;
    }
    memcpy(data, m_recv_buffer.data(),read_size);
    m_recv_buffer.remove(0,read_size);

    return read_size;
}

qint64 MyTcpSocket::skipData(qint64 maxSize)
{

    std::unique_lock<std::mutex> lock(m_socket_mutex);
    int skipped_num = maxSize <= m_recv_buffer.size() ? maxSize : m_recv_buffer.size();
    m_recv_buffer.remove(0,skipped_num);

    return skipped_num;
}

qint64 MyTcpSocket::writeData(const char *data, qint64 len)
{

    std::unique_lock<std::mutex> lock(m_socket_mutex);
    m_asio_socket->async_write_some(const_buffer(data,len), std::bind(&MyTcpSocket::asyncWriteCallback,this,std::placeholders::_1,std::placeholders::_2));

    return len;
}

void MyTcpSocket::asyncConnectCallback(const std::error_code &ec)
{

    if(ec)
    {
        emit socketErrorOccurred(ec);
        emit connectFinished(false);
    }
    else
    {
        std::unique_lock<std::mutex> lock(m_socket_mutex);
        QIODevice::open(QIODevice::ReadWrite);
        emit connectFinished(true);
        m_asio_socket->async_read_some(buffer(m_asio_read_buf,m_read_buffer_size),std::bind(&MyTcpSocket::asyncReadCallback,this,std::placeholders::_1,std::placeholders::_2));
    }

}


bool MyTcpSocket::bind(const QHostAddress &address, quint16 port)
{

    std::unique_lock<std::mutex> lock(m_socket_mutex);
    try
    {
        ip::tcp::endpoint ep(ip::address::from_string(address.toString().toStdString()),port);
        m_asio_acceptor->open(ep.protocol());
        m_asio_acceptor->set_option(ip::tcp::acceptor::reuse_address());
        m_asio_acceptor->bind(ep);
    }
    catch(boost::wrapexcept<boost::system::system_error> error)
    {
        emit socketErrorOccurred(error.code());
        return false;
    }
    m_asio_acceptor->listen(0);
    socket_ptr sock_(new ip::tcp::socket(*MyIOContext::getIOContext()));
    m_asio_acceptor->async_accept(*sock_,std::bind(&MyTcpSocket::asyncAcceptCallback,this,sock_,std::placeholders::_1));

    return true;
}

void MyTcpSocket::setReadBufferSize(quint64 buf_size)
{

    std::unique_lock<std::mutex> lock(m_socket_mutex);
    m_read_buffer_size = buf_size;
    delete []m_asio_read_buf;
    m_asio_read_buf = new char[buf_size];

}

QString MyTcpSocket::peerAddress() const
{
    return QString::fromStdString(m_asio_socket->remote_endpoint().address().to_string());
}

quint16 MyTcpSocket::peerPort() const
{
    return m_asio_socket->remote_endpoint().port();
}

QString MyTcpSocket::localAddress() const
{
    return QString::fromStdString(m_asio_socket->local_endpoint().address().to_string());
}

quint16 MyTcpSocket::localPort() const
{
    return m_asio_socket->local_endpoint().port();
}

bool MyTcpSocket::connectToHost(const QString &hostName, quint16 port)
{

    std::unique_lock<std::mutex> lock(m_socket_mutex);
    try
    {
        ip::tcp::endpoint host(ip::address::from_string(hostName.toStdString()),port);
        m_asio_socket->async_connect(host,std::bind(&MyTcpSocket::asyncConnectCallback,this,std::placeholders::_1));
    }
    catch(boost::wrapexcept<boost::system::system_error> error)
    {
        emit socketErrorOccurred(error.code());
        return false;
    }
    return true;
}

void MyTcpSocket::disconnectFromHost()
{

    std::unique_lock<std::mutex> lock(m_socket_mutex);
    m_asio_socket->shutdown(ip::tcp::socket::shutdown_type::shutdown_both);
}


void MyTcpSocket::asyncReadCallback(const std::error_code &ec, size_t size)
{

    if(!ec)
    {
        std::unique_lock<std::mutex> lock(m_socket_mutex);
        m_recv_buffer.append(m_asio_read_buf,size);
        emit readyRead();
        m_asio_socket->async_read_some(buffer(m_asio_read_buf,m_read_buffer_size),std::bind(&MyTcpSocket::asyncReadCallback,this,std::placeholders::_1,std::placeholders::_2));
    }
    else
    {
        emit socketErrorOccurred(ec);
        emit disconnectedFromHost();
    }

}

void MyTcpSocket::asyncWriteCallback(const std::error_code &ec, size_t size)
{
    if(ec)
    {
        emit socketErrorOccurred(ec);
    }
}

void MyTcpSocket::asyncAcceptCallback(socket_ptr sock,const std::error_code &ec)
{
    if(!ec)
    {
        std::unique_lock<std::mutex> lock(m_socket_mutex);
        MyTcpSocket *new_con = new MyTcpSocket(sock);
        emit newConnectionIncoming(new_con);
        socket_ptr sock_(new ip::tcp::socket(*MyIOContext::getIOContext()));
        m_asio_acceptor->async_accept(*sock_,std::bind(&MyTcpSocket::asyncAcceptCallback,this,sock_,std::placeholders::_1));
    }
    else
    {
        emit socketErrorOccurred(ec);
    }
}

boost::asio::io_context *MyTcpSocket::MyIOContext::getIOContext()
{
    if(io_context == nullptr)
    {
        io_mutex.lock();
        io_context = new boost::asio::io_context();
        io_thread = new std::thread([](){
            io_context::work worker(*io_context);
            io_context->run();
        });
        io_mutex.unlock();
    }
    return io_context;
}
