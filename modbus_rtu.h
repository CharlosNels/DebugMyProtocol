#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <QObject>
#include <QByteArray>
#include "ModbusFrameInfo.h"

class Modbus_RTU : public QObject
{
    Q_OBJECT
public:
    static QByteArray masterFrame2Pack(const ModbusFrameInfo &frame_info);
    static ModbusFrameInfo masterPack2Frame(const QByteArray &pack);
    static QByteArray slaveFrame2Pack(const ModbusFrameInfo &frame_info);
    static ModbusFrameInfo slavePack2Frame(const QByteArray &pack);
    static bool validPack(const QByteArray &pack);

private:
    explicit Modbus_RTU(QObject *parent = nullptr);
};

#endif // MODBUS_RTU_H
