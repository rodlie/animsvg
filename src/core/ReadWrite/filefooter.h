#ifndef FILEFOOTER_H
#define FILEFOOTER_H
#include <QIODevice>
#include <cstring>

#include "exceptions.h"

class eWriteStream;

class CORE_EXPORT FileFooter {
public:
    static void sWrite(eWriteStream& dst);

    static qint64 sSize(const int evVersion);

    static int sReadEvFileVersion(QIODevice * const src);
private:
    static char sEVFormat[15];
    static char sAppName[15];
    static char sAppVersion[15];
};

#endif // FILEFOOTER_H
