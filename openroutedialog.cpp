#include "openroutedialog.h"
#include "ui_openroutedialog.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QSerialPortInfo>
#include <QMainWindow>
#include <QDebug>
#include "floatbox.h"
#include "mytcpsocket.h"
#include "myudpsocket.h"

const QMap<QString, QSerialPort::BaudRate> OpenRouteDialog::baud_map = {
    {"1200", QSerialPort::Baud1200},
    {"2400", QSerialPort::Baud2400},
    {"4800", QSerialPort::Baud4800},
    {"9600", QSerialPort::Baud9600},
    {"19200", QSerialPort::Baud19200},
    {"38400", QSerialPort::Baud38400},
    {"57600", QSerialPort::Baud57600},
    {"115200", QSerialPort::Baud115200}
};

const QMap<QString, QSerialPort::DataBits> OpenRouteDialog::data_bits_map = {
    {tr("5 bit"), QSerialPort::Data5},
    {tr("6 bit"), QSerialPort::Data6},
    {tr("7 bit"), QSerialPort::Data7},
    {tr("8 bit"), QSerialPort::Data8}
};

const QMap<QString, QSerialPort::Parity> OpenRouteDialog::parity_map = {
    {tr("None"), QSerialPort::NoParity},
    {tr("Even"), QSerialPort::EvenParity},
    {tr("Odd"), QSerialPort::OddParity},
    {tr("Space"), QSerialPort::SpaceParity},
    {tr("Mark"), QSerialPort::MarkParity}
};

const QMap<QString, QSerialPort::StopBits> OpenRouteDialog::stop_bits_map = {
    {tr("1 bit"), QSerialPort::OneStop},
    {tr("1.5 bits"), QSerialPort::OneAndHalfStop},
    {tr("2 bits"), QSerialPort::TwoStop}
};

const QMap<QString, QSerialPort::FlowControl> OpenRouteDialog::flow_control_map = {
    {tr("None"), QSerialPort::NoFlowControl},
    {"RTS/CTS", QSerialPort::HardwareControl},
    {"XON/XOFF", QSerialPort::SoftwareControl}
};

const QMap<QString, QList<QString> > OpenRouteDialog::protocol_map = {
    {tr("Serial Port"), {"Modbus-RTU", "Modbus-ASCII"}},
    {tr("TCP-Server"), {"Modbus-TCP", "Modbus-RTU", "Modbus-ASCII"}},
    {tr("TCP-Client"), {"Modbus-TCP", "Modbus-RTU", "Modbus-ASCII"}},
    {"UDP", {"Modbus-UDP","Modbus-RTU","Modbus-ASCII"}}
};

const QMap<QString, int> OpenRouteDialog::protocol_enum_map = {
    {"Modbus-RTU", MODBUS_RTU},
    {"Modbus-ASCII", MODBUS_ASCII},
    {"Modbus-TCP", MODBUS_TCP},
    {"Modbus-UDP", MODBUS_UDP}
};

OpenRouteDialog::OpenRouteDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OpenRouteDialog), m_connecting_client{nullptr}, m_parent_window(parent)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    QList<QSerialPortInfo> serial_ports = QSerialPortInfo::availablePorts();
    for(const auto &x : serial_ports)
    {
        ui->box_port_name->addItem(x.portName());
    }
    ui->box_baud_rate->addItems(baud_map.keys());
    ui->box_parity->addItems(parity_map.keys());
    ui->box_data_bits->addItems(data_bits_map.keys());
    ui->box_stop_bits->addItems(stop_bits_map.keys());
    ui->box_flow_control->addItems(flow_control_map.keys());
    ui->box_protocol_serial_port->addItems(protocol_map[tr("Serial Port")]);

    QList<QHostAddress> hosts = QNetworkInterface::allAddresses();
    bool cvt_ok{ false };
    for(const auto &x : hosts)
    {
        x.toIPv4Address(&cvt_ok);
        if(cvt_ok)
        {
            ui->box_tcp_server_addr->addItem(x.toString());
        }
    }

    ui->box_tcp_server_protocol->addItems(protocol_map[tr("TCP-Server")]);


    ui->box_tcp_client_protocol->addItems(protocol_map[tr("TCP-Client")]);

    ui->box_udp_protocol->addItems(protocol_map[("UDP")]);
}

