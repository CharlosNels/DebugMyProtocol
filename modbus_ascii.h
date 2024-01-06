#ifndef MODBUS_ASCII_H
#define MODBUS_ASCII_H

#include <QObject>
#include "ModbusFrameInfo.h"

class Modbus_ASCII : public QObject
{
    Q_OBJECT
public:
    static QByteArray masterFrame2Pack(const ModbusFrameInfo &frame_info);
    static ModbusFrameInfo masterPack2Frame(const QByteArray &pack);
    static QByteArray slaveFrame2Pack(const ModbusFrameInfo &frame_info);
    static ModbusFrameInfo slavePack2Frame(const QByteArray &pack);
    static bool validPack(const QByteArray &pack);

private:
    explicit Modbus_ASCII(QObject *parent = nullptr);
    static const char pack_start_character;
    static const QByteArray pack_terminator;
};

#endif // MODBUS_ASCII_H
