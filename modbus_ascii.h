#ifndef MODBUS_ASCII_H
#define MODBUS_ASCII_H

#include <QObject>
#include "ModbusFrameInfo.h"

class Modbus_ASCII : public QObject
{
    Q_OBJECT
private:
    explicit Modbus_ASCII(QObject *parent = nullptr);
};

#endif // MODBUS_ASCII_H
