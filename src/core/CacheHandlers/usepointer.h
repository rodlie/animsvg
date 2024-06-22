#ifndef USEPOINTER_H
#define USEPOINTER_H

#include "smartPointers/stdpointer.h"
#include "cachecontainer.h"

class CORE_EXPORT UsePointerBase {
protected:
    UsePointerBase() {}

    void decInUse(CacheContainer* ptr) {
        if(ptr) ptr->decInUse();
    }

    void incInUse(CacheContainer* ptr) {
        if(ptr) ptr->incInUse();
    }
};

template <template<class> class Ptr, class T = CacheContainer>
class CORE_EXPORT UsePointerT : public UsePointerBase {
public:
    UsePointerT() {}

    ~UsePointerT() { setPtr(nullptr); }

    UsePointerT(T* const ptr) {
        setPtr(ptr);
    }

    UsePointerT(const Ptr<T>& ptr) {
        setPtr(ptr);
    }

    UsePointerT(const UsePointerT& other) {
        setPtr(other.mPtr);
    }

    T* get() const { return mPtr.get(); }

    void swap(UsePointerT& other) {
        const auto t = mPtr;
        setPtr(other.mPtr);
        other.setPtr(t);
    }

    void reset() { setPtr(nullptr); }

    UsePointerT &operator=(const UsePointerT& other) {
        setPtr(other.mPtr);
        return *this;
    }

    UsePointerT &operator=(const Ptr<T>& t) {
        setPtr(t);
        return *this;
    }

    UsePointerT &operator=(T* const t) {
        setPtr(t);
        return *this;
    }

    T* operator->() const { return get(); }
    operator bool() const { return get(); }
    bool operator==(T* const t) { return get() == t; }
    bool operator==(const Ptr<T>& t) { return mPtr == t; }
private:
    void setPtr(const Ptr<T>& t) {
        decInUse(get());
        mPtr = t;
        incInUse(get());
    }

    Ptr<T> mPtr;
};

template <class T = CacheContainer>
using UsePointer = UsePointerT<stdptr, T>;

template <class T = CacheContainer>
using UseSharedPointer = UsePointerT<stdsptr, T>;

#endif // USEPOINTER_H
