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

#include "dialogs/exportsvgdialog.h"
#include "mainwindow.h"

#include <QTimer>
#include <iostream>
#include <ostream>

void MainWindow::runRenderer(const bool &detach)
{
    Q_UNUSED(detach) // TODO

    if (!mIsRenderer) { return; }
    const auto args = QApplication::arguments();

    const auto inputList = args.filter(QRegularExpression("^--input="));
    const auto outputList = args.filter(QRegularExpression("^--output="));

    const QString inputFilePath = inputList.count() > 0 ? inputList.first().split("--input=").takeLast().simplified() : "";
    const QString outputFilePath = outputList.count() > 0 ? outputList.first().split("--output=").takeLast().simplified() : "";

    const bool hasInput = QFile::exists(inputFilePath);
    const bool hasIO = hasInput && !outputFilePath.isEmpty();
    const bool hasSVG = args.contains("--svg");

    if (!hasInput) {
        std::cout << tr("No input file!").toStdString() << std::endl;
        QTimer::singleShot(10, this, &MainWindow::close);
        return;
    }

    try {
        loadEVFile(inputFilePath);
        mDocument.setPath(inputFilePath);
    } catch (const std::exception& e) { gPrintExceptionCritical(e); }
    mDocument.actionFinished();
    printProjectInfo();

    if (!hasIO) {
        std::cout << tr("No output file!").toStdString() << std::endl;
        QTimer::singleShot(10, this, &MainWindow::close);
        return;
    }

    if (!hasSVG) {
        std::cout << tr("Only SVG output is currently supported!").toStdString() << std::endl;
        QTimer::singleShot(10, this, &MainWindow::close);
        return;
    }
    const auto warnings = checkBeforeExportSVG();
    if (!warnings.isEmpty()) {
        std::cout << warnings.toStdString() << std::endl;
    }

    const auto dialog = new ExportSvgDialog(this, QString());
    connect(dialog, &ExportSvgDialog::exportDone, this, &MainWindow::close);
    dialog->exportSVG(outputFilePath);
}

void MainWindow::printProjectInfo()
{
    std::cout << tr("Project: %1").arg(mDocument.fEvFile).toStdString() << std::endl;
    std::cout << tr("Scenes: %1").arg(mDocument.fScenes.count()).toStdString() << std::endl;
    for (const auto &scene : mDocument.fScenes) {
        std::cout << "---" << std::endl;
        std::cout << tr("Scene: %1").arg(scene->prp_getName()).toStdString() << std::endl;
        std::cout << tr("Canvas: %1x%2").arg(QString::number(scene->getCanvasWidth()),
                                             QString::number(scene->getCanvasHeight())).toStdString() << std::endl;
        std::cout << tr("Fps: %1").arg(QString::number(scene->getFps())).toStdString() << std::endl;
        std::cout << tr("Frame Range: %1-%2").arg(QString::number(scene->getFrameRange().fMin),
                                                  QString::number(scene->getFrameRange().fMax)).toStdString() << std::endl;
    }
    std::cout << "---" << std::endl;
}
