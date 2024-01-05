#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include <QObject>
#include "ModbusFrameInfo.h"

/*
 * The format of modbus-udp packets is the same as that of modbus-tcp
 */

class Modbus_TCP : public QObject
{
    Q_OBJECT
public:
    static QByteArray frame2pack(const ModbusFrameInfo &frame_info);
    static ModbusFrameInfo pack2frame(const QByteArray &pack);

private:
    explicit Modbus_TCP(QObject *parent = nullptr);
};

#endif // MODBUS_TCP_H
