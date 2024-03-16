#include "memorystructs.h"

intMB::intMB(const longB &b) :
    intMB(intKB(b)) {}

intMB::intMB(const intKB &kb) :
    fValue(kb.fValue/1024) {}

intKB::intKB(const longB &b) :
    fValue(static_cast<int>(b.fValue/1024)) {}

intKB::intKB(const intMB &mb) :
    fValue(mb.fValue*1024) {}

longB::longB(const intKB &kb) :
    fValue(static_cast<qint64>(kb.fValue)*1024) {}

longB::longB(const intMB &mb) :
    longB(intKB(mb)) {}


bool operator>(const intMB &a, const intMB &b) {
    return a.fValue > b.fValue;
}

bool operator<(const intMB& a, const intMB& b) {
    return a.fValue < b.fValue;
}

intMB operator-(const intMB& a, const intMB& b) {
    return intMB(a.fValue - b.fValue);
}

intMB operator+(const intMB& a, const intMB& b) {
    return intMB(a.fValue + b.fValue);
}

intMB operator*(const intMB& a, const int& b) {
    return intMB(a.fValue*b);
}

intMB operator/(const intMB& a, const int& b) {
    return intMB(a.fValue/b);
}

bool operator>(const intKB &a, const intKB &b) {
    return a.fValue > b.fValue;
}

bool operator<(const intKB& a, const intKB& b) {
    return a.fValue < b.fValue;
}

intKB operator-(const intKB& a, const intKB& b) {
    return intKB(a.fValue - b.fValue);
}

intKB operator+(const intKB& a, const intKB& b) {
    return intKB(a.fValue + b.fValue);
}

intKB operator*(const intKB& a, const int& b) {
    return intKB(a.fValue*b);
}

intKB operator/(const intKB& a, const int& b) {
    return intKB(a.fValue/b);
}

bool operator>(const longB &a, const longB &b) {
    return a.fValue > b.fValue;
}

bool operator<(const longB& a, const longB& b) {
    return a.fValue < b.fValue;
}

longB operator-(const longB& a, const longB& b) {
    return longB(a.fValue - b.fValue);
}

longB operator+(const longB& a, const longB& b) {
    return longB(a.fValue + b.fValue);
}

longB operator*(const longB& a, const qint64& b) {
    return longB(a.fValue*b);
}

longB operator/(const longB& a, const qint64& b) {
    return longB(a.fValue/b);
}
