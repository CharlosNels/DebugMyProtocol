#include "mapdefines.h"

MapDefines MapDefine;

MapDefines::MapDefines(QObject *parent)
    : QObject{parent}
{}

void MapDefines::init()
{
    modbus_function_map = {
        {tr("01 Read Coils (0x)"),ModbusReadCoils},
        {tr("02 Read Descrete Inputs (1x)"), ModbusReadDescreteInputs},
        {tr("03 Read Holding Registers (4x)"), ModbusReadHoldingRegisters},
        {tr("04 Read Input Registers (3x)"), ModbusReadInputRegisters},
        {tr("05 Write Single Coil"), ModbusWriteSingleCoil},
        {tr("06 Write Single Register"), ModbusWriteSingleRegister},
        {tr("15 Write Multiple Coils"), ModbusWriteMultipleCoils},
        {tr("16 Write Multiple Registers"), ModbusWriteMultipleRegisters}
    };
    modbus_slave_function_map = {
        {tr("01 Coil Status (0x)"), ModbusCoilStatus},
        {tr("02 Input Status (1x)"), ModbusInputStatus},
        {tr("03 Holding Registers (4x)"), ModbusHoldingRegisters},
        {tr("04 Input Registers (3x)"), ModbusInputRegisters}
    };
    modbus_error_code_map = {
        {ModbusErrorCode_Timeout,                                   tr("Timeout Error")},
        {ModbusErrorCode_Illegal_Function,                          tr("Illegal Function")},
        {ModbusErrorCode_Illegal_Data_Address,                      tr("Illegal Data Address")},
        {ModbusErrorCode_Illegal_Data_Value,                        tr("Illegal Data Value")},
        {ModbusErrorCode_Slave_Device_Failure,                      tr("Slave Device Failure")},
        {ModbusErrorCode_Acknowledge,                               tr("Acknowledge")},
        {ModbusErrorCode_Slave_Device_Busy,                         tr("Slave Device Busy")},
        {ModbusErrorCode_Negative_Acknowledgment,                   tr("Negative Acknowledgment")},
        {ModbusErrorCode_Memory_Parity_Error,                       tr("Memory Parity Error")},
        {ModbusErrorCode_Gateway_Path_Unavailable,                  tr("Gateway Path Unavailable")},
        {ModbusErrorCode_Gateway_Target_Device_Failed_To_Respond,   tr("Gateway Target Device Failed To Respond")}
    };
    modbus_error_code_comment_map = {
        {ModbusErrorCode_Timeout,                                   tr("The slave did not reply within the specified time.")},
        {ModbusErrorCode_Illegal_Function,                          tr("The function code received in the request is not an authorized action for the slave. The slave may be in the wrong state to process a specific request.")},
        {ModbusErrorCode_Illegal_Data_Address,                      tr("The data address received by the slave is not an authorized address for the slave.")},
        {ModbusErrorCode_Illegal_Data_Value,                        tr("The value in the request data field is not an authorized value for the slave.")},
        {ModbusErrorCode_Slave_Device_Failure,                      tr("The slave fails to perform a requested action because of an unrecoverable error.")},
        {ModbusErrorCode_Acknowledge,                               tr("The slave accepts the request but needs a long time to process it.")},
        {ModbusErrorCode_Slave_Device_Busy,                         tr("The slave is busy processing another command. The master must send the request once the slave is available.")},
        {ModbusErrorCode_Negative_Acknowledgment,                   tr("The slave cannot perform the programming request sent by the master.")},
        {ModbusErrorCode_Memory_Parity_Error,                       tr("The slave detects a parity error in the memory when attempting to read extended memory.")},
        {ModbusErrorCode_Gateway_Path_Unavailable,                  tr("The gateway is overloaded or not correctly configured.")},
        {ModbusErrorCode_Gateway_Target_Device_Failed_To_Respond,   tr("The slave is not present on the network.")}
    };
    format_map = {
        {tr("Signed"),                              CellFormat::Format_Signed},
        {tr("Unsigned"),                            CellFormat::Format_Unsigned},
        {tr("Hex"),                                 CellFormat::Format_Hex},
        {tr("Binary"),                              CellFormat::Format_Binary},
        {tr("Int32 Big-endian"),                    CellFormat::Format_32_Bit_Signed_Big_Endian},
        {tr("Int32 Little-endian"),                 CellFormat::Format_32_Bit_Signed_Little_Endian},
        {tr("Int32 Big-endian byte swap"),          CellFormat::Format_32_Bit_Signed_Big_Endian_Byte_Swap},
        {tr("Int32 Little-endian byte swap"),       CellFormat::Format_32_Bit_Signed_Little_Endian_Byte_Swap},
        {tr("UInt32 Big-endian"),                   CellFormat::Format_32_Bit_Unsigned_Big_Endian},
        {tr("UInt32 Little-endian"),                CellFormat::Format_32_Bit_Unsigned_Little_Endian},
        {tr("UInt32 Big-endian byte swap"),         CellFormat::Format_32_Bit_Unsigned_Big_Endian_Byte_Swap},
        {tr("UInt32 Little-endian byte swap"),      CellFormat::Format_32_Bit_Unsigned_Little_Endian_Byte_Swap},
        {tr("Int64 Big-endian"),                    CellFormat::Format_64_Bit_Signed_Big_Endian},
        {tr("Int64 Little-endian"),                 CellFormat::Format_64_Bit_Signed_Little_Endian},
        {tr("Int64 Big-endian byte swap"),          CellFormat::Format_64_Bit_Signed_Big_Endian_Byte_Swap},
        {tr("Int64 Little-endian byte swap"),       CellFormat::Format_64_Bit_Signed_Little_Endian_Byte_Swap},
        {tr("UInt64 Big-endian"),                   CellFormat::Format_64_Bit_Unsigned_Big_Endian},
        {tr("UInt64 Little-endian"),                CellFormat::Format_64_Bit_Unsigned_Little_Endian},
        {tr("UInt64 Big-endian byte swap"),         CellFormat::Format_64_Bit_Unsigned_Big_Endian_Byte_Swap},
        {tr("UInt64 Little-endian byte swap"),      CellFormat::Format_64_Bit_Unsigned_Little_Endian_Byte_Swap},
        {tr("Float Big-endian"),                    CellFormat::Format_32_Bit_Float_Big_Endian},
        {tr("Float Little-endian"),                 CellFormat::Format_32_Bit_Float_Little_Endian},
        {tr("Float Big-endian byte swap"),          CellFormat::Format_32_Bit_Float_Big_Endian_Byte_Swap},
        {tr("Float Little-endian byte swap"),       CellFormat::Format_32_Bit_Float_Little_Endian_Byte_Swap},
        {tr("Double Big-endian"),                   CellFormat::Format_64_Bit_Float_Big_Endian},
        {tr("Double Little-endian"),                CellFormat::Format_64_Bit_Float_Little_Endian},
        {tr("Double Big-endian byte swap"),         CellFormat::Format_64_Bit_Float_Big_Endian_Byte_Swap},
        {tr("Double Little-endian byte swap"),      CellFormat::Format_64_Bit_Float_Little_Endian_Byte_Swap}
    };
    baud_map = {
        {"1200", QSerialPort::Baud1200},
        {"2400", QSerialPort::Baud2400},
        {"4800", QSerialPort::Baud4800},
        {"9600", QSerialPort::Baud9600},
        {"19200", QSerialPort::Baud19200},
        {"38400", QSerialPort::Baud38400},
        {"57600", QSerialPort::Baud57600},
        {"115200", QSerialPort::Baud115200}
    };
    data_bits_map = {
        {tr("5 bit"), QSerialPort::Data5},
        {tr("6 bit"), QSerialPort::Data6},
        {tr("7 bit"), QSerialPort::Data7},
        {tr("8 bit"), QSerialPort::Data8}
    };
    parity_map = {
        {tr("None"), QSerialPort::NoParity},
        {tr("Even"), QSerialPort::EvenParity},
        {tr("Odd"), QSerialPort::OddParity},
        {tr("Space"), QSerialPort::SpaceParity},
        {tr("Mark"), QSerialPort::MarkParity}
    };
    stop_bits_map = {
        {tr("1 bit"), QSerialPort::OneStop},
        {tr("1.5 bits"), QSerialPort::OneAndHalfStop},
        {tr("2 bits"), QSerialPort::TwoStop}
    };
    flow_control_map = {
        {tr("None"), QSerialPort::NoFlowControl},
        {"RTS/CTS", QSerialPort::HardwareControl},
        {"XON/XOFF", QSerialPort::SoftwareControl}
    };
    protocol_map = {
        {tr("Serial Port"), {"Modbus-RTU", "Modbus-ASCII"}},
        {tr("TCP-Server"), {"Modbus-TCP", "Modbus-RTU", "Modbus-ASCII"}},
        {tr("TCP-Client"), {"Modbus-TCP", "Modbus-RTU", "Modbus-ASCII"}},
        {"UDP", {"Modbus-UDP","Modbus-RTU","Modbus-ASCII"}}
    };
    protocol_enum_map = {
        {"Modbus-RTU", MODBUS_RTU},
        {"Modbus-ASCII", MODBUS_ASCII},
        {"Modbus-TCP", MODBUS_TCP},
        {"Modbus-UDP", MODBUS_UDP}
    };

}