OpenRouteDialog::~OpenRouteDialog()
{
    delete ui;
}

void OpenRouteDialog::tcpSocketDisconnectedFromHost()
{
    MyTcpSocket *socket = dynamic_cast<MyTcpSocket*>(sender());
    if(socket)
    {
        socket->deleteLater();
    }
}

void OpenRouteDialog::clientConnectFinished(bool connected)
{
    if(connected)
    {
        connect(m_connecting_client, &MyTcpSocket::disconnectedFromHost, this, &OpenRouteDialog::tcpSocketDisconnectedFromHost);
        emit createdRoute(m_connecting_client, QString("%1:%2 - %3").arg(m_connecting_client->peerAddress()).arg(m_connecting_client->peerPort()).arg(ui->box_tcp_client_protocol->currentText()),protocol_enum_map[ui->box_tcp_client_protocol->currentText()], ui->box_identity_tcp_client->currentText() == tr("Master"));
        hide();
    }
    else
    {
        m_connecting_client->deleteLater();
    }
}

void OpenRouteDialog::socketErrorOccurred(const std::error_code &ec)
{
    FloatBox::message(QString("%1 %2").arg(ec.value()).arg(QString::fromStdString(ec.message())), 3000, m_parent_window->geometry());
}

void OpenRouteDialog::newTcpConnectionIncoming(MyTcpSocket *sock_ptr)
{
    MyTcpSocket *server = dynamic_cast<MyTcpSocket*>(sender());
    if(server)
    {
        connect(sock_ptr, &MyTcpSocket::disconnectedFromHost, this, &OpenRouteDialog::tcpSocketDisconnectedFromHost);
        emit createdRoute(sock_ptr,QString("%1:%2 - %3").arg(sock_ptr->peerAddress()).arg(sock_ptr->peerPort()).arg(m_server_protocol_map[server]),protocol_enum_map[m_server_protocol_map[server]], m_server_identity_map[server]);
    }
}

void OpenRouteDialog::on_button_open_serial_port_clicked()
{
    QSerialPort *serial_port = new QSerialPort(ui->box_port_name->currentText());
    serial_port->setBaudRate(baud_map[ui->box_baud_rate->currentText()]);
    serial_port->setDataBits(data_bits_map[ui->box_data_bits->currentText()]);
    serial_port->setStopBits(stop_bits_map[ui->box_stop_bits->currentText()]);
    serial_port->setParity(parity_map[ui->box_parity->currentText()]);
    serial_port->setFlowControl(flow_control_map[ui->box_flow_control->currentText()]);
    if(serial_port->open(QIODevice::ReadWrite))
    {
        emit createdRoute(serial_port, serial_port->portName(), protocol_enum_map[ui->box_protocol_serial_port->currentText()], ui->box_identity_serial_port->currentText() == tr("Master"));
        hide();
    }
    else
    {
        FloatBox::message(serial_port->errorString(), 3000, m_parent_window->geometry());
    }
}


void OpenRouteDialog::on_button_listen_clicked()
{
    MyTcpSocket *server = new MyTcpSocket();
    connect(server, &MyTcpSocket::socketErrorOccurred, this, &OpenRouteDialog::socketErrorOccurred);
    if(server->bind(QHostAddress(ui->box_tcp_server_addr->currentText()),ui->box_tcp_server_port->value()))
    {
        connect(server, &MyTcpSocket::newConnectionIncoming, this, &OpenRouteDialog::newTcpConnectionIncoming);
        m_server_protocol_map[server] = ui->box_tcp_server_protocol->currentText();
        m_server_identity_map[server] = ui->box_identity_tcp_server->currentText() == tr("Master");
        m_listening_servers.append(server);
        FloatBox::message(QString("%1 : %2:%3").arg(tr("Listening"),ui->box_tcp_server_addr->currentText()).arg(ui->box_tcp_server_port->value()),3000,m_parent_window->geometry());
        hide();
    }
    else
    {
        server->deleteLater();
    }
}


