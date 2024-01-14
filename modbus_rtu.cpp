#include "modbus_rtu.h"
#include "utils.h"
#include <QDebug>

QByteArray Modbus_RTU::masterFrame2Pack(const ModbusFrameInfo &frame_info)
{
    QByteArray ret;
    ret.append(quint8(frame_info.id));
    ret.append(quint8(frame_info.function));
    ret.append(quint8(frame_info.reg_addr >> 8 & 0xFF));
    ret.append(quint8(frame_info.reg_addr & 0xFF));
    if(frame_info.function != ModbusWriteSingleCoil &&
        frame_info.function != ModbusWriteSingleRegister)
    {
        ret.append(quint8(frame_info.quantity >> 8 & 0xFF));
        ret.append(quint8(frame_info.quantity & 0xFF));
    }
    if(frame_info.function == ModbusWriteSingleCoil ||
        frame_info.function == ModbusWriteSingleRegister)
    {
        ret.append(quint8(frame_info.reg_values[0] >> 8 & 0xFF));
        ret.append(quint8(frame_info.reg_values[0] & 0xFF));
    }
    if(frame_info.function == ModbusWriteMultipleCoils)
    {
        quint8 byte_num = quint8(pageConvert(frame_info.quantity, 8));
        ret.append(byte_num);
        quint8 const *coils = (quint8 const *)frame_info.reg_values;
        for(int i = 0;i < byte_num;++i)
        {
            ret.append(coils[i]);
        }
    }
    else if(frame_info.function == ModbusWriteMultipleRegisters)
    {
        ret.append(quint8(frame_info.quantity * 2));
        for(int i = 0;i <frame_info.quantity;++i)
        {
            ret.append(quint8(frame_info.reg_values[i] >> 8 & 0xFF));
            ret.append(quint8(frame_info.reg_values[i] & 0xFF));
        }
    }
    quint16 crc_value = CRC_16(ret,ret.size());
    ret.append(quint8(crc_value & 0xFF));
    ret.append(quint8(crc_value >> 8 & 0xFF));
    return ret;
}

ModbusFrameInfo Modbus_RTU::masterPack2Frame(const QByteArray &pack)
{
    ModbusFrameInfo ret;
    ret.id = quint8(pack[0]);
    ret.function = quint8(pack[1]);
    if(ret.function == ModbusReadCoils ||
        ret.function == ModbusReadDescreteInputs)
    {
        ret.quantity = quint8(pack[2]);
        quint8 *coils = (quint8 *)ret.reg_values;
        for(int i = 0;i < ret.quantity;++i)
        {
            coils[i] = pack[3 + i];
        }
    }
    else if(ret.function == ModbusReadHoldingRegisters ||
               ret.function == ModbusReadInputRegisters)
    {
        ret.quantity = quint8(pack[2]) / 2;
        for(int i = 0;i < ret.quantity;++i)
        {
            ret.reg_values[i] = quint16(pack[3 + 2 * i]) << 8 | quint8(pack[4 + 2 * i]);
        }
    }
    else if(ret.function == ModbusWriteSingleCoil ||
               ret.function == ModbusWriteSingleRegister)
    {
        ret.quantity = 1;
        quint8 *coils = (quint8 *)ret.reg_values;
        coils[0] = pack[4];
        coils[1] = pack[5];
    }
    else if(ret.function == ModbusWriteMultipleCoils
        || ret.function == ModbusWriteMultipleRegisters)
    {
        ret.quantity = quint16(pack[4]) << 8 | quint8(pack[5]);
    }
    else if(ret.function > ModbusFunctionError)
    {
        ret.reg_values[0] = pack[2];
    }
    else
    {
        qDebug()<<"unknown modbus function : "<<ret.function;
    }
    return ret;
}

