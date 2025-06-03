#ifndef MODBUSREGREADDEFINITIONS_H
#define MODBUSREGREADDEFINITIONS_H
#include <QObject>

struct ModbusRegReadDefinitions
{
    bool is_master{true};
    quint8 id;
    quint8 function;
    quint16 reg_addr;
    quint16 quantity;
    qint32 scan_rate;
    QByteArray packet;
};

#endif // MODBUSREGREADDEFINITIONS_H
