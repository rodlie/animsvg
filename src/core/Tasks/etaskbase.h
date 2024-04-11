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

#ifndef ETASKBASE_H
#define ETASKBASE_H

#include "../smartPointers/ememory.h"

enum class eTaskState {
    created,
    qued,
    processing,
    finished,
    canceled,
    waiting
};

class eTask;

class CORE_EXPORT eTaskBase {
public:
    virtual ~eTaskBase() { cancelDependent(); }
protected:
    virtual void afterProcessing() {}
    virtual void afterCanceled() {}
    virtual bool handleException() { return false; }
public:
    struct Dependent {
        using Func = std::function<void()>;

        Func fFinished;
        Func fCanceled;
    };

    void addDependent(const Dependent& func);
    void addDependent(eTask * const task);

    bool unhandledException() const;
    std::exception_ptr takeException();
    void setException(const std::exception_ptr& exception);

    void finishedProcessing();

    eTaskState getState() const { return mState; }
    bool finished() const { return mState == eTaskState::finished; }
    bool isQued() { return mState == eTaskState::qued; }

    bool isActive() { return mState != eTaskState::created &&
                             mState != eTaskState::finished; }

    bool readyToBeProcessed() { return mNDependancies == 0; }

    bool waitingToCancel() const { return mCancel; }
    void cancel();
protected:
    eTaskState mState = eTaskState::created;

    void moveDependent(eTaskBase* const to);
private:
    void decDependencies() { mNDependancies--; }
    void incDependencies() { mNDependancies++; }

    void tellDependentThatFinished();
    void cancelDependent();

    bool mCancel = false;
    int mNDependancies = 0;
    QList<Dependent> mDependentF;
    QList<stdptr<eTask>> mDependent;
    std::exception_ptr mUpdateException;
};

#endif // ETASKBASE_H