void OpenRouteDialog::on_button_connect_clicked()
{
    MyTcpSocket *client = new MyTcpSocket();
    connect(client, &MyTcpSocket::socketErrorOccurred, this, &OpenRouteDialog::socketErrorOccurred);
    m_connecting_client = client;
    if(client->connectToHost(ui->edit_tcp_remote_server_addr->text(), ui->box_tcp_remote_server_port->value()))
    {
        connect(client, &MyTcpSocket::connectFinished, this, &OpenRouteDialog::clientConnectFinished);
    }
    else
    {
        client->deleteLater();
    }
}


void OpenRouteDialog::on_box_protocol_serial_port_currentTextChanged(const QString &arg1)
{
    if(arg1.contains("Modbus"))
    {
        ui->box_identity_serial_port->show();
        ui->label_identity_serial_port->show();
    }
    else
    {
        ui->box_identity_serial_port->hide();
        ui->label_identity_serial_port->hide();
    }
}


void OpenRouteDialog::on_box_tcp_server_protocol_currentTextChanged(const QString &arg1)
{
    if(arg1.contains("Modbus"))
    {
        ui->box_identity_tcp_server->show();
        ui->label_identity_tcp_server->show();
    }
    else
    {
        ui->box_identity_tcp_server->hide();
        ui->label_identity_tcp_server->hide();
    }
}


void OpenRouteDialog::on_box_tcp_client_protocol_currentTextChanged(const QString &arg1)
{
    if(arg1.contains("Modbus"))
    {
        ui->box_identity_tcp_client->show();
        ui->label_identity_tcp_client->show();
    }
    else
    {
        ui->box_identity_tcp_client->hide();
        ui->label_identity_tcp_client->hide();
    }
}


void OpenRouteDialog::on_box_udp_protocol_currentTextChanged(const QString &arg1)
{
    if(arg1.contains("Modbus"))
    {
        ui->box_identity_udp->show();
        ui->label_identity_udp->show();
    }
    else
    {
        ui->box_identity_udp->hide();
        ui->label_identity_udp->hide();
    }
}


void OpenRouteDialog::on_button_connect_udp_clicked()
{
    MyUdpSocket *udp_socket = new MyUdpSocket();
    connect(udp_socket, &MyUdpSocket::socketErrorOccurred, this, &OpenRouteDialog::socketErrorOccurred);
    if(ui->box_identity_udp->currentText() == tr("Master"))
    {
        if(udp_socket->connectTo(QHostAddress(ui->edit_udp_remote_server_addr->text()), ui->box_udp_remote_server_port->value()))
        {
            emit createdRoute(udp_socket, QString("%1:%2 - %3").arg(ui->edit_udp_remote_server_addr->text()).arg(ui->box_udp_remote_server_port->value()).arg(ui->box_udp_protocol->currentText()), protocol_enum_map[ui->box_udp_protocol->currentText()], ui->box_identity_udp->currentText() == tr("Master"));
            hide();
        }
        else
        {
            udp_socket->deleteLater();
        }
    }
    else
    {
        if(udp_socket->bind(QHostAddress(ui->edit_udp_remote_server_addr->text()), ui->box_udp_remote_server_port->value()))
        {
            emit createdRoute(udp_socket, QString("%1:%2 - %3").arg(ui->edit_udp_remote_server_addr->text()).arg(ui->box_udp_remote_server_port->value()).arg(ui->box_udp_protocol->currentText()), protocol_enum_map[ui->box_udp_protocol->currentText()], ui->box_identity_udp->currentText() == tr("Master"));
            hide();
        }
        else
        {
            udp_socket->deleteLater();
        }
    }

}

