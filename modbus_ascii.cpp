#include "modbus_ascii.h"
#include <QByteArray>
#include <QDebug>
#include "utils.h"


const char Modbus_ASCII::pack_start_character = ':';
const QByteArray Modbus_ASCII::pack_terminator = QByteArray("\x0D\x0A",2);

QByteArray Modbus_ASCII::masterFrame2Pack(const ModbusFrameInfo &frame_info)
{
    QByteArray ret{};
    ret.append(quint8(frame_info.id));
    ret.append(quint8(frame_info.function));
    ret.append(quint8(frame_info.reg_addr >> 8 & 0xFF));
    ret.append(quint8(frame_info.reg_addr & 0XFF));
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
        unsigned char const *coils = (unsigned char const *)frame_info.reg_values;
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
    ret.append(LRC(ret,ret.size()));
    return ret.toHex().toUpper().prepend(pack_start_character).append(pack_terminator);
}

ModbusFrameInfo Modbus_ASCII::masterPack2Frame(const QByteArray &pack)
{
    ModbusFrameInfo ret{};
    QByteArray hex_pack = QByteArray::fromHex(pack.mid(1, pack.size() - 3));
    ret.id = quint8(hex_pack[0]);
    ret.function = quint8(hex_pack[1]);
    if(ret.function == ModbusReadCoils ||
        ret.function == ModbusReadDescreteInputs)
    {
        ret.quantity = quint8(hex_pack[2]);
        unsigned char *coils = (unsigned char *)ret.reg_values;
        for(int i = 0;i < ret.quantity;++i)
        {
            coils[i] = hex_pack[3 + i];
        }
    }
    else if(ret.function == ModbusReadHoldingRegisters ||
             ret.function == ModbusReadInputRegisters)
    {
        ret.quantity = quint8(hex_pack[2]) / 2;
        for(int i = 0;i < ret.quantity;++i)
        {
            ret.reg_values[i] = quint16(hex_pack[3 + 2 * i]) << 8 | quint8(hex_pack[4 + 2 * i]);
        }
    }
    else if(ret.function == ModbusWriteSingleCoil ||
             ret.function == ModbusWriteSingleRegister)
    {
        ret.quantity = 1;
        unsigned char *coils = (unsigned char *)ret.reg_values;
        coils[0] = hex_pack[4];
        coils[1] = hex_pack[5];
    }
    else if(ret.function == ModbusWriteMultipleCoils
             || ret.function == ModbusWriteMultipleRegisters)
    {
        ret.quantity = quint16(hex_pack[4]) << 8 | quint8(hex_pack[5]);
    }
    else if(ret.function > ModbusFunctionError)
    {
        ret.reg_values[0] = hex_pack[2];
    }
    else
    {
        qDebug()<<"unknown modbus function : "<<ret.function;
    }
    return ret;
}

QByteArray Modbus_ASCII::slaveFrame2Pack(const ModbusFrameInfo &frame_info)
{
    QByteArray ret;
    ret.append(quint8(frame_info.id));
    ret.append(quint8(frame_info.function));
    if(frame_info.function == ModbusCoilStatus ||
        frame_info.function == ModbusInputStatus)
    {
        quint8 byte_num = quint8(pageConvert(frame_info.quantity, 8));
        unsigned char const *coils = (unsigned char const *)frame_info.reg_values;
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
    ret.append(LRC(ret, ret.size()));
    return ret.toHex().toUpper().prepend(pack_start_character).append(pack_terminator);
}

ModbusFrameInfo Modbus_ASCII::slavePack2Frame(const QByteArray &pack)
{
    ModbusFrameInfo ret{};
    QByteArray hex_pack = QByteArray::fromHex(pack.mid(1, pack.size() - 3));
    ret.id = quint8(hex_pack[0]);
    ret.function = quint8(hex_pack[1]);
    if(ret.function == ModbusReadCoils ||
        ret.function == ModbusReadDescreteInputs ||
        ret.function == ModbusReadHoldingRegisters ||
        ret.function == ModbusReadInputRegisters)
    {
        ret.reg_addr = quint16(hex_pack[2]) << 8 | quint8(hex_pack[3]);
        ret.quantity = quint16(hex_pack[4]) << 8 | quint8(hex_pack[5]);
    }
    else if(ret.function == ModbusWriteSingleCoil ||
             ret.function == ModbusWriteSingleRegister)
    {
        ret.reg_addr = quint16(hex_pack[2]) << 8 | quint8(hex_pack[3]);
        ret.quantity = 1;
        ret.reg_values[0] = quint16(hex_pack[4]) << 8 | quint8(hex_pack[5]);
    }
    else if(ret.function == ModbusWriteMultipleCoils)
    {
        ret.reg_addr = quint16(hex_pack[2]) << 8 | quint8(hex_pack[3]);
        ret.quantity = quint16(hex_pack[4]) << 8 | quint8(hex_pack[5]);
        int byte_num =  quint8(hex_pack[6]);
        unsigned char *coils = (unsigned char *)ret.reg_values;
        for(int i = 0;i < byte_num;++i)
        {
            coils[i] = hex_pack[7 + i];
        }
    }
    else if(ret.function == ModbusWriteMultipleRegisters)
    {
        ret.reg_addr = quint16(hex_pack[2]) << 8 | quint8(hex_pack[3]);
        ret.quantity = quint16(hex_pack[4]) << 8 | quint8(hex_pack[5]);
        for(int i = 0;i < ret.quantity; ++i)
        {
            ret.reg_values[i] = quint16(hex_pack[7 + 2 * i]) << 8 | quint8(hex_pack[8 + i * 2]);
        }
    }
    return ret;
}

bool Modbus_ASCII::validPack(const QByteArray &pack)
{
    QByteArray hex_pack = QByteArray::fromHex(pack.mid(1,pack.size() - 3));
    return pack.startsWith(pack_start_character) && pack.endsWith(pack_terminator) && (LRC(hex_pack, hex_pack.size()) == 0);
}

Modbus_ASCII::Modbus_ASCII()
{}
