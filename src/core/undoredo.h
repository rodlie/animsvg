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

#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <QList>
#include "smartPointers/ememory.h"
#include "framerange.h"

class UndoRedo_priv;
class UndoRedoSet;

class CORE_EXPORT UndoRedoStack : public SelfRef {
    Q_OBJECT
public:
    class StackBlock {
    public:
        StackBlock(UndoRedoStack* stack = nullptr) : mStack(stack) {
            if(mStack) mStack->mUndoRedoBlocked++;
        }

        ~StackBlock() { reset(); }

        void reset() {
            if(mStack) {
                mStack->mUndoRedoBlocked--;
                mStack = nullptr;
            }
        }
    private:
        UndoRedoStack* mStack = nullptr;
    };

    UndoRedoStack(const std::function<bool(int)>& changeFrameFunc);

    void pushName(const QString& name);
    bool newCollection();

    void addUndoRedo(const QString &name,
                     const std::function<void()> &undo,
                     const std::function<void()> &redo);

    QString undoText() const;
    QString redoText() const;

    bool canUndo() const;
    bool canRedo() const;

    bool redo();
    bool undo();
    void emptySomeOfUndo();

    StackBlock blockUndoRedo();

    void setFrame(const int frame)
    { mCurrentAbsFrame = frame; }

    void clear();
signals:
    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);

    void undoTextChanged(const QString& undoText);
    void redoTextChanged(const QString& undoText);
private:
    void checkUndoRedoChanged();

    void checkUndoRedoTextChanged();
    void checkUndoTextChanged();
    void checkRedoTextChanged();

    void checkCanUndoRedoChanged();
    void checkCanUndoChanged();
    void checkCanRedoChanged();

    void addToSet(const stdsptr<UndoRedo_priv> &undoRedo);

    int mCurrentAbsFrame = 0;
    const std::function<bool(int)> mChangeFrameFunc;

    int mUndoRedoBlocked = 0;
    int mLastUndoRedoFrame = FrameRange::EMAX;

    bool mCanUndo = false;
    bool mCanRedo = false;

    QString mUndoText;
    QString mRedoText;

    QString mCurrentSetName;
    stdsptr<UndoRedoSet> mCurrentSet;
    QList<stdsptr<UndoRedo_priv>> mUndoStack;
    QList<stdsptr<UndoRedo_priv>> mRedoStack;
};

#endif // UNDOREDO_H
