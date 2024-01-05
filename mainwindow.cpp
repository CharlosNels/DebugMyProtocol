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
#include "mytcpsocket.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<std::error_code>("std::error_code");
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
        ModbusWidget *modbus_widget = new ModbusWidget(is_master, com, protocol);
        modbus_widget->setWindowTitle(name);
        ui->mdi_area_modbus->addSubWindow(modbus_widget);
        modbus_widget->show();
    }
}

