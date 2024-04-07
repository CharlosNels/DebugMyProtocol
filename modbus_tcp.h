#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include "ModbusBase.h"

/*
 * The format of modbus-udp packets is the same as that of modbus-tcp
 */

class Modbus_TCP : public ModbusBase
{
public:
    QByteArray masterFrame2Pack(const ModbusFrameInfo &frame_info);
    ModbusFrameInfo masterPack2Frame(const QByteArray &pack);
    QByteArray slaveFrame2Pack(const ModbusFrameInfo &frame_info);
    ModbusFrameInfo slavePack2Frame(const QByteArray &pack);
    bool validPack(const QByteArray &pack);
    Modbus_TCP();
};

#endif // MODBUS_TCP_H
