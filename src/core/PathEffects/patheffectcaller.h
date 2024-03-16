#ifndef PATHEFFECTCALLER_H
#define PATHEFFECTCALLER_H

#include "../skia/skiaincludes.h"
#include "../smartPointers/stdselfref.h"

class CORE_EXPORT PathEffectCaller : public StdSelfRef {
public:
    PathEffectCaller();

    virtual void apply(SkPath& path) = 0;
};

#endif // PATHEFFECTCALLER_H
