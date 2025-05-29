#ifndef REGISTER_VALUE_T_H
#define REGISTER_VALUE_T_H
#include "EnumHeader.h"

struct register_value_t{
    void *addr;
    CellFormat format;
};

#endif // REGISTER_VALUE_T_H
