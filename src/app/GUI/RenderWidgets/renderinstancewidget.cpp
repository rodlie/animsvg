// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "renderinstancewidget.h"
#include "GUI/global.h"
#include <QMenu>
#include "outputsettingsdialog.h"
#include "outputsettingsprofilesdialog.h"
#include "outputsettingsdisplaywidget.h"
#include "rendersettingsdisplaywidget.h"
#include "Private/esettings.h"
#include "GUI/edialogs.h"

#include "appsupport.h"

RenderInstanceWidget::RenderInstanceWidget(
        Canvas *canvas, QWidget *parent) :
    ClosableContainer(parent), mSettings(canvas) {
    iniGUI();
    connect(&mSettings, &RenderInstanceSettings::stateChanged,
            this, &RenderInstanceWidget::updateFromSettings);
    updateFromSettings();
}

RenderInstanceWidget::RenderInstanceWidget(const RenderInstanceSettings& sett,
                                           QWidget *parent) :
    ClosableContainer(parent), mSettings(sett) {
    iniGUI();
    connect(&mSettings, &RenderInstanceSettings::stateChanged,
            this, &RenderInstanceWidget::updateFromSettings);
    updateFromSettings();
}

void RenderInstanceWidget::iniGUI() {
    if(!OutputSettingsProfile::sOutputProfilesLoaded) {
        OutputSettingsProfile::sOutputProfilesLoaded = true;
        const QString dirPath = AppSupport::getAppOutputProfilesPath();
        QDirIterator dirIt(dirPath, QDirIterator::NoIteratorFlags);
        while(dirIt.hasNext()) {
            const auto path = dirIt.next();
            const QFileInfo fileInfo(path);
            if(!fileInfo.isFile()) continue;
            if(!fileInfo.completeSuffix().contains("eProf")) continue;
            const auto profile = enve::make_shared<OutputSettingsProfile>();
            try {
                profile->load(path);
            } catch(const std::exception& e) {
                gPrintExceptionCritical(e);
            }
            OutputSettingsProfile::sOutputProfiles << profile;
        }
    }

    setCheckable(true);
    setObjectName("darkWidget");
    mNameLabel = new QLabel(this);
    mNameLabel->setTextFormat(Qt::RichText);
    mNameLabel->setFixedHeight(eSizesUI::widget);
    mNameLabel->setObjectName("darkWidget");

    setLabelWidget(mNameLabel);

    QWidget *contWid = new QWidget(this);
    contWid->setLayout(mContentLayout);
    mContentLayout->setAlignment(Qt::AlignTop);
    contWid->setObjectName("darkWidget");

    addContentWidget(contWid);

    const auto renderSettings = new ClosableContainer();
    mRenderSettingsDisplayWidget = new RenderSettingsDisplayWidget(this);
    renderSettings->addContentWidget(mRenderSettingsDisplayWidget);

    QWidget *renderSettingsLabelWidget = new QWidget();
    renderSettingsLabelWidget->setObjectName("darkWidget");
    QHBoxLayout *renderSettingsLayout = new QHBoxLayout();
    renderSettingsLayout->setSpacing(0);
    renderSettingsLayout->setMargin(0);
    renderSettingsLayout->setAlignment(Qt::AlignLeft);

    eSizesUI::widget.addSpacing(renderSettingsLayout);
    QLabel *renderSettingsLabel = new QLabel("Render Settings:", this);
    //renderSettingsLabel->setObjectName("darkWidget");
    renderSettingsLabel->setFixedHeight(eSizesUI::widget);
    renderSettingsLayout->addWidget(renderSettingsLabel);
    eSizesUI::widget.addSpacing(renderSettingsLayout);

    mRenderSettingsButton = new QPushButton("Settings");
    mRenderSettingsButton->setObjectName("renderSettings");
    mRenderSettingsButton->setSizePolicy(QSizePolicy::Maximum,
                                        QSizePolicy::Maximum);
    connect(mRenderSettingsButton, &QPushButton::pressed,
            this, &RenderInstanceWidget::openRenderSettingsDialog);
    renderSettingsLayout->addWidget(mRenderSettingsButton);

    renderSettingsLabelWidget->setLayout(renderSettingsLayout);
    renderSettings->setLabelWidget(renderSettingsLabelWidget);

    mContentLayout->addWidget(renderSettings);

    mOutputSettings = new ClosableContainer();
    mOutputSettingsDisplayWidget = new OutputSettingsDisplayWidget(this);
    mOutputSettings->addContentWidget(mOutputSettingsDisplayWidget);

    QWidget *outputSettingsLabelWidget = new QWidget();
    outputSettingsLabelWidget->setObjectName("darkWidget");
    QHBoxLayout *outputSettingsLayout = new QHBoxLayout();
    outputSettingsLayout->setSpacing(0);
    outputSettingsLayout->setMargin(0);
    outputSettingsLayout->setAlignment(Qt::AlignTop);

    eSizesUI::widget.addSpacing(outputSettingsLayout);
    QLabel *outputSettingsLabel = new QLabel("Output Settings:", this);
    outputSettingsLabel->setFixedHeight(eSizesUI::widget);
    outputSettingsLabel->setSizePolicy(QSizePolicy::Maximum,
                                       QSizePolicy::Maximum);
    outputSettingsLayout->addWidget(outputSettingsLabel);
    eSizesUI::widget.addSpacing(outputSettingsLayout);

    mOutputSettingsProfilesButton = new OutputProfilesListButton(this);
    connect(mOutputSettingsProfilesButton, &OutputProfilesListButton::profileSelected,
            this, &RenderInstanceWidget::outputSettingsProfileSelected);
    mOutputSettingsProfilesButton->setObjectName("renderSettings");
    mOutputSettingsProfilesButton->setFixedSize(eSizesUI::widget, eSizesUI::widget);
    mOutputSettingsProfilesButton->setIconSize(QSize(eSizesUI::widget, eSizesUI::widget));

    mOutputSettingsButton = new QPushButton("Settings");
    mOutputSettingsButton->setObjectName("renderSettings");
    mOutputSettingsButton->setSizePolicy(QSizePolicy::Maximum,
                                         QSizePolicy::Maximum);
    connect(mOutputSettingsButton, &QPushButton::pressed,
            this, &RenderInstanceWidget::openOutputSettingsDialog);
    outputSettingsLayout->addWidget(mOutputSettingsProfilesButton);
    outputSettingsLayout->addWidget(mOutputSettingsButton);

    outputSettingsLayout->addSpacing(6*eSizesUI::widget);

    QLabel *outputDestinationLabel = new QLabel("Output Destination:", this);
    outputDestinationLabel->setFixedHeight(eSizesUI::widget);
    outputDestinationLabel->setSizePolicy(QSizePolicy::Maximum,
                                          QSizePolicy::Maximum);
    outputSettingsLayout->addWidget(outputDestinationLabel);
    eSizesUI::widget.addSpacing(outputSettingsLayout);

    mOutputDestinationButton = new QPushButton("Destination");
    mOutputDestinationButton->setObjectName("renderSettings");
    mOutputDestinationButton->setSizePolicy(QSizePolicy::Maximum,
                                            QSizePolicy::Maximum);
    connect(mOutputDestinationButton, &QPushButton::pressed,
            this, &RenderInstanceWidget::openOutputDestinationDialog);
    outputSettingsLayout->addWidget(mOutputDestinationButton);


    outputSettingsLabelWidget->setLayout(outputSettingsLayout);

    mOutputSettings->setLabelWidget(outputSettingsLabelWidget);

    mContentLayout->addWidget(mOutputSettings);

    mContentLayout->setMargin(0);
    mContentLayout->setSpacing(0);
}

