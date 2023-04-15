/*
# enve2d - https://github.com/enve2d
#
# Copyright (c) enve2d developers
# Copyright (C) 2016-2020 Maurycy Liebner
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
*/

#include "svgexporter.h"
#include "canvas.h"

#include "appsupport.h"

SvgExporter::SvgExporter(const QString& path,
                         Canvas* const scene,
                         const FrameRange& frameRange,
                         const qreal fps,
                         const bool background,
                         const bool fixedSize,
                         const bool loop,
                         bool html)
    : ComplexTask(INT_MAX, tr("SVG Export"))
    , fScene(scene)
    , fAbsRange(frameRange)
    , fFps(fps)
    , fBackground(background)
    , fFixedSize(fixedSize)
    , fLoop(loop)
    , mHtml(html)
    , mOpen(false)
    , mFile(path)
    , mSvg(createElement("svg"))
    , mDefs(createElement("defs"))
{

}

void SvgExporter::nextStep()
{
    if (!mOpen) {
        if (mFile.open(QIODevice::WriteOnly)) {
            mStream.setDevice(&mFile);
            if (mHtml) {
                mStream << QString::fromUtf8("<!DOCTYPE html>") << QT_ENDL;
                mStream << QString::fromUtf8("<html>") << QT_ENDL;
                mStream << QString::fromUtf8("<head>") << QT_ENDL;
                mStream << QString::fromUtf8("<meta charset=\"utf-8\" />") << QT_ENDL;
                mStream << QString::fromUtf8("<title>Preview</title>") << QT_ENDL;
                mStream << QString::fromUtf8("<style>html { background: repeating-conic-gradient(#b0b0b0 0% 25%, transparent 0% 50%) 50% / 40px 40px; }</style>") << QT_ENDL;
                mStream << QString::fromUtf8("</head>") << QT_ENDL;
                mStream << QString::fromUtf8("<body>") << QT_ENDL;
            } else {
                mStream << QString::fromUtf8("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>") << QT_ENDL;
            }
            mStream << QString::fromUtf8("<!-- Created with enve2d https://enve2d.org -->") << QT_ENDL << QT_ENDL;
            fScene->saveSceneSVG(*this);
        } else {
            RuntimeThrow("Could not open:\n\"" + mFile.fileName() + "\"");
        }
        mOpen = true;
    }
    if (mWaitingTasks.isEmpty()) { return finish(); }
    const auto task = mWaitingTasks.takeFirst();
    addTask(task);
}

void SvgExporter::addNextTask(const stdsptr<eTask>& task)
{
    mWaitingTasks << task;
}

void SvgExporter::finish()
{
    if (mOpen) {
        mSvg.appendChild(mDefs);
        mDoc.appendChild(mSvg);
        mStream << mDoc.toString();
        if (mHtml) {
            mStream << QString::fromUtf8("</body>") << QT_ENDL;
            mStream << QString::fromUtf8("</html>") << QT_ENDL;
        }
        mStream.flush();
        mFile.close();
    }
    setValue(INT_MAX);
}
