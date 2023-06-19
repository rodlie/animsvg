/*
#
# Friction - https://friction.graphics
#
# Copyright (c) Friction contributors
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

#include "sizesetter.h"

SizeSetter::SizeSetter() {}

void SizeSetter::add(QObject* const obj, const SetSizeFunc& setter) {
    setter(mSize);
    connect(this, &SizeSetter::sizeChanged, obj, setter);
}

void SizeSetter::add(const SetSizeFunc& setter) {
    setter(mSize);
    connect(this, &SizeSetter::sizeChanged, this, setter);
}

void SizeSetter::addSpacing(QVBoxLayout* const layout) {
    addSpacing(layout, [this] { return size(); });
}

void SizeSetter::addSpacing(QHBoxLayout* const layout) {
    addSpacing(layout, [this] { return size(); });
}

void SizeSetter::addHalfSpacing(QVBoxLayout* const layout) {
    addSpacing(layout, [this] { return size()/2; });
}

void SizeSetter::addHalfSpacing(QHBoxLayout* const layout) {
    addSpacing(layout, [this] { return size()/2; });
}

void SizeSetter::addSpacing(QVBoxLayout* const layout,
                            const Evaluator& size) {
    const auto spacer = new QSpacerItem(0, size(),
                                        QSizePolicy::Minimum,
                                        QSizePolicy::Fixed);
    layout->addSpacerItem(spacer);
    connect(this, &SizeSetter::sizeChanged,
            layout, [layout, spacer](const int size) {
        spacer->changeSize(0, size,
                           QSizePolicy::Minimum,
                           QSizePolicy::Fixed);
        layout->invalidate();
    });
}

void SizeSetter::addSpacing(QHBoxLayout* const layout,
                            const Evaluator& size) {
    const auto spacer = new QSpacerItem(size(), 0,
                                        QSizePolicy::Fixed,
                                        QSizePolicy::Minimum);
    layout->addSpacerItem(spacer);
    connect(this, &SizeSetter::sizeChanged,
            layout, [layout, spacer](const int size) {
        spacer->changeSize(size, 0,
                           QSizePolicy::Fixed,
                           QSizePolicy::Minimum);
        layout->invalidate();
    });
}

void SizeSetter::set(const int size) {
    if(size == mSize) return;
    mSize = size;
    emit sizeChanged(size, QPrivateSignal());
}

void SizeSetter::updateSize() {
    if(mEvaluator) set(mEvaluator());
}

void SizeSetter::setEvaluator(const Evaluator& evaluator) {
    mEvaluator = evaluator;
    updateSize();
}