void RenderInstanceWidget::updateFromSettings() {
    const auto renderState = mSettings.getCurrentState();
    bool enabled = renderState != RenderState::paused &&
       renderState != RenderState::rendering;
    setEnabled(enabled);
    QString nameLabelTxt = "&nbsp;&nbsp;" + mSettings.getName() +
            "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    if(renderState == RenderState::error) {
        nameLabelTxt += "<font color='red'>" +
                        mSettings.getRenderError() +
                        "</font>";
    } else if(renderState == RenderState::finished) {
        nameLabelTxt += "<font color='green'>finished</font>";
        mCheckBox->setChecked(false);
    } else if(renderState == RenderState::rendering) {
        nameLabelTxt += "rendering...";
    } else if(renderState == RenderState::waiting) {
        nameLabelTxt += "waiting";
    } else if(renderState == RenderState::paused) {
        nameLabelTxt += "paused";
    }
    mNameLabel->setText(nameLabelTxt);

    QString destinationTxt = mSettings.getOutputDestination();
    if(destinationTxt.isEmpty()) {
        destinationTxt = "Destination";
    }
    mOutputDestinationButton->setText(destinationTxt);
    const OutputSettings &outputSettings = mSettings.getOutputRenderSettings();
    OutputSettingsProfile *outputProfile = mSettings.getOutputSettingsProfile();
    QString outputTxt;
    if(outputProfile) {
        outputTxt = outputProfile->getName();
    } else {
        const auto formatT = outputSettings.fOutputFormat;
        if(formatT) {
            outputTxt = "Custom " + QString(formatT->long_name);
        } else {
            outputTxt = "Settings";
        }
    }
    mOutputSettingsButton->setText(outputTxt);
    mOutputSettingsDisplayWidget->setOutputSettings(outputSettings);

    const RenderSettings &renderSettings = mSettings.getRenderSettings();
    mRenderSettingsDisplayWidget->setRenderSettings(mSettings.getTargetCanvas(),
                                                    renderSettings);
    const QString str = QString("%1 - %2,  %3%,  %4 x %5,  %6fps").
                            arg(renderSettings.fMinFrame).
                            arg(renderSettings.fMaxFrame).
                            arg(renderSettings.fResolution*100).
                            arg(renderSettings.fVideoWidth).
                            arg(renderSettings.fVideoHeight).
                            arg(renderSettings.fFps);
    mRenderSettingsButton->setText(str);
}

