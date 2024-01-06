#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include <QObject>
#include "ModbusFrameInfo.h"

/*
 * The format of modbus-udp packets is the same as that of modbus-tcp
 */

class Modbus_TCP : public QObject
{
    Q_OBJECT
public:
    static QByteArray masterFrame2Pack(const ModbusFrameInfo &frame_info);
    static ModbusFrameInfo masterPack2Frame(const QByteArray &pack);
    static QByteArray slaveFrame2Pack(const ModbusFrameInfo &frame_info);
    static ModbusFrameInfo slavePack2Frame(const QByteArray &pack);
    static bool validPack(const QByteArray &pack);

private:
    explicit Modbus_TCP(QObject *parent = nullptr);
};

#endif // MODBUS_TCP_H
