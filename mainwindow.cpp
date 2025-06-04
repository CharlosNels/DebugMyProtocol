#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QMap>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <QTranslator>
#include <QInputDialog>
#include <QFile>
#include "modbuswidget.h"
#include "addregdialog.h"
#include "modbus_rtu.h"
#include "modbus_ascii.h"
#include "modbus_tcp.h"
#include "openroutedialog.h"
#include "ModbusRegReadDefinitions.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<ModbusRegReadDefinitions*>("ModbusRegReadDefinitions*");
    m_open_route_dialog = new OpenRouteDialog(this);
    connect(m_open_route_dialog, &OpenRouteDialog::createdRoute, this, &MainWindow::routeCreated);
    m_open_route_dialog->hide();
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void MainWindow::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
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
        ModbusWidget *modbus_widget = new ModbusWidget(is_master, com, modbus, protocol, this);
        modbus_widget->setWindowTitle(name + QString(" - %1" ).arg(is_master ? tr("Master") : tr("Slave")));
        ui->mdi_area_modbus->addSubWindow(modbus_widget);
        modbus_widget->show();
    }
}

extern int language_index;
extern QTranslator translator;

void MainWindow::on_actionLanguage_triggered()
{
    QString language = QInputDialog::getItem(this, tr("Chose Language"), tr("Chose Language"), {"English", "简体中文"}, language_index, false);
    if(!language.isEmpty())
    {
        QString language_file;
        QString trans_setting_file_name = "./Config/translate_setting";
        if(language == "简体中文")
        {
            language_file = "./zh_CN.qm";
            if(translator.load(language_file))
            {
                language_index = 1;
                QFile file(trans_setting_file_name);
                if(file.open(QIODevice::WriteOnly))
                {
                    file.write(language_file.toUtf8());
                }
                QCoreApplication::installTranslator(&translator);
            }
            else
            {
                qDebug()<<"Load Translation File Failed : "<<language_file;
            }
        }
        else if(language == "English")
        {
            language_file = "./en_US.qm";
            language_index = 0;
            QFile file(trans_setting_file_name);
            if(file.open(QIODevice::WriteOnly))
            {
                file.write(language_file.toUtf8());
            }
            if(!translator.load(language_file))
            {
                qDebug()<<language_file;
            }
            QCoreApplication::installTranslator(&translator);
        }
    }
}

