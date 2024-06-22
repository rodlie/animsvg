#ifndef COLORANIMATORCREATOR_H
#define COLORANIMATORCREATOR_H


#include "Animators/coloranimator.h"
#include "shaderpropertycreator.h"

class CORE_EXPORT ColorAnimatorCreator : public ShaderPropertyCreator {
    e_OBJECT
    ColorAnimatorCreator(const QColor iniVal,
                         const bool glValue,
                         const QString& name,
                         const QString& nameUI) :
        ShaderPropertyCreator(glValue, name, nameUI),
        fIniVal(iniVal) {}

    const QColor fIniVal;

    qsptr<Property> create() const {
        const auto result = enve::make_shared<ColorAnimator>(fNameUI);
        result->setColor(fIniVal);
        return result;
    }
};

#endif // COLORANIMATORCREATOR_H
