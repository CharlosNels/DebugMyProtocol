#include "modbus_tcp.h"
#include "utils.h"
#include <QDebug>


QByteArray Modbus_TCP::masterFrame2Pack(const ModbusFrameInfo &frame_info)
{
    QByteArray ret;
    ret.append(quint8(frame_info.trans_id >> 8 & 0xFF));
    ret.append(quint8(frame_info.trans_id & 0xFF));
    ret.append(qint8(00));
    ret.append(qint8(00));
    QByteArray data_pack;
    data_pack.append(quint8(frame_info.id));
    data_pack.append(quint8(frame_info.function));
    data_pack.append(quint8(frame_info.reg_addr >> 8 & 0xFF));
    data_pack.append(quint8(frame_info.reg_addr & 0xFF));
    if(frame_info.function != ModbusWriteSingleCoil &&
        frame_info.function != ModbusWriteSingleRegister)
    {
        data_pack.append(quint8(frame_info.quantity >> 8 & 0xFF));
        data_pack.append(quint8(frame_info.quantity & 0xFF));
    }
    if(frame_info.function == ModbusWriteSingleCoil ||
        frame_info.function == ModbusWriteSingleRegister)
    {
        data_pack.append(quint8(frame_info.reg_values[0] >> 8 & 0xFF));
        data_pack.append(quint8(frame_info.reg_values[0] & 0xFF));
    }
    if(frame_info.function == ModbusWriteMultipleCoils)
    {
        quint8 byte_num = quint8(pageConvert(frame_info.quantity, 8));
        data_pack.append(byte_num);
        unsigned char const *coils = (unsigned char const *)frame_info.reg_values;
        for(int i = 0;i < byte_num;++i)
        {
            data_pack.append(coils[i]);
        }
    }
    else if(frame_info.function == ModbusWriteMultipleRegisters)
    {
        data_pack.append(quint8(frame_info.quantity * 2));
        for(int i = 0;i <frame_info.quantity;++i)
        {
            data_pack.append(quint8(frame_info.reg_values[i] >> 8 & 0xFF));
            data_pack.append(quint8(frame_info.reg_values[i] & 0xFF));
        }
    }
    quint16 data_pack_size = data_pack.size();
    ret.append(quint8(data_pack_size >> 8 & 0xFF));
    ret.append(quint8(data_pack_size & 0xFF));
    ret.append(data_pack);
    return ret;
}

ModbusFrameInfo Modbus_TCP::masterPack2Frame(const QByteArray &pack)
{
    ModbusFrameInfo ret{};
    ret.trans_id = quint16(pack[0]) << 8 | quint8(pack[1]);
    QByteArray data_pack = pack.mid(6);
    ret.id = quint8(data_pack[0]);
    ret.function = quint8(data_pack[1]);
    if(ret.function == ModbusReadCoils ||
        ret.function == ModbusReadDescreteInputs)
    {
        ret.quantity = quint8(data_pack[2]);
        unsigned char *coils = (unsigned char *)ret.reg_values;
        for(int i = 0;i < ret.quantity;++i)
        {
            coils[i] = data_pack[3 + i];
        }
    }
    else if(ret.function == ModbusReadHoldingRegisters ||
             ret.function == ModbusReadInputRegisters)
    {
        ret.quantity = quint8(data_pack[2]) / 2;
        for(int i = 0;i < ret.quantity;++i)
        {
            ret.reg_values[i] = quint16(data_pack[3 + 2 * i]) << 8 | quint8(data_pack[4 + 2 * i]);
        }
    }
    else if(ret.function == ModbusWriteSingleCoil ||
             ret.function == ModbusWriteSingleRegister)
    {
        ret.quantity = 1;
        unsigned char *coils = (unsigned char *)ret.reg_values;
        coils[0] = data_pack[4];
        coils[1] = data_pack[5];
    }
    else if(ret.function == ModbusWriteMultipleCoils
             || ret.function == ModbusWriteMultipleRegisters)
    {
        ret.quantity = quint16(data_pack[4]) << 8 | quint8(data_pack[5]);
    }
    else if(ret.function > ModbusFunctionError)
    {
        ret.reg_values[0] = data_pack[2];
    }
    else
    {
        qDebug()<<"unknown modbus function : "<<ret.function;
    }
    return ret;
}

