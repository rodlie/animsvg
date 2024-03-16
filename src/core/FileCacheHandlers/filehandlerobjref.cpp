#include "filehandlerobjref.h"

void FileHandlerObjRefBase::increment(FileCacheHandler * const hadler) const {
    hadler->mReferenceCount++;
}

void FileHandlerObjRefBase::decrement(FileCacheHandler * const hadler) const {
    hadler->mReferenceCount--;
}
