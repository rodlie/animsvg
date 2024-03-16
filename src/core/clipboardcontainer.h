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

#ifndef CLIPBOARDCONTAINER_H
#define CLIPBOARDCONTAINER_H
#include <QList>
#include <QWeakPointer>
#include <typeindex>
#include "smartPointers/ememory.h"
#include "ReadWrite/basicreadwrite.h"
#include "Animators/SmartPath/smartpath.h"
class QrealAnimator;
class BoundingBox;
class ContainerBox;
class Key;
class Animator;
class Property;

typedef QPair<qptr<Animator>, QByteArray> AnimatorKeyDataPair;

enum class ClipboardType : short {
    misc,
    boxes,
    keys,
    property,
    box,
    dynamicProperties,
    smartPath
};

class CORE_EXPORT Clipboard : public StdSelfRef {
public:
    Clipboard(const ClipboardType type);
    ClipboardType getType() const;

    using Writer = std::function<void(eWriteStream&)>;
    using Reader = std::function<void(eReadStream&)>;

    void write(const Writer& writer);
    void read(const Reader& reader);
private:
    QByteArray mData;
    const ClipboardType mType;
};

class CORE_EXPORT SmartPathClipboard : public Clipboard {
public:
    SmartPathClipboard(const SmartPath& path) :
        Clipboard(ClipboardType::smartPath), mPath(path) {}

    const SmartPath& path() const { return mPath; }
private:
    SmartPath mPath;
};

class CORE_EXPORT BoxesClipboard : public Clipboard {
    e_OBJECT
protected:
    BoxesClipboard(const QList<BoundingBox*> &src);
public:
    void pasteTo(ContainerBox * const parent);
};

class CORE_EXPORT KeysClipboard : public Clipboard {
    e_OBJECT
protected:
    KeysClipboard();
public:
    void paste(const int pasteFrame, const bool merge,
               const std::function<void(Key*)> &selectAction = nullptr);

    void addTargetAnimator(Animator *anim, const QByteArray& keyData);
private:
    QList<AnimatorKeyDataPair> mAnimatorData;
};

class CORE_EXPORT BoxClipboard : public Clipboard {
    e_OBJECT
protected:
    BoxClipboard(const BoundingBox * const source);
public:
    bool paste(BoundingBox * const target);

    template<typename T>
    bool hasType() {
        return mContentType == std::type_index(typeid(T));
    }

    template<typename T>
    bool compatibleTarget(T* const obj) const {
        return mContentType == std::type_index(typeid(*obj));
    }

    template<typename T>
    static bool sCopyAndPaste(const T* const from, T* const to) {
        return BoxClipboard(from).paste(to);
    }

    template<typename T>
    static bool sCopyAndPaste(const qsptr<const T>& from,
                              const qsptr<T>& to) {
        return BoxClipboard(from.get()).paste(to.get());
    }
private:
    const std::type_index mContentType;
};

class CORE_EXPORT PropertyClipboard : public Clipboard {
    e_OBJECT
protected:
    PropertyClipboard(const Property * const source);
public:
    bool paste(Property * const target);

    template<typename T>
    bool hasType() {
        return mContentType == std::type_index(typeid(T));
    }

    template<typename T>
    bool compatibleTarget(T* const obj) const {
        return mContentType == std::type_index(typeid(*obj));
    }

    template<typename T>
    static bool sCopyAndPaste(const T* const from, T* const to) {
        return PropertyClipboard(from).paste(to);
    }

    template<typename T>
    static bool sCopyAndPaste(const qsptr<T>& from, const qsptr<T>& to) {
        return PropertyClipboard(from.get()).paste(to.get());
    }
private:
    const std::type_index mContentType;
};

template <typename T>
class DynamicComplexAnimatorBase;

class CORE_EXPORT DynamicPropsClipboard : public Clipboard {
    e_OBJECT
protected:
    template<typename T>
    DynamicPropsClipboard(const QList<T*>& source) :
        Clipboard(ClipboardType::dynamicProperties),
        mContentBaseType(std::type_index(typeid(T))) {
        const auto writer = [&source](eWriteStream& writeStream) {
            writeStream << source.count();
            for(const auto& src : source)
                src->prp_writeProperty_impl(writeStream);
        };
        write(writer);
    }
public:
    template<typename T>
    bool paste(DynamicComplexAnimatorBase<T> * const target) {
        if(!compatibleTarget(target)) return false;
        const auto reader = [target](eReadStream& readStream) {
            target->prp_readProperty_impl(readStream);
        };
        read(reader);
        return true;
    }

    template<typename T>
    bool compatibleTarget(DynamicComplexAnimatorBase<T> * const obj) {
        Q_UNUSED(obj)
        return mContentBaseType == std::type_index(typeid(T));
    }
private:
    const std::type_index mContentBaseType;
};

#endif // CLIPBOARDCONTAINER_H
