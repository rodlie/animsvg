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

#ifndef ANIMATIONCACHEHANDLER_H
#define ANIMATIONCACHEHANDLER_H

#include "smartPointers/selfref.h"

#include "skia/skiahelpers.h"
#include "framerange.h"

class eTask;
class eTaskBase;
class ImageCacheContainer;
class SvgExporter;
class QDomElement;

class CORE_EXPORT AnimationFrameHandler : public SelfRef {
protected:
    AnimationFrameHandler();
public:
    virtual ImageCacheContainer* getFrameAtFrame(const int relFrame) = 0;
    virtual ImageCacheContainer* getFrameAtOrBeforeFrame(const int relFrame) = 0;
    virtual eTask* scheduleFrameLoad(const int frame) = 0;
    virtual int getFrameCount() const = 0;
    virtual void reload() = 0;

    eTaskBase* saveAnimationSVG(SvgExporter& exp, QDomElement& parent,
                                const FrameRange& relRange,
                                const FrameRange& visRelRange);
};

#endif // ANIMATIONCACHEHANDLER_H
