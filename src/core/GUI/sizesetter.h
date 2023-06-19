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

#ifndef SIZESETTER_H
#define SIZESETTER_H

#include <QObject>

#include <QBoxLayout>
#include <functional>

#include "../core_global.h"

class CORE_EXPORT SizeSetter : public QObject {
    Q_OBJECT
public:
    SizeSetter();

    using SetSizeFunc = std::function<void(int)>;
    using Evaluator = std::function<int()>;

    void add(QObject* const obj, const SetSizeFunc& setter);
    void add(const SetSizeFunc& setter);

    void addSpacing(QVBoxLayout* const layout);
    void addSpacing(QHBoxLayout* const layout);

    void addHalfSpacing(QVBoxLayout* const layout);
    void addHalfSpacing(QHBoxLayout* const layout);

    void addSpacing(QVBoxLayout* const layout,
                    const Evaluator& size);
    void addSpacing(QHBoxLayout* const layout,
                    const Evaluator& size);
    void set(const int size);

    int size() const { return mSize; }

    operator int() const { return mSize; }

    void updateSize();

    void setEvaluator(const Evaluator& evaluator);
signals:
    void sizeChanged(const int size, QPrivateSignal);
private:
    int mSize = 0;
    Evaluator mEvaluator;
};

#endif // SIZESETTER_H
