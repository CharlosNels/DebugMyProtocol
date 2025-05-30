#ifndef REGISTER_VALUE_T_H
#define REGISTER_VALUE_T_H
#include "EnumHeader.h"
#include <QString>

struct register_value_t{
    void *addr;
    CellFormat format;
    quint16 reg_addr;
    QString reg_name;
    bool operator<(const register_value_t another)const{
        if(addr != another.addr)
        {
            return addr < another.addr;
        }
        if(format != another.format)
        {
            return format < another.format;
        }
        if(reg_addr != another.reg_addr)
        {
            return reg_addr < another.reg_addr;
        }
        return reg_name < another.reg_name;
    }
};

#endif // REGISTER_VALUE_T_H
