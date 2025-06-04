#ifndef MAPDEFINES_H
#define MAPDEFINES_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QSerialPort>
#include "EnumHeader.h"

class MapDefines : public QObject
{
    Q_OBJECT
public:
    explicit MapDefines(QObject *parent = nullptr);
    void init();
signals:

public:
    QMap<QString,quint8> modbus_function_map;
    QMap<QString,quint8> modbus_slave_function_map;
    QMap<ModbusErrorCode, QString> modbus_error_code_map;
    QMap<ModbusErrorCode, QString> modbus_error_code_comment_map;
    QMap<QString, int> format_map;
    QMap<QString, QSerialPort::BaudRate> baud_map;
    QMap<QString, QSerialPort::DataBits> data_bits_map;
    QMap<QString, QSerialPort::Parity> parity_map;
    QMap<QString, QSerialPort::StopBits> stop_bits_map;
    QMap<QString, QSerialPort::FlowControl> flow_control_map;
    QMap<QString, QList<QString> > protocol_map;
    QMap<QString, Protocols> protocol_enum_map;
};
extern MapDefines MapDefine;
#endif // MAPDEFINES_H
