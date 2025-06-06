#ifndef MODBUSBASE_H
#define MODBUSBASE_H

#include <QObject>

struct ModbusFrameInfo;

class ModbusBase
{
public:
    ModbusBase(){};
    virtual ~ModbusBase(){};
    virtual bool validPack(const QByteArray &pack) = 0;
    virtual ModbusFrameInfo masterPack2Frame(const QByteArray &pack) = 0;
    virtual QByteArray masterFrame2Pack(const ModbusFrameInfo &frame_info) = 0;
    virtual ModbusFrameInfo slavePack2Frame(const QByteArray &pack) = 0;
    virtual QByteArray slaveFrame2Pack(const ModbusFrameInfo &frame_info) = 0;
};

#endif // MODBUSBASE_H
