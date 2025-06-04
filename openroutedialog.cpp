#include "openroutedialog.h"
#include "ui_openroutedialog.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QSerialPortInfo>
#include <QMainWindow>
#include <QDebug>
#include <QTableWidgetItem>
#include "floatbox.h"
#include "mytcpsocket.h"
#include "myudpsocket.h"
#include "mytcpserver.h"
#include "mapdefines.h"

OpenRouteDialog::OpenRouteDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OpenRouteDialog), m_connecting_client{nullptr}, m_parent_window(parent)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);

    ui->server_list_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    QList<QSerialPortInfo> serial_ports = QSerialPortInfo::availablePorts();
    for(const auto &x : serial_ports)
    {
        ui->box_port_name->addItem(x.portName());
    }
    ui->box_baud_rate->addItems(MapDefine.baud_map.keys());
    ui->box_parity->addItems(MapDefine.parity_map.keys());
    ui->box_data_bits->addItems(MapDefine.data_bits_map.keys());
    ui->box_stop_bits->addItems(MapDefine.stop_bits_map.keys());
    ui->box_flow_control->addItems(MapDefine.flow_control_map.keys());
    ui->box_protocol_serial_port->addItems(MapDefine.protocol_map[tr("Serial Port")]);

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

    ui->box_tcp_server_protocol->addItems(MapDefine.protocol_map[tr("TCP-Server")]);


    ui->box_tcp_client_protocol->addItems(MapDefine.protocol_map[tr("TCP-Client")]);

    ui->box_udp_protocol->addItems(MapDefine.protocol_map[("UDP")]);
}

OpenRouteDialog::~OpenRouteDialog()
{
    delete ui;
}

void OpenRouteDialog::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
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
        emit createdRoute(m_connecting_client, QString("%1:%2 - %3").arg(m_connecting_client->peerAddress()).arg(m_connecting_client->peerPort()).arg(ui->box_tcp_client_protocol->currentText()),MapDefine.protocol_enum_map[ui->box_tcp_client_protocol->currentText()], ui->box_identity_tcp_client->currentText() == tr("Master"));
        hide();
        m_connecting_client = nullptr;
    }
    else
    {
        m_connecting_client->deleteLater();
        m_connecting_client = nullptr;
    }
}

void OpenRouteDialog::socketErrorOccurred()
{
    QObject *sock = sender();
    MyTcpSocket *tcp_sock = dynamic_cast<MyTcpSocket*>(sock);
    if(tcp_sock)
    {
        FloatBox::message(tcp_sock->getErrorString(), 3000, m_parent_window->geometry());
    }
    else
    {
        MyUdpSocket *udp_sock = dynamic_cast<MyUdpSocket*>(sock);
        if(udp_sock)
        {
            FloatBox::message(udp_sock->getErrorString(), 3000, m_parent_window->geometry());
        }
        else
        {
            MyTcpServer *tcp_server = dynamic_cast<MyTcpServer*>(sock);
            if(tcp_server)
            {
                FloatBox::message(tcp_server->getErrorString(), 3000, m_parent_window->geometry());
            }
        }
    }

}

void OpenRouteDialog::newTcpConnectionIncoming(MyTcpSocket *sock_ptr)
{
    MyTcpServer *server = dynamic_cast<MyTcpServer*>(sender());
    if(server)
    {
        connect(sock_ptr, &MyTcpSocket::disconnectedFromHost, this, &OpenRouteDialog::tcpSocketDisconnectedFromHost);
        emit createdRoute(sock_ptr,QString("%1:%2 - %3").arg(sock_ptr->peerAddress()).arg(sock_ptr->peerPort()).arg(m_server_protocol_map[server]),MapDefine.protocol_enum_map[m_server_protocol_map[server]], m_server_identity_map[server]);
    }
}

void OpenRouteDialog::on_button_open_serial_port_clicked()
{
    QSerialPort *serial_port = new QSerialPort(ui->box_port_name->currentText());
    serial_port->setBaudRate(MapDefine.baud_map[ui->box_baud_rate->currentText()]);
    serial_port->setDataBits(MapDefine.data_bits_map[ui->box_data_bits->currentText()]);
    serial_port->setStopBits(MapDefine.stop_bits_map[ui->box_stop_bits->currentText()]);
    serial_port->setParity(MapDefine.parity_map[ui->box_parity->currentText()]);
    serial_port->setFlowControl(MapDefine.flow_control_map[ui->box_flow_control->currentText()]);
    if(serial_port->open(QIODevice::ReadWrite))
    {
        emit createdRoute(serial_port, serial_port->portName(), MapDefine.protocol_enum_map[ui->box_protocol_serial_port->currentText()], ui->box_identity_serial_port->currentText() == tr("Master"));
        hide();
    }
    else
    {
        FloatBox::message(serial_port->errorString(), 3000, m_parent_window->geometry());
    }
}


