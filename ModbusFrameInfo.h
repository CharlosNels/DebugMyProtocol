#ifndef MODBUSFRAMEINFO_H
#define MODBUSFRAMEINFO_H

#include <QString>

enum ModbusFunctions{
    ModbusReadCoils = 0x01,
    ModbusReadDescreteInputs = 0x02,
    ModbusReadHoldingRegisters = 0x03,
    ModbusReadInputRegisters = 0x04,
    ModbusWriteSingleCoil = 0x05,
    ModbusWriteSingleRegister = 0x06,
    ModbusWriteMultipleCoils = 0x0F,
    ModbusWriteMultipleRegisters = 0x10,
    ModbusCoilStatus = ModbusReadCoils,
    ModbusInputStatus = ModbusReadDescreteInputs,
    ModbusHoldingRegisters = ModbusReadHoldingRegisters,
    ModbusInputRegisters = ModbusReadInputRegisters,
    ModbusFunctionError = 0x80,
};

enum ModbusErrorCode{
    ModbusErrorCode_Timeout = -1,
    ModbusErrorCode_OK = 0x00,
    ModbusErrorCode_Illegal_Function = 0x01,
    ModbusErrorCode_Illegal_Data_Address = 0x02,
    ModbusErrorCode_Illegal_Data_Value = 0x03,
    ModbusErrorCode_Slave_Device_Failure = 0x04,
    ModbusErrorCode_Acknowledge = 0x05,
    ModbusErrorCode_Slave_Device_Busy = 0x06,
    ModbusErrorCode_Negative_Acknowledgment = 0x07,
    ModbusErrorCode_Memory_Parity_Error = 0x08,
    ModbusErrorCode_Gateway_Path_Unavailable = 0x10,
    ModbusErrorCode_Gateway_Target_Device_Failed_To_Respond = 0x11,
};

struct ModbusFrameInfo{
    //tcp,udp transaction identifier
    quint16 trans_id{};
    //master or slave ID
    quint8 id{};
    //function code
    quint8 function{};
    //register or coil address
    quint16 reg_addr{};
    quint16 quantity{};
    quint16 reg_values[2000]{0};

    QString toString() const
    {
        QString ret = QString("trans_id:%1 id:%2 function:%3 reg_addr:%4 quantity:%5 reg_values:%6")
                          .arg(trans_id).arg(id).arg(function).arg(reg_addr).arg(quantity)
                          .arg(QString(QByteArray((char*)reg_values,quantity * 2).toHex(' ').toUpper()));
        return ret;
    }
};

#endif // MODBUSFRAMEINFO_H
