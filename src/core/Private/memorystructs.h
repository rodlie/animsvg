#ifndef MEMORYSTRUCTS_H
#define MEMORYSTRUCTS_H

#include "../core_global.h"

struct intMB;
struct intKB;
struct longB;

// int representing mega bytes
struct CORE_EXPORT intMB {
    intMB() : fValue(0) {}
    explicit intMB(const int value) : fValue(value) {}
    explicit intMB(const longB& b);
    explicit intMB(const intKB& kb);
    int fValue;

    intMB operator-() const {
        return intMB(-fValue);
    }
};

// int representing kilo bytes
struct CORE_EXPORT intKB {
    intKB() : fValue(0) {}
    explicit intKB(const int value) : fValue(value) {}
    explicit intKB(const longB& b);
    explicit intKB(const intMB& mb);
    int fValue;

    intKB operator-() const {
        return intKB(-fValue);
    }
};

// qint64 representing bytes
struct CORE_EXPORT longB {
    longB() : fValue(0) {}
    explicit longB(const qint64 value) : fValue(value) {}
    explicit longB(const intKB& kb);
    explicit longB(const intMB& mb);
    qint64 fValue;

    longB operator-() const {
        return longB(-fValue);
    }
};

CORE_EXPORT
bool operator>(const intMB& a, const intMB& b);
CORE_EXPORT
bool operator<(const intMB& a, const intMB& b);
CORE_EXPORT
intMB operator-(const intMB& a, const intMB& b);
CORE_EXPORT
intMB operator+(const intMB& a, const intMB& b);
CORE_EXPORT
intMB operator*(const intMB& a, const int& b);
CORE_EXPORT
intMB operator/(const intMB& a, const int& b);

CORE_EXPORT
bool operator>(const intKB& a, const intKB& b);
CORE_EXPORT
bool operator<(const intKB& a, const intKB& b);
CORE_EXPORT
intKB operator-(const intKB& a, const intKB& b);
CORE_EXPORT
intKB operator+(const intKB& a, const intKB& b);
CORE_EXPORT
intKB operator*(const intKB& a, const int& b);
CORE_EXPORT
intKB operator/(const intKB& a, const int& b);

CORE_EXPORT
bool operator>(const longB& a, const longB& b);
CORE_EXPORT
bool operator<(const longB& a, const longB& b);
CORE_EXPORT
longB operator-(const longB& a, const longB& b);
CORE_EXPORT
longB operator+(const longB& a, const longB& b);
CORE_EXPORT
longB operator*(const longB& a, const qint64& b);
CORE_EXPORT
longB operator/(const longB& a, const qint64& b);

#endif // MEMORYSTRUCTS_H
