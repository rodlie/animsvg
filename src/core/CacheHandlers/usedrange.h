#ifndef USEDRANGE_H
#define USEDRANGE_H
#include "hddcachablerangecont.h"
class HddCachableCacheHandler;

class CORE_EXPORT UsedRange {
    friend class HddCachableCacheHandler;
    UsedRange(HddCachableCacheHandler * const handler);
public:
    typedef HddCachableRangeCont Cont;

    void addIfInRange(Cont* const cont);
    void removeIfInRange(Cont* const cont);

    void replaceRange(const iValueRange& range);
    void setRangeMin(const int min);
    void setRangeMax(const int max);
    void incRangeMax(const int inc = 1);
    void incRangeMin(const int inc = 1);
    void clearRange();

    const iValueRange& range() const { return mRange; }
    bool validRange() const { return mRange.isValid(); }
private:
    HddCachableCacheHandler* const mHandler;
    iValueRange mRange;
};

#endif // USEDRANGE_H
