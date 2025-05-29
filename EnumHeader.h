#ifndef ENUMHEADER_H
#define ENUMHEADER_H


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

enum Protocols{
    MODBUS_RTU,
    MODBUS_ASCII,
    MODBUS_TCP,
    MODBUS_UDP,
};

enum CellFormat{
    Format_None = 0,
    Format_Coil,
    Format_Signed,
    Format_Unsigned,
    Format_Hex,
    Format_Ascii_Hex,
    Format_Binary,
    Format_32_Bit_Signed_Big_Endian = 32,
    Format_32_Bit_Signed_Little_Endian,
    Format_32_Bit_Signed_Big_Endian_Byte_Swap,
    Format_32_Bit_Signed_Little_Endian_Byte_Swap,
    Format_32_Bit_Unsigned_Big_Endian,
    Format_32_Bit_Unsigned_Little_Endian,
    Format_32_Bit_Unsigned_Big_Endian_Byte_Swap,
    Format_32_Bit_Unsigned_Little_Endian_Byte_Swap,
    Format_32_Bit_Float_Big_Endian,
    Format_32_Bit_Float_Little_Endian,
    Format_32_Bit_Float_Big_Endian_Byte_Swap,
    Format_32_Bit_Float_Little_Endian_Byte_Swap,
    Format_64_Bit_Signed_Big_Endian = 64,
    Format_64_Bit_Signed_Little_Endian,
    Format_64_Bit_Signed_Big_Endian_Byte_Swap,
    Format_64_Bit_Signed_Little_Endian_Byte_Swap,
    Format_64_Bit_Unsigned_Big_Endian,
    Format_64_Bit_Unsigned_Little_Endian,
    Format_64_Bit_Unsigned_Big_Endian_Byte_Swap,
    Format_64_Bit_Unsigned_Little_Endian_Byte_Swap,
    Format_64_Bit_Float_Big_Endian,
    Format_64_Bit_Float_Little_Endian,
    Format_64_Bit_Float_Big_Endian_Byte_Swap,
    Format_64_Bit_Float_Little_Endian_Byte_Swap,
};

#endif // ENUMHEADER_H