QByteArray Modbus_TCP::slaveFrame2Pack(const ModbusFrameInfo &frame_info)
{
    QByteArray ret;
    ret.append(quint8(frame_info.trans_id >> 8 & 0xFF));
    ret.append(quint8(frame_info.trans_id & 0xFF));
    ret.append(qint8(00));
    ret.append(qint8(00));
    QByteArray data_pack;
    data_pack.append(quint8(frame_info.id));
    data_pack.append(quint8(frame_info.function));
    if(frame_info.function == ModbusCoilStatus ||
        frame_info.function == ModbusInputStatus)
    {
        quint8 byte_num = quint8(pageConvert(frame_info.quantity, 8));
        unsigned char const *coils = (unsigned char const *)frame_info.reg_values;
        data_pack.append(byte_num);
        for(int i = 0; i < byte_num; ++i)
        {
            data_pack.append(coils[i]);
        }
    }
    else if(frame_info.function == ModbusHoldingRegisters ||
             frame_info.function == ModbusInputRegisters)
    {
        quint8 byte_num = quint8(frame_info.quantity << 1);
        data_pack.append(quint8(byte_num));
        for(int i = 0;i < frame_info.quantity;++i)
        {
            data_pack.append(quint8(frame_info.reg_values[i] >> 8 & 0xFF));
            data_pack.append(quint8(frame_info.reg_values[i] & 0xFF));
        }
    }
    else if(frame_info.function == ModbusWriteSingleCoil ||
             frame_info.function == ModbusWriteSingleRegister)
    {
        data_pack.append(quint8(frame_info.reg_addr >> 8 & 0xFF));
        data_pack.append(quint8(frame_info.reg_addr & 0xFF));
        data_pack.append(quint8(frame_info.reg_values[0] >> 8 & 0xFF));
        data_pack.append(quint8(frame_info.reg_values[0] & 0xFF));
    }
    else if(frame_info.function == ModbusWriteMultipleCoils ||
             frame_info.function == ModbusWriteMultipleRegisters)
    {
        data_pack.append(quint8(frame_info.reg_addr >> 8 & 0xFF));
        data_pack.append(quint8(frame_info.reg_addr & 0xFF));
        data_pack.append(quint8(frame_info.quantity >> 8 & 0xFF));
        data_pack.append(quint8(frame_info.quantity & 0xFF));
    }
    else if(frame_info.function > ModbusFunctionError)
    {
        data_pack.append(quint8(frame_info.reg_values[0] & 0xFF));
    }
    quint16 data_pack_size = data_pack.size();
    ret.append(quint8(data_pack_size >> 8 & 0xFF));
    ret.append(quint8(data_pack_size & 0xFF));
    ret.append(data_pack);
    return ret;
}

ModbusFrameInfo Modbus_TCP::slavePack2Frame(const QByteArray &pack)
{
    ModbusFrameInfo ret{};
    ret.trans_id = quint16(pack[0]) << 8 | quint8(pack[1]);
    QByteArray data_pack = pack.mid(6);
    ret.id = quint8(data_pack[0]);
    ret.function = quint8(data_pack[1]);
    if(ret.function == ModbusReadCoils ||
        ret.function == ModbusReadDescreteInputs ||
        ret.function == ModbusReadHoldingRegisters ||
        ret.function == ModbusReadInputRegisters)
    {
        ret.reg_addr = quint16(data_pack[2]) << 8 | quint8(data_pack[3]);
        ret.quantity = quint16(data_pack[4]) << 8 | quint8(data_pack[5]);
    }
    else if(ret.function == ModbusWriteSingleCoil ||
             ret.function == ModbusWriteSingleRegister)
    {
        ret.reg_addr = quint16(data_pack[2]) << 8 | quint8(data_pack[3]);
        ret.quantity = 1;
        ret.reg_values[0] = quint16(data_pack[4]) << 8 | quint8(data_pack[5]);
    }
    else if(ret.function == ModbusWriteMultipleCoils)
    {
        ret.reg_addr = quint16(data_pack[2]) << 8 | quint8(data_pack[3]);
        ret.quantity = quint16(data_pack[4]) << 8 | quint8(data_pack[5]);
        int byte_num =  quint8(data_pack[6]);
        unsigned char *coils = (unsigned char *)ret.reg_values;
        for(int i = 0;i < byte_num;++i)
        {
            coils[i] = data_pack[7 + i];
        }
    }
    else if(ret.function == ModbusWriteMultipleRegisters)
    {
        ret.reg_addr = quint16(data_pack[2]) << 8 | quint8(data_pack[3]);
        ret.quantity = quint16(data_pack[4]) << 8 | quint8(data_pack[5]);
        for(int i = 0;i < ret.quantity; ++i)
        {
            ret.reg_values[i] = quint16(data_pack[7 + 2 * i]) << 8 | quint8(data_pack[8 + 2 * i]);
        }
    }
    return ret;
}

bool Modbus_TCP::validPack(const QByteArray &pack)
{
    quint16 data_pack_size = quint16(pack[4]) << 8 | quint8(pack[5]);
    return data_pack_size == pack.size() - 6;
}

Modbus_TCP::Modbus_TCP(QObject *parent)
    : QObject{parent}
{}