RenderInstanceSettings &RenderInstanceWidget::getSettings() {
    return mSettings;
}

void RenderInstanceWidget::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::RightButton) {
        QMenu menu(this);
        menu.addAction("Duplicate");
        const auto state = mSettings.getCurrentState();
        const bool deletable = state != RenderState::rendering &&
                               state != RenderState::paused;
        menu.addAction("Delete")->setEnabled(deletable);
        const auto act = menu.exec(e->globalPos());
        if(act) {
            if(act->text() == "Duplicate") {
                emit duplicate(mSettings);
            } else if(act->text() == "Delete") {
                deleteLater();
            }
        }
    } else return ClosableContainer::mousePressEvent(e);
}

void RenderInstanceWidget::openOutputSettingsDialog() {
    const OutputSettings &outputSettings = mSettings.getOutputRenderSettings();
    const auto dialog = new OutputSettingsDialog(outputSettings, this);
    if(dialog->exec()) {
        mSettings.setOutputSettingsProfile(nullptr);
        OutputSettings outputSettings = dialog->getSettings();
        mSettings.setOutputRenderSettings(outputSettings);
        const auto outputFormat = outputSettings.fOutputFormat;
        if(!outputFormat) {
            mOutputSettingsButton->setText("Settings");
        } else {
            mOutputSettingsButton->setText("Custom " +
                        QString(outputFormat->long_name));
        }
        mOutputSettingsDisplayWidget->setOutputSettings(outputSettings);
        updateOutputDestinationFromCurrentFormat();
    }
    delete dialog;
}

#include "Private/document.h"

void RenderInstanceWidget::updateOutputDestinationFromCurrentFormat() {
    const OutputSettings &outputSettings = mSettings.getOutputRenderSettings();
    const auto format = outputSettings.fOutputFormat;
    if(!format) return;
    const bool isImgSeq = !std::strcmp(format->name, "image2");
    QString outputDst = mSettings.getOutputDestination();
    if(outputDst.isEmpty()) {
        outputDst = Document::sInstance->projectDirectory() + "/untitled";
    }
    const QString tmpStr = QString(format->extensions);
    const QStringList supportedExt = tmpStr.split(",");
    const QString fileName = outputDst.split("/").last();
    const QStringList dividedName = fileName.split(".");
    QString currExt;
    if(dividedName.count() > 1) {
        QString namePart = dividedName.at(dividedName.count() - 2);
        if(namePart.count() > 0) {
            currExt = dividedName.last();
        }
    }
    if(!supportedExt.contains(currExt) && !supportedExt.isEmpty()) {
        const QString firstSupported = supportedExt.first();
        if(!firstSupported.isEmpty()) {
            if(currExt.isEmpty()) {
                if(outputDst.right(1) == ".") {
                    outputDst = outputDst.left(outputDst.count() - 1);
                }
            } else {
                int extId = outputDst.lastIndexOf("." + currExt);
                outputDst.remove(extId, 1 + currExt.count());
            }
            outputDst += "." + firstSupported;
        }
    }
    if(isImgSeq && outputDst.contains(".") && !outputDst.contains("%05d")) {
        QStringList div = outputDst.split(".");
        div.replace(div.count() - 2, div.at(div.count() - 2) + "%05d");
        outputDst = div.join(".");
    }
    mSettings.setOutputDestination(outputDst);
    mOutputDestinationButton->setText(outputDst);
}

