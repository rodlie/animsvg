#ifndef QREALSNAPSHOT_H
#define QREALSNAPSHOT_H
class QrealKey;
#include "pointhelpers.h"
#include "framerange.h"

class QrealSnapshot {
    friend class Iterator;
    struct KeySnaphot {
        qreal fC0Frame;
        qreal fC0Value;

        qreal fFrame;
        qreal fValue;

        qreal fC1Frame;
        qreal fC1Value;
    };
public:
    class Iterator {
    public:
        Iterator(const qreal& startFrame, const qreal& sampleStep,
                 const QrealSnapshot * const snap);

        qreal getValueAndProgress(const qreal& progress);

        bool staticValue() const;
    private:
        void updateSamples();

        bool mInterpolate;
        bool mStaticValue;
        const qreal mSampleFrameStep;
        const qreal mInvFrameSpan;
        qreal mPrevFrame;
        qreal mNextFrame;
        qreal mPrevValue;
        qreal mNextValue;
        qreal mCurrentFrame;
        const QrealSnapshot * const mSnapshot;
    };

    QrealSnapshot() {}
    QrealSnapshot(const qreal& currentValue) :
        QrealSnapshot(currentValue, 1, 1) {}
    QrealSnapshot(const qreal& currentValue,
             const qreal& frameMultiplier,
             const qreal& valueMultiplier) :
        mCurrentValue(currentValue*valueMultiplier),
        mFrameMultiplier(frameMultiplier),
        mValueMultiplier(valueMultiplier) {}

    void appendKey(const QrealKey * const key);

    qreal getValue(const qreal& relFrame) const;
protected:
    void getPrevAndNextKey(const qreal& relFrame,
                           KeySnaphot const *& prevKey,
                           KeySnaphot const *& nextKey) const;
    void getPrevAndNextKey(const qreal& relFrame,
                           KeySnaphot const *& prevKey,
                           KeySnaphot const *& nextKey,
                           const int& minId, const int& maxId) const;
    void getPrevAndNextKeyId(const qreal& relFrame,
                             int & prevKey, int & nextKey) const;
    void getPrevAndNextKeyId(const qreal& relFrame,
                             int & prevKey, int & nextKey,
                             const int& minId, const int& maxId) const;
    qreal mCurrentValue;
    QList<KeySnaphot> mKeys;

    qreal mFrameMultiplier;
    qreal mValueMultiplier;
};
#endif // QREALSNAPSHOT_H
