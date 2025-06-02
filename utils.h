#ifndef UTILS_H
#define UTILS_H
#include "register_value_t.h"
#include <QByteArray>
#include <QtEndian>

quint16 CRC_16(QByteArray data, int len);

quint8 LRC(QByteArray data, int len);

int pageConvert(int num, int page);

template <class T> T myFromLittleEndianByteSwap(const void *src) {
    const size_t size = sizeof(T);
    char const *src_ = (const char *)src;
    T ret{};
    char *dest = (char *)&ret;
    for (int i = 0; i < size; i += 2) {
        dest[i] = src_[i + 1];
        dest[i + 1] = src_[i];
    }
    return ret;
}

template <class T> void myToLittleEndianByteSwap(T val, void *dest) {
    const size_t size = sizeof(T);
    char *src = (char *)&val;
    char *dest_ = (char *)dest;
    for (int i = 0; i < size; i += 2) {
        dest_[i] = src[i + 1];
        dest_[i + 1] = src[i];
    }
}

template <class T> T myFromBigEndianByteSwap(const void *src) {
    T ret{};
    char *dest = (char *)&ret;
    char const *src_ = (const char *)src;
    for (int i = 0; i < sizeof(T); ++i) {
        dest[i] = src_[sizeof(T) - i - 1];
    }
    return ret;
}

template <class T> void myToBigEndianByteSwap(T val, void *dest) {
    char *src = (char *)&val;
    char *dest_ = (char *)dest;
    for (int i = 0; i < sizeof(T); ++i) {
        dest_[i] = src[sizeof(T) - i - 1];
    }
}

template <class T> T myFromLittleEndian(const void *src) {
    T ret{};
    char *dest = (char *)&ret;
    memcpy(dest, src, sizeof(T));
    return ret;
}

template <class T> void myToLittleEndian(T val, void *dest) { memcpy(dest, &val, sizeof(T)); }

template <class T> T myFromBigEndian(const void *src) {
    char const *src_ = (const char *)src;
    const size_t size = sizeof(T);
    T ret{};
    char *dest = (char *)&ret;
    for (int i = 0; i < size; i += 2) {
        dest[i] = src_[size - i - 2];
        dest[i + 1] = src_[size - i - 1];
    }
    return ret;
}

template <class T> void myToBigEndian(T val, void *dest) {
    const size_t size = sizeof(T);
    char *src = (char *)&val;
    char *dest_ = (char *)dest;
    for (int i = 0; i < size; i += 2) {
        dest_[i] = src[size - i - 2];
        dest_[i + 1] = src[size - i - 1];
    }
}

template <class K, class V> K getKeyByValue(const QMap<K, V> &map, V val) {
    for (auto x = map.constBegin(); x != map.constEnd(); ++x) {
        if (x.value() == val) {
            return x.key();
        }
    }
    return K();
}

quint16 getBit(quint8 data, int bit_index);

void setBit(quint8 &data, int bit_index, quint16 value);

void setModbusPacketTransID(QByteArray &pack, quint16 trans_id);

double get_value_by_format(register_value_t *reg_val);

#endif // UTILS_H
