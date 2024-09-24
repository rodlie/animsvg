#ifndef NAMEDPROPERTY_H
#define NAMEDPROPERTY_H

#include "property.h"
#include "typemenu.h"
#include "GUI/propertynamedialog.h"

template <typename T>
class NamedProperty : public T {
    e_OBJECT
protected:
    using T::T;
public:
    void prp_readProperty_impl(eReadStream& src) override {
        T::prp_readProperty_impl(src);
        QString name; src >> name;
        this->prp_setName(name);
    }
    void prp_writeProperty_impl(eWriteStream& dst) const override {
        T::prp_writeProperty_impl(dst);
        dst << this->prp_getName();
    }

    void prp_setupTreeViewMenu(PropertyMenu * const menu) override {
        if(menu->hasActionsForType<NamedProperty<T>>()) return;
        menu->addedActionsForType<NamedProperty<T>>();
        {
            const auto parentWidget = menu->getParentWidget();
            menu->addPlainAction(QIcon::fromTheme("dialog-informaton"), QObject::tr("Rename"), [this, parentWidget]() {
                PropertyNameDialog::sRenameProperty(this, parentWidget);
            });
            menu->addSeparator();
            const PropertyMenu::PlainSelectedOp<NamedProperty<T>> dOp =
            [](NamedProperty<T>* const prop) {
                const auto parent = prop->template getParent<
                        DynamicComplexAnimatorBase<Animator>>();
                parent->removeChild(prop->template ref<NamedProperty<T>>());
            };
            menu->addPlainAction(QIcon::fromTheme("trash"), QObject::tr("Delete Property"), dOp);
        }

        menu->addSeparator();
        T::prp_setupTreeViewMenu(menu);
    }
};

#endif // NAMEDPROPERTY_H
