#ifndef MODBUSREGREADDEFINITIONS_H
#define MODBUSREGREADDEFINITIONS_H
#include <QObject>

struct ModbusRegReadDefinitions
{
    bool is_master{true};
    quint8 id;
    quint8 function;
    quint16 reg_addr;
    quint32 quantity;
    quint32 scan_rate;
    QByteArray packet;
};

#endif // MODBUSREGREADDEFINITIONS_H
