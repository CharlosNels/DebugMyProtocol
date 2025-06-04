#ifndef OPENROUTEDIALOG_H
#define OPENROUTEDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QList>
#include <QMap>
#include "EnumHeader.h"

namespace Ui {
class OpenRouteDialog;
}

class MyTcpSocket;
class MyTcpServer;
class QPushButton;

class OpenRouteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenRouteDialog(QWidget *parent = nullptr);
    ~OpenRouteDialog();

signals:
    void createdRoute(QIODevice *com, QString name, int protocol, bool is_master);

protected:
    void changeEvent(QEvent *event) override;

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

    void modify_button_clicked();

    void delete_button_clicked();

private:
    Ui::OpenRouteDialog *ui;

    QList<MyTcpServer*> m_listening_servers;
    QList<QPushButton*> m_modify_buttons;
    QList<QPushButton*> m_delete_buttons;
    QMap<MyTcpServer*, QString> m_server_protocol_map;
    QMap<MyTcpServer*, bool> m_server_identity_map;
    MyTcpSocket *m_connecting_client;

    QWidget *m_parent_window;
};

#endif // OPENROUTEDIALOG_H
