#ifndef GRADIENTPOINT_H
#define GRADIENTPOINT_H
#include "movablepoint.h"
#include "fillstrokesettings.h"
class VectorPath;

class GradientPoint : public MovablePoint
{
public:
    GradientPoint(QPointF absPos, VectorPath *parent);
    GradientPoint(int idT, VectorPath *parent);
    void setRelativePos(QPointF relPos, bool saveUndoRedo);
    void setColor(QColor fillColor);
    void draw(QPainter *p);
private:
    QColor mFillColor = QColor(255, 0, 0, 155);
};

#endif // GRADIENTPOINT_H
