#include "filefooter.h"

#include "evformat.h"
#include "ewritestream.h"

char FileFooter::sEVFormat[15] = "enve ev";
char FileFooter::sAppName[15] = "enve";
char FileFooter::sAppVersion[15] = PROJECT_VERSION;

void FileFooter::sWrite(eWriteStream& dst) {
    dst << EvFormat::version;
    dst.write(&sEVFormat[0], sizeof(char[15]));
    dst.write(&sAppName[0], sizeof(char[15]));
    dst.write(&sAppVersion[0], sizeof(char[15]));
}

qint64 FileFooter::sSize(const int evVersion) {
    if(evVersion > 1) return static_cast<qint64>(3*sizeof(char[15])) + sizeof(int);
    else return static_cast<qint64>(3*sizeof(char[15]));
}

int FileFooter::sReadEvFileVersion(QIODevice * const src) {
    const qint64 savedPos = src->pos();
    const qint64 pos = src->size() - static_cast<qint64>(3*sizeof(char[15]));
    if(!src->seek(pos))
        RuntimeThrow("Failed to seek to FileFooter");

    char format[15];
    src->read(format, sizeof(char[15]));
    if(std::strcmp(format, sEVFormat)) return 0;

    char appName[15];
    src->read(appName, sizeof(char[15]));

    char appVersion[15];
    src->read(appVersion, sizeof(char[15]));
    int evFormatVersion;
    const bool is000 = std::strncmp(&appVersion[0], "0.0.0", 5) == 0;
    const bool is000c = is000 && std::strncmp(&appVersion[0], "0.0.0c", 6) == 0;

    if(is000 && !is000c) {
        evFormatVersion = 1;
    } else {
        if(!src->seek(pos - static_cast<qint64>(sizeof(int))))
            RuntimeThrow("Failed to seek to ev format version");
        src->read(reinterpret_cast<char*>(&evFormatVersion),
                  sizeof(int));
    }

    if(!src->seek(savedPos))
        RuntimeThrow("Could not restore current position for QIODevice.");
    return evFormatVersion;
}
