#include "myudpsocket.h"
#include "mytcpsocket.h"

using namespace boost::asio;

MyUdpSocket::MyUdpSocket(QHostAddress host, quint16 port, quint64 read_buffer_size, QObject *parent)
    : QIODevice{parent}, m_read_buffer_size(read_buffer_size)
{
    m_asio_socket = boost::make_shared<ip::udp::socket>(*MyTcpSocket::my_tcp_context::getTcpContext());
    m_asio_read_buf = nullptr;
    setReadBufferSize(read_buffer_size);
    m_remote_ep = ip::udp::endpoint(ip::address::from_string(host.toString().toStdString()),port);
    m_asio_socket->async_receive_from(buffer(m_asio_read_buf,read_buffer_size), m_remote_ep, std::bind(&MyUdpSocket::asyncReceiveCallback,this,std::placeholders::_1,std::placeholders::_2));
}

void MyUdpSocket::setReadBufferSize(quint64 buf_size)
{
    m_read_buffer_size = buf_size;
    delete []m_asio_read_buf;
    m_asio_read_buf = new char[buf_size];
}


bool MyUdpSocket::isSequential() const
{
    return true;
}

void MyUdpSocket::close()
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
}

qint64 MyUdpSocket::bytesAvailable() const
{
    return m_recv_buffer.size();
}

qint64 MyUdpSocket::bytesToWrite() const
{
    return 0;
}

qint64 MyUdpSocket::readData(char *data, qint64 maxlen)
{
    std::unique_lock<std::mutex> lock(m_socket_mutex);
    maxlen = maxlen == 0 ? UINT64_MAX : maxlen;
    int read_size = m_recv_buffer.size() <= maxlen ? m_recv_buffer.size() : maxlen;
    memcpy(data,m_recv_buffer.data(),read_size);
    m_recv_buffer.remove(0,read_size);

    return read_size;
}

qint64 MyUdpSocket::readLineData(char *data, qint64 maxlen)
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

qint64 MyUdpSocket::writeData(const char *data, qint64 len)
{
    std::unique_lock<std::mutex> lock(m_socket_mutex);
    m_asio_socket->async_send_to(const_buffer(data,len), m_remote_ep, std::bind(&MyUdpSocket::asyncSendCallback,this,std::placeholders::_1,std::placeholders::_2));

    return len;
}

qint64 MyUdpSocket::skipData(qint64 maxSize)
{
    std::unique_lock<std::mutex> lock(m_socket_mutex);
    int skipped_num = maxSize <= m_recv_buffer.size() ? maxSize : m_recv_buffer.size();
    m_recv_buffer.remove(0,skipped_num);

    return skipped_num;
}

void MyUdpSocket::asyncSendCallback(const std::error_code &ec, int size)
{
    if(ec)
    {
        emit socketErrorOccurred(ec);
    }
}

void MyUdpSocket::asyncReceiveCallback(const std::error_code &ec, int size)
{
    if(ec)
    {
        emit socketErrorOccurred(ec);
    }
    else
    {
        std::unique_lock<std::mutex> lock(m_socket_mutex);
        m_recv_buffer.append(m_asio_read_buf, size);
        emit readyRead();
        m_asio_socket->async_receive_from(buffer(m_asio_read_buf,m_read_buffer_size), m_remote_ep, std::bind(&MyUdpSocket::asyncReceiveCallback,this,std::placeholders::_1,std::placeholders::_2));
    }
}
