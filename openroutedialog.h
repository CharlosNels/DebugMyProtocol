#ifndef OPENROUTEDIALOG_H
#define OPENROUTEDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QList>
#include <QMap>

namespace Ui {
class OpenRouteDialog;
}

class MyTcpSocket;
class MyUdpSocket;

enum Protocols{
    MODBUS_RTU,
    MODBUS_ASCII,
    MODBUS_TCP,
    MODBUS_UDP,
};

class OpenRouteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenRouteDialog(QWidget *parent = nullptr);
    ~OpenRouteDialog();

signals:
    void createdRoute(QIODevice *com, QString name, int protocol, bool is_master);

private slots:

    void tcpSocketDisconnectedFromHost();

    void clientConnectFinished(bool connected);

    void socketErrorOccurred();

    void newTcpConnectionIncoming(MyTcpSocket *sock_ptr);

    void on_button_open_serial_port_clicked();

    void on_button_listen_clicked();

    void on_button_connect_clicked();

    void on_box_protocol_serial_port_currentTextChanged(const QString &arg1);

    void on_box_tcp_server_protocol_currentTextChanged(const QString &arg1);

    void on_box_tcp_client_protocol_currentTextChanged(const QString &arg1);

    void on_box_udp_protocol_currentTextChanged(const QString &arg1);

    void on_button_connect_udp_clicked();

private:
    Ui::OpenRouteDialog *ui;

    static const QMap<QString, QSerialPort::BaudRate> baud_map;
    static const QMap<QString, QSerialPort::DataBits> data_bits_map;
    static const QMap<QString, QSerialPort::Parity> parity_map;
    static const QMap<QString, QSerialPort::StopBits> stop_bits_map;
    static const QMap<QString, QSerialPort::FlowControl> flow_control_map;
    static const QMap<QString, QList<QString> > protocol_map;
    static const QMap<QString, int> protocol_enum_map;

    QList<MyTcpSocket*> m_listening_servers;
    QMap<MyTcpSocket*, QString> m_server_protocol_map;
    QMap<MyTcpSocket*, bool> m_server_identity_map;
    MyTcpSocket *m_connecting_client;

    QWidget *m_parent_window;
};

#endif // OPENROUTEDIALOG_H
