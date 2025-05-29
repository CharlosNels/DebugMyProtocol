#ifndef MODBUSFRAMEINFO_H
#define MODBUSFRAMEINFO_H

#include <QString>

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
