#include "settingsdialog.h"
#include "Private/esettings.h"
#include "hardwareinfo.h"
#include "exceptions.h"
#include "GUI/global.h"

#include "performancesettingswidget.h"
#include "interfacesettingswidget.h"
#include "canvassettingswidget.h"
#include "timelinesettingswidget.h"
#include "pluginssettingswidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QStatusBar>

SettingsDialog::SettingsDialog(QWidget * const parent) :
    QDialog(parent) {
    eSizesUI::widget.add(this, [this](const int size) {
        setMinimumWidth(20*size);
    });
    setWindowTitle(tr("Settings"));

    const auto mainLauout = new QVBoxLayout;
    setLayout(mainLauout);

    mTabWidget = new QTabWidget(this);
    eSizesUI::widget.add(this, [this](const int size) {
        mTabWidget->setContentsMargins(size, size, size, size);
    });

    const auto performance = new PerformanceSettingsWidget(this);
    addSettingsWidget(performance,tr("Hardware"));

    //const auto ui = new InterfaceSettingsWidget(this);
    //addSettingsWidget(ui, "Interface");

    const auto canvas = new CanvasSettingsWidget(this);
    addSettingsWidget(canvas, tr("Canvas"));

    const auto timeline = new TimelineSettingsWidget(this);
    addSettingsWidget(timeline, tr("Timeline"));

    const auto plugins = new PluginsSettingsWidget(this);
    addSettingsWidget(plugins, tr("Plugins"));

    //const auto external = new ExternalAppsSettingsWidget(this);
    //addSettingsWidget(external, "External Apps");

    mainLauout->addWidget(mTabWidget);

    const auto buttonsLayout = new QHBoxLayout;

    const auto restoreButton = new QPushButton(QIcon::fromTheme("loop_back"),
                                               tr("Restore Defaults"),
                                               this);
    restoreButton->setFocusPolicy(Qt::NoFocus);

    const auto cancelButton = new QPushButton(QIcon::fromTheme("cancel"),
                                              tr("Close"),
                                              this);
    cancelButton->setFocusPolicy(Qt::NoFocus);

    const auto applyButton = new QPushButton(QIcon::fromTheme("disk_drive"),
                                             tr("Save"),
                                             this);
    applyButton->setFocusPolicy(Qt::NoFocus);

    buttonsLayout->addWidget(restoreButton);
    eSizesUI::widget.addSpacing(buttonsLayout);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(applyButton);
    buttonsLayout->addWidget(cancelButton);

    eSizesUI::widget.addSpacing(mainLauout);
    mainLauout->addStretch();
    mainLauout->addLayout(buttonsLayout);
    const auto statusBar = new QStatusBar(this);
    mainLauout->addWidget(statusBar);

    connect(restoreButton, &QPushButton::released, this, [this]() {
        eSettings::sInstance->loadDefaults();
        updateSettings(true /* restore */);
        eSizesUI::font.updateSize();
        eSizesUI::widget.updateSize();
    });

    connect(cancelButton, &QPushButton::released, this, &QDialog::close);

    connect(applyButton, &QPushButton::released,
            this, [this, statusBar]() {
        for(const auto widget : mSettingWidgets) {
            widget->applySettings();
        }
        emit eSettings::sInstance->settingsChanged();
        try {
            eSettings& sett = *eSettings::sInstance;
            sett.saveToFile();
        } catch(const std::exception& e) {
            gPrintExceptionCritical(e);
        }
        statusBar->showMessage(tr("Settings saved, you might have to restart"), 1500);
    });

    updateSettings();
}

void SettingsDialog::addSettingsWidget(SettingsWidget * const widget,
                                       const QString &name) {
    mTabWidget->addTab(widget, name);
    mSettingWidgets << widget;
}

void SettingsDialog::updateSettings(bool restore) {
    for(const auto widget : mSettingWidgets) {
        widget->updateSettings(restore);
    }
}
