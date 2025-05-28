#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QMap>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QMessageBox>
#include "modbuswidget.h"
#include "addregdialog.h"
#include "modbus_rtu.h"
#include "modbus_ascii.h"
#include "modbus_tcp.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<ModbusRegReadDefinitions*>("ModbusRegReadDefinitions*");
    m_open_route_dialog = new OpenRouteDialog(this);
    connect(m_open_route_dialog, &OpenRouteDialog::createdRoute, this, &MainWindow::routeCreated);
    m_open_route_dialog->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_Route_triggered()
{
    m_open_route_dialog->show();
}

void MainWindow::routeCreated(QIODevice *com, QString name, int protocol, bool is_master)
{
    if(protocol == MODBUS_RTU ||
        protocol == MODBUS_ASCII ||
        protocol == MODBUS_TCP ||
        protocol == MODBUS_UDP)
    {
        ModbusBase *modbus{ nullptr };
        switch(protocol)
        {
            case MODBUS_RTU:
                modbus = new Modbus_RTU();
                break;
            case MODBUS_ASCII:
                modbus = new Modbus_ASCII();
                break;
            case MODBUS_TCP:
            case MODBUS_UDP:
                modbus = new Modbus_TCP();
        }
        if(!modbus)
        {
            return;
        }
        ModbusWidget *modbus_widget = new ModbusWidget(is_master, com, modbus, protocol);
        modbus_widget->setWindowTitle(name + QString(" - %1" ).arg(is_master ? tr("Master") : tr("Slave")));
        ui->mdi_area_modbus->addSubWindow(modbus_widget);
        modbus_widget->show();
    }
}

