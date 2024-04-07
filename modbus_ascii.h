#ifndef MODBUS_ASCII_H
#define MODBUS_ASCII_H

#include "ModbusBase.h"

class Modbus_ASCII : public ModbusBase
{
public:
    QByteArray masterFrame2Pack(const ModbusFrameInfo &frame_info);
    ModbusFrameInfo masterPack2Frame(const QByteArray &pack);
    QByteArray slaveFrame2Pack(const ModbusFrameInfo &frame_info);
    ModbusFrameInfo slavePack2Frame(const QByteArray &pack);
    bool validPack(const QByteArray &pack);
    Modbus_ASCII();
private:
    static const char pack_start_character;
    static const QByteArray pack_terminator;
};

#endif // MODBUS_ASCII_H