QByteArray Modbus_RTU::slaveFrame2Pack(const ModbusFrameInfo &frame_info)
{
    QByteArray ret;
    ret.append(quint8(frame_info.id));
    ret.append(quint8(frame_info.function));
    if(frame_info.function == ModbusCoilStatus ||
        frame_info.function == ModbusInputStatus)
    {
        quint8 byte_num = quint8(pageConvert(frame_info.quantity, 8));
        quint8 const *coils = (quint8 const *)frame_info.reg_values;
        ret.append(byte_num);
        for(int i = 0; i < byte_num; ++i)
        {
            ret.append(coils[i]);
        }
    }
    else if(frame_info.function == ModbusHoldingRegisters ||
               frame_info.function == ModbusInputRegisters)
    {
        quint8 byte_num = quint8(frame_info.quantity << 1);
        ret.append(quint8(byte_num));
        for(int i = 0;i < frame_info.quantity;++i)
        {
            ret.append(quint8(frame_info.reg_values[i] >> 8 & 0xFF));
            ret.append(quint8(frame_info.reg_values[i] & 0xFF));
        }
    }
    else if(frame_info.function == ModbusWriteSingleCoil ||
               frame_info.function == ModbusWriteSingleRegister)
    {
        ret.append(quint8(frame_info.reg_addr >> 8 & 0xFF));
        ret.append(quint8(frame_info.reg_addr & 0xFF));
        ret.append(quint8(frame_info.reg_values[0] >> 8 & 0xFF));
        ret.append(quint8(frame_info.reg_values[0] & 0xFF));
    }
    else if(frame_info.function == ModbusWriteMultipleCoils ||
               frame_info.function == ModbusWriteMultipleRegisters)
    {
        ret.append(quint8(frame_info.reg_addr >> 8 & 0xFF));
        ret.append(quint8(frame_info.reg_addr & 0xFF));
        ret.append(quint8(frame_info.quantity >> 8 & 0xFF));
        ret.append(quint8(frame_info.quantity & 0xFF));
    }
    else if(frame_info.function > ModbusFunctionError)
    {
        ret.append(quint8(frame_info.reg_values[0] & 0xFF));
    }
    quint16 crc_value = CRC_16(ret, ret.size());
    ret.append(quint8(crc_value & 0xFF));
    ret.append(quint8(crc_value >> 8 & 0xFF));
    return ret;
}

ModbusFrameInfo Modbus_RTU::slavePack2Frame(const QByteArray &pack)
{
    ModbusFrameInfo ret;
    ret.id = quint8(pack[0]);
    ret.function = quint8(pack[1]);
    if(ret.function == ModbusReadCoils ||
        ret.function == ModbusReadDescreteInputs ||
        ret.function == ModbusReadHoldingRegisters ||
        ret.function == ModbusReadInputRegisters)
    {
        ret.reg_addr = quint16(pack[2]) << 8 | quint8(pack[3]);
        ret.quantity = quint16(pack[4]) << 8 | quint8(pack[5]);
    }
    else if(ret.function == ModbusWriteSingleCoil ||
               ret.function == ModbusWriteSingleRegister)
    {
        ret.reg_addr = quint16(pack[2]) << 8 | quint8(pack[3]);
        ret.quantity = 1;
        ret.reg_values[0] = quint16(pack[4]) << 8 | quint8(pack[5]);
    }
    else if(ret.function == ModbusWriteMultipleCoils)
    {
        ret.reg_addr = quint16(pack[2]) << 8 | quint8(pack[3]);
        ret.quantity = quint16(pack[4]) << 8 | quint8(pack[5]);
        int byte_num =  quint8(pack[6]);
        quint8 *coils = (quint8 *)ret.reg_values;
        for(int i = 0;i < byte_num;++i)
        {
            coils[i] = pack[7 + i];
        }
    }
    else if(ret.function == ModbusWriteMultipleRegisters)
    {
        ret.reg_addr = quint16(pack[2]) << 8 | quint8(pack[3]);
        ret.quantity = quint16(pack[4]) << 8 | quint8(pack[5]);
        for(int i = 0;i < ret.quantity; ++i)
        {
            ret.reg_values[i] = quint16(pack[7 + 2 * i]) << 8 | quint8(pack[8 + 2 * i]);
        }
    }
    return ret;
}

bool Modbus_RTU::validPack(const QByteArray &pack)
{
    return CRC_16(pack,pack.size()) == 0;
}

Modbus_RTU::Modbus_RTU(QObject *parent)
    : QObject{parent}
{}
