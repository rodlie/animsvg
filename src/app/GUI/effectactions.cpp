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

void MainWindow::setupMenuEffects()
{
    { // path TODO
        // adapt PathEffectsMenu::addPathEffectsToBoxActionMenu(menu);
    }
    { // transform
        const auto menu = mEffectsMenu->addMenu(QIcon::fromTheme("preferences"), tr("Transform Effects"));
        const auto adder = [this, menu](const QString& name,
                                        const TransformEffectMenuCreator::EffectCreator& creator) {
            if (name.isEmpty()) { return; }
            const auto act =  menu->addAction(QIcon::fromTheme("preferences"), name);
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
