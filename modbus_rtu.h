#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include "ModbusBase.h"

class Modbus_RTU : public ModbusBase
{
public:
    QByteArray masterFrame2Pack(const ModbusFrameInfo &frame_info);
    ModbusFrameInfo masterPack2Frame(const QByteArray &pack);
    QByteArray slaveFrame2Pack(const ModbusFrameInfo &frame_info);
    ModbusFrameInfo slavePack2Frame(const QByteArray &pack);
    bool validPack(const QByteArray &pack);
    Modbus_RTU();
};

#endif // MODBUS_RTU_H
