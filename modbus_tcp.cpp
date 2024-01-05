#include "modbus_tcp.h"

QByteArray Modbus_TCP::frame2pack(const ModbusFrameInfo &frame_info)
{

}

ModbusFrameInfo Modbus_TCP::pack2frame(const QByteArray &pack)
{

}

Modbus_TCP::Modbus_TCP(QObject *parent)
    : QObject{parent}
{}