void OpenRouteDialog::on_button_listen_clicked()
{
    MyTcpServer *server = new MyTcpServer();
    connect(server, &MyTcpServer::errorOccurred, this, &OpenRouteDialog::socketErrorOccurred);
    if(server->bind(QHostAddress(ui->box_tcp_server_addr->currentText()),ui->box_tcp_server_port->value()))
    {
        connect(server, &MyTcpServer::newConnectionIncoming, this, &OpenRouteDialog::newTcpConnectionIncoming);
        m_server_protocol_map[server] = ui->box_tcp_server_protocol->currentText();
        m_server_identity_map[server] = ui->box_identity_tcp_server->currentText() == tr("Master");
        m_listening_servers.append(server);

        ui->server_list_table->setRowCount(ui->server_list_table->rowCount() + 1);
        ui->server_list_table->setItem(ui->server_list_table->rowCount() - 1, 0, new QTableWidgetItem(ui->box_tcp_server_addr->currentText()));
        ui->server_list_table->setItem(ui->server_list_table->rowCount() - 1, 1, new QTableWidgetItem(QString::number(ui->box_tcp_server_port->value())));
        QComboBox *proto_box = new QComboBox();
        proto_box->addItems(MapDefine.protocol_map["TCP-Server"]);
        proto_box->setCurrentText(ui->box_tcp_server_protocol->currentText());
        ui->server_list_table->setCellWidget(ui->server_list_table->rowCount() - 1, 2, proto_box);
        QComboBox *identify_box = new QComboBox();
        identify_box->addItems({tr("Master"), tr("Slave")});
        identify_box->setCurrentText(ui->box_identity_tcp_server->currentText());
        ui->server_list_table->setCellWidget(ui->server_list_table->rowCount() - 1, 3, identify_box);
        QPushButton *modify_button = new QPushButton(tr("Modify"));
        connect(modify_button, &QPushButton::clicked, this, &OpenRouteDialog::modify_button_clicked);
        m_modify_buttons.append(modify_button);
        ui->server_list_table->setCellWidget(ui->server_list_table->rowCount() - 1, 4, modify_button);
        QPushButton *delete_button = new QPushButton(tr("Delete"));
        connect(delete_button, &QPushButton::clicked, this, &OpenRouteDialog::delete_button_clicked);
        m_delete_buttons.append(delete_button);
        ui->server_list_table->setCellWidget(ui->server_list_table->rowCount() - 1, 5, delete_button);

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
    if(m_connecting_client != nullptr)
    {
        return;
    }
    MyTcpSocket *client = new MyTcpSocket();
    connect(client, &MyTcpSocket::socketErrorOccurred, this, &OpenRouteDialog::socketErrorOccurred);
    connect(client, &MyTcpSocket::connectFinished, this, &OpenRouteDialog::clientConnectFinished);
    client->connectToHost(ui->edit_tcp_remote_server_addr->text(), ui->box_tcp_remote_server_port->value());
    m_connecting_client = client;
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
            emit createdRoute(udp_socket, QString("%1:%2 - %3").arg(ui->edit_udp_remote_server_addr->text()).arg(ui->box_udp_remote_server_port->value()).arg(ui->box_udp_protocol->currentText()), MapDefine.protocol_enum_map[ui->box_udp_protocol->currentText()], ui->box_identity_udp->currentText() == tr("Master"));
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
            emit createdRoute(udp_socket, QString("%1:%2 - %3").arg(ui->edit_udp_remote_server_addr->text()).arg(ui->box_udp_remote_server_port->value()).arg(ui->box_udp_protocol->currentText()), MapDefine.protocol_enum_map[ui->box_udp_protocol->currentText()], ui->box_identity_udp->currentText() == tr("Master"));
            hide();
        }
        else
        {
            udp_socket->deleteLater();
        }
    }

}

void OpenRouteDialog::modify_button_clicked()
{
    QPushButton *modify_button = dynamic_cast<QPushButton*>(sender());
    if(modify_button)
    {
        int row = m_modify_buttons.indexOf(modify_button);
        QComboBox *proto_box = dynamic_cast<QComboBox*>(ui->server_list_table->cellWidget(row, 2));
        QComboBox *identify_box = dynamic_cast<QComboBox*>(ui->server_list_table->cellWidget(row, 3));
        m_server_protocol_map[m_listening_servers[row]] = proto_box->currentText();
        m_server_identity_map[m_listening_servers[row]] = identify_box->currentText() == tr("Master");
    }

}

void OpenRouteDialog::delete_button_clicked()
{
    QPushButton *delete_button = dynamic_cast<QPushButton*>(sender());
    if(delete_button)
    {
        int row = m_delete_buttons.indexOf(delete_button);
        m_server_protocol_map.remove(m_listening_servers[row]);
        m_server_identity_map.remove(m_listening_servers[row]);
        m_listening_servers[row]->close();
        m_listening_servers[row]->deleteLater();
        m_listening_servers.removeAt(row);
        m_delete_buttons.removeAt(row);
        m_modify_buttons.removeAt(row);
        ui->server_list_table->removeRow(row);
    }
}

