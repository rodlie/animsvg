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

#ifndef EFFECTSLOADER_H
#define EFFECTSLOADER_H

#include "Private/Tasks/offscreenqgl33c.h"

struct ShaderEffectProgram;
struct ShaderEffectCreator;

class EffectsLoader : public QObject, protected OffscreenQGL33c
{
    Q_OBJECT

public:
    EffectsLoader();
    ~EffectsLoader();

    void initializeGpu();
    void iniCustomPathEffects();
    void iniCustomRasterEffects();
    void iniShaderEffects();
    void iniCustomBoxes();

signals:
    void programChanged(ShaderEffectProgram*);

private:
    void reloadProgram(ShaderEffectCreator * const loaded,
                       const QString &fragPath);
    void iniSingleRasterEffectProgram(const QString &grePath);
    void iniShaderEffectProgramExec(const QString &grePath);

    void iniCustomRasterEffect(const QString &soPath);
    void iniIfCustomRasterEffect(const QString &path);

    QStringList mLoadedGREPaths;
    GLuint mPlainSquareVAO;
    GLuint mTexturedSquareVAO;
};

#endif // EFFECTSLOADER_H
