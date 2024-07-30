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

#include "mainwindow.h"

#include "RasterEffects/rastereffectmenucreator.h"
#include "BlendEffects/blendeffectmenucreator.h"
#include "TransformEffects/transformeffectmenucreator.h"
#include "PathEffects/patheffectmenucreator.h"

void MainWindow::setupMenuEffects()
{
    { // path
        const auto menu1 = mEffectsMenu->addMenu(QIcon::fromTheme("preferences"), tr("Path Effects"));
        const auto menu2 = mEffectsMenu->addMenu(QIcon::fromTheme("preferences"), tr("Fill Effects"));
        const auto menu3 = mEffectsMenu->addMenu(QIcon::fromTheme("preferences"), tr("Outline Base Effects"));
        const auto menu4 = mEffectsMenu->addMenu(QIcon::fromTheme("preferences"), tr("Outline Effects"));
        const auto adder = [this,
                            menu1,
                            menu2,
                            menu3,
                            menu4](const QString& name,
                                   const PathEffectMenuCreator::EffectCreator& creator) {
            if (name.isEmpty()) { return; }
            {
                const auto act =  menu1->addAction(QIcon::fromTheme("preferences"), name);
                act->setData(QString(name).prepend(tr("Add ")).append(tr(" (Path Effect)")));
                cmdAddAction(act);
                connect(act, &QAction::triggered, this, [this, creator]() {
                    addPathEffect(creator());
                });
            }
            {
                const auto act =  menu2->addAction(QIcon::fromTheme("preferences"), name);
                act->setData(QString(name).prepend(tr("Add ")).append(tr(" (Fill Effect)")));
                cmdAddAction(act);
                connect(act, &QAction::triggered, this, [this, creator]() {
                    addFillPathEffect(creator());
                });
            }
            {
                const auto act =  menu3->addAction(QIcon::fromTheme("preferences"), name);
                act->setData(QString(name).prepend(tr("Add ")).append(tr(" (Outline Base Effect)")));
                cmdAddAction(act);
                connect(act, &QAction::triggered, this, [this, creator]() {
                    addOutlineBasePathEffect(creator());
                });
            }
            {
                const auto act =  menu4->addAction(QIcon::fromTheme("preferences"), name);
                act->setData(QString(name).prepend(tr("Add ")).append(tr(" (Outline Effect)")));
                cmdAddAction(act);
                connect(act, &QAction::triggered, this, [this, creator]() {
                    addOutlinePathEffect(creator());
                });
            }
        };
        PathEffectMenuCreator::forEveryEffect(adder);
    }
    { // transform
        const auto menu = mEffectsMenu->addMenu(QIcon::fromTheme("preferences"), tr("Transform Effects"));
        const auto adder = [this, menu](const QString& name,
                                        const TransformEffectMenuCreator::EffectCreator& creator) {
            if (name.isEmpty()) { return; }
            const auto act =  menu->addAction(QIcon::fromTheme("preferences"), name);
            act->setData(QString(name).prepend(tr("Add ")).append(tr(" (Transform Effect)")));
            cmdAddAction(act);
            connect(act, &QAction::triggered, this, [this, creator]() {
                addTransformEffect(creator());
            });
        };
        TransformEffectMenuCreator::forEveryEffect(adder);
    }
    { // blend
        const auto menu = mEffectsMenu->addMenu(QIcon::fromTheme("preferences"), tr("Blend Effects"));
        const auto adder = [this, menu](const QString& name,
                                        const BlendEffectMenuCreator::EffectCreator& creator) {
            if (name.isEmpty()) { return; }
            const auto act =  menu->addAction(QIcon::fromTheme("preferences"), name);
            act->setData(QString(name).prepend(tr("Add ")).append(tr(" (Blend Effect)")));
            cmdAddAction(act);
            connect(act, &QAction::triggered, this, [this, creator]() {
                addBlendEffect(creator());
            });
        };
        BlendEffectMenuCreator::forEveryEffect(adder);
    }
    { // raster
        const auto menu = mEffectsMenu->addMenu(QIcon::fromTheme("preferences"), tr("Raster Effects"));
        const auto adder = [this, menu](const QString& name, const QString& path,
                                        const RasterEffectMenuCreator::EffectCreator& creator) {
            if (name.isEmpty()) { return; }
            QString title = name;
            if (!path.isEmpty()) { title.append(QString(" (%1)").arg(path));}
            const auto act =  menu->addAction(QIcon::fromTheme("preferences"), title);
            act->setData(QString(name).prepend(tr("Add ")).append(tr(" (Raster Effect)")));
            cmdAddAction(act);
            connect(act, &QAction::triggered, this, [this, creator]() {
                addRasterEffect(creator());
            });
        };
        RasterEffectMenuCreator::forEveryEffectCore(adder);
        menu->addSeparator();
        RasterEffectMenuCreator::forEveryEffectCustom(adder);
        RasterEffectMenuCreator::forEveryEffectShader(adder);
        //RasterEffectMenuCreator::forEveryEffect(adder);
    }
}

void MainWindow::addRasterEffect(const qsptr<RasterEffect> &effect)
{
    const auto box = getCurrentBox();
    if (!box) { return; }

    box->addRasterEffect(effect);
    mDocument.actionFinished();
}

void MainWindow::addBlendEffect(const qsptr<BlendEffect> &effect)
{
    const auto box = getCurrentBox();
    if (!box) { return; }

    box->addBlendEffect(effect);
    mDocument.actionFinished();
}

void MainWindow::addTransformEffect(const qsptr<TransformEffect> &effect)
{
    const auto box = getCurrentBox();
    if (!box) { return; }

    box->addTransformEffect(effect);
    mDocument.actionFinished();
}

void MainWindow::addPathEffect(const qsptr<PathEffect> &effect)
{
    const auto box = getCurrentBox();
    if (!box) { return; }

    box->addPathEffect(effect);
    mDocument.actionFinished();
}

void MainWindow::addFillPathEffect(const qsptr<PathEffect> &effect)
{
    const auto box = getCurrentBox();
    if (!box) { return; }

    box->addFillPathEffect(effect);
    mDocument.actionFinished();
}

void MainWindow::addOutlineBasePathEffect(const qsptr<PathEffect> &effect)
{
    const auto box = getCurrentBox();
    if (!box) { return; }

    box->addOutlineBasePathEffect(effect);
    mDocument.actionFinished();
}

void MainWindow::addOutlinePathEffect(const qsptr<PathEffect> &effect)
{
    const auto box = getCurrentBox();
    if (!box) { return; }

    box->addOutlinePathEffect(effect);
    mDocument.actionFinished();
}