void RenderInstanceWidget::outputSettingsProfileSelected(OutputSettingsProfile *profile) {
    mSettings.setOutputSettingsProfile(profile);
    updateOutputDestinationFromCurrentFormat();
    updateFromSettings();
}

void RenderInstanceWidget::openOutputDestinationDialog() {
    QString supportedExts;
    QString selectedExt;
    const OutputSettings &outputSettings = mSettings.getOutputRenderSettings();
    const auto format = outputSettings.fOutputFormat;
    if(format) {
        QString tmpStr = QString(format->extensions);
        const QStringList supportedExt = tmpStr.split(",");
        selectedExt = "." + supportedExt.first();
        tmpStr.replace(",", " *.");
        supportedExts = "Output File (*." + tmpStr + ")";
    }
    QString iniText = mSettings.getOutputDestination();
    if(iniText.isEmpty()) {
        iniText = Document::sInstance->projectDirectory() +
                  "/untitled" + selectedExt;
    }
    QString saveAs = eDialogs::saveFile("Output Destination",
                                        iniText, supportedExts);
    if(saveAs.isEmpty()) return;
    mSettings.setOutputDestination(saveAs);
    mOutputDestinationButton->setText(saveAs);
    updateOutputDestinationFromCurrentFormat();
}

#include "rendersettingsdialog.h"
void RenderInstanceWidget::openRenderSettingsDialog() {
    const auto dialog = new RenderSettingsDialog(mSettings, this);
    if(dialog->exec()) {
        const RenderSettings sett = dialog->getSettings();
        mSettings.setRenderSettings(sett);
        mSettings.setTargetCanvas(dialog->getCurrentScene());
        updateFromSettings();
    }
    delete dialog;
}

void RenderInstanceWidget::write(eWriteStream &dst) const {
    mSettings.write(dst);
    dst << isChecked();
}

void RenderInstanceWidget::read(eReadStream &src) {
    mSettings.read(src);
    bool checked; src >> checked;
    setChecked(checked);
}

#include "Private/esettings.h"
OutputProfilesListButton::OutputProfilesListButton(RenderInstanceWidget *parent) :
    QPushButton(parent) {
    mParentWidget = parent;
    setIcon(QIcon(eSettings::sIconsDir() + "/down-arrow.png"));
}

void OutputProfilesListButton::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton) {
        QMenu menu;
        int i = 0;
        for(const auto& profile :
            OutputSettingsProfile::sOutputProfiles) {
            QAction *actionT = new QAction(profile->getName());
            actionT->setData(QVariant(i));
            menu.addAction(actionT);
            i++;
        }
        if(OutputSettingsProfile::sOutputProfiles.isEmpty()) {
            menu.addAction("No profiles")->setEnabled(false);
        }
        menu.addSeparator();
        QAction *actionT = new QAction("Edit...");
        actionT->setData(QVariant(-1));
        menu.addAction(actionT);

        QAction *selectedAction = menu.exec(mapToGlobal(QPoint(0, height())));
        if(selectedAction) {
            int profileId = selectedAction->data().toInt();
            if(profileId == -1) {
                const OutputSettings &outputSettings =
                        mParentWidget->getSettings().getOutputRenderSettings();
                const auto dialog = new OutputProfilesDialog(outputSettings, this);
                if(dialog->exec()) {
                    const auto profile = dialog->getCurrentProfile();
                    emit profileSelected(profile);
                }
            } else {
                const auto profile = OutputSettingsProfile::
                        sOutputProfiles.at(profileId).get();
                emit profileSelected(profile);
            }
        }
    }
}
