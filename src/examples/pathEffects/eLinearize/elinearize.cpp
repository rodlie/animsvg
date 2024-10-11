/*
#
# Friction - https://friction.graphics
#
# Copyright (c) Ole-André Rodlie and contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# See 'README.md' for more information.
#
*/

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "elinearize.h"
#include "enveCore/Animators/qrealanimator.h"

void eCreateNewestVersion(qsptr<CustomPathEffect> &result) {
    // Use default, most up to date, version
    result = enve::make_shared<eLinearize000>();
}

void eCreate(const CustomIdentifier &identifier,
             qsptr<CustomPathEffect> &result) {
    Q_UNUSED(identifier)
    // Choose version based on identifier
    // if(identifier.fVersion == CustomIdentifier::Version{0, 0, 0})
    result = enve::make_shared<eLinearize000>();
}

// Returned value must be unique, lets enve distinguish effects
QString effectId() {
    return "waer9yv11r3gl10x1qtm";
}

#define eLName QStringLiteral("eLinearize")

// Name of your effect used in UI
void eName(QString &result) {
    result = eLName;
}

// here specify your effect's most up to date version
CustomIdentifier::Version effectVersion() {
    return { 0, 0, 0 };
}

void eIdentifier(CustomIdentifier& result) {
    result = { effectId(), eLName, effectVersion() };
}

bool eSupports(const CustomIdentifier &identifier) {
    if(identifier.fEffectId != effectId()) return false;
    if(identifier.fEffectName != eLName) return false;
    return identifier.fVersion == effectVersion();
}

eLinearize000::eLinearize000() :
    CustomPathEffect(eLName.toLower()) {
    mInfluence = enve::make_shared<QrealAnimator>(0, 0, 1, 0.1, "influence");
    ca_addChild(mInfluence);
}

CustomIdentifier eLinearize000::getIdentifier() const {
    return { effectId(), eLName, { 0, 0, 0 } };
}

class eLinearize000EffectCaller : public PathEffectCaller {
public:
    eLinearize000EffectCaller(const qreal infl) :
        mInfl(toSkScalar(infl)) {}

    void apply(SkPath& path);
private:
    const float mInfl;
};

void eLinearize000EffectCaller::apply(SkPath &path) {
    const float invInf = 1 - mInfl;
    SkPath src;
    path.swap(src);
    path.setFillType(src.getFillType());

    SkPath::Iter iter(src, false);
    SkPoint pts[4];
    for(;;) {
        switch(iter.next(pts)) {
        case SkPath::kLine_Verb: {
            path.lineTo(pts[1]);
        } break;
        case SkPath::kQuad_Verb: {
            path.quadTo(pts[1]*invInf + (pts[0] + pts[2])*0.5f*mInfl, pts[2]);
        } break;
        case SkPath::kConic_Verb: {
            path.conicTo(pts[1]*invInf + (pts[0] + pts[2])*0.5f*mInfl,
                         pts[2], iter.conicWeight());
        } break;
        case SkPath::kCubic_Verb: {
            path.cubicTo(pts[0]*mInfl + pts[1]*invInf,
                         pts[3]*mInfl + pts[2]*invInf, pts[3]);
        } break;
        case SkPath::kClose_Verb: {
            path.close();
        } break;
        case SkPath::kMove_Verb: {
            path.moveTo(pts[0]);
        } break;
        case SkPath::kDone_Verb:
            return;
        }
    }
}

stdsptr<PathEffectCaller> eLinearize000::getEffectCaller(
        const qreal relFrame, const qreal influence) const {
    const qreal infl = mInfluence->getEffectiveValue(relFrame)*influence;
    return enve::make_shared<eLinearize000EffectCaller>(infl);
}
