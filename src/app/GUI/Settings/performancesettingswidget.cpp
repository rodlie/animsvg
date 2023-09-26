#include "performancesettingswidget.h"
#include "Private/esettings.h"
#include "exceptions.h"
#include "hardwareinfo.h"
#include "GUI/global.h"

#include "../audiohandler.h"
#include "appsupport.h"

#include <QTimer>
#include <QGroupBox>
#include <QScrollArea>

#define RASTER_HW_SUPPORT_ID Qt::UserRole + 1

PerformanceSettingsWidget::PerformanceSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
    , mAudioDevicesCombo(nullptr)
{

    QHBoxLayout* cpuCapSett = new QHBoxLayout;

    mCpuThreadsCapCheck = new QCheckBox(tr("CPU cap"), this);
    mCpuThreadsCapLabel = new QLabel(this);
    mCpuThreadsCapSlider = new QSlider(Qt::Horizontal);
    mCpuThreadsCapSlider->setRange(1, HardwareInfo::sCpuThreads());
    connect(mCpuThreadsCapCheck, &QCheckBox::toggled,
            mCpuThreadsCapSlider, &QWidget::setEnabled);
    connect(mCpuThreadsCapCheck, &QCheckBox::toggled,
            mCpuThreadsCapLabel, &QWidget::setEnabled);
    mCpuThreadsCapSlider->setEnabled(false);
    mCpuThreadsCapLabel->setEnabled(false);
    connect(mCpuThreadsCapSlider, &QSlider::valueChanged,
            this, [this](const int val) {
        const int nTot = HardwareInfo::sCpuThreads();
        mCpuThreadsCapLabel->setText(QString("%1 / %2").arg(val).arg(nTot));
    });

    addWidget(mCpuThreadsCapCheck);
    cpuCapSett->addWidget(mCpuThreadsCapLabel);
    cpuCapSett->addWidget(mCpuThreadsCapSlider);
    addLayout(cpuCapSett);

    addSeparator();

    QHBoxLayout* ramCapSett = new QHBoxLayout;

    mRamMBCapCheck = new QCheckBox(tr("RAM cap"), this);
    mRamMBCapSpin = new QSpinBox(this);
    mRamMBCapSpin->setRange(250, intMB(HardwareInfo::sRamKB()).fValue);
    mRamMBCapSpin->setSuffix(" MB");
    mRamMBCapSpin->setEnabled(false);

    mRamMBCapSlider = new QSlider(Qt::Horizontal);
    mRamMBCapSlider->setRange(250, intMB(HardwareInfo::sRamKB()).fValue);
    mRamMBCapSlider->setEnabled(false);

    connect(mRamMBCapCheck, &QCheckBox::toggled,
            mRamMBCapSpin, &QWidget::setEnabled);
    connect(mRamMBCapCheck, &QCheckBox::toggled,
            mRamMBCapSlider, &QWidget::setEnabled);

    connect(mRamMBCapSpin, qOverload<int>(&QSpinBox::valueChanged),
            mRamMBCapSlider, &QSlider::setValue);
    connect(mRamMBCapSlider, &QSlider::valueChanged,
            mRamMBCapSpin, &QSpinBox::setValue);

    ramCapSett->addWidget(mRamMBCapCheck);
    ramCapSett->addWidget(mRamMBCapSpin);
    addLayout(ramCapSett);
    addWidget(mRamMBCapSlider);

    addSeparator();

    const auto gpuGroup = new QGroupBox(HardwareInfo::sGpuRendererString(),
                                        this);
    const auto gpuGroupLayout = new QVBoxLayout(gpuGroup);

    mAccPreferenceLabel = new QLabel(tr("Acceleration preference"));
    const auto sliderWidget = new QWidget(this);
    const auto sliderLayout = new QHBoxLayout(sliderWidget);
    mAccPreferenceCpuLabel = new QLabel(tr("CPU"));
    mAccPreferenceSlider = new QSlider(Qt::Horizontal);
    mAccPreferenceSlider->setRange(0, 4);
    mAccPreferenceGpuLabel = new QLabel(tr("GPU"));
    sliderLayout->addWidget(mAccPreferenceCpuLabel);
    sliderLayout->addWidget(mAccPreferenceSlider);
    sliderLayout->addWidget(mAccPreferenceGpuLabel);
    mAccPreferenceDescLabel = new QLabel(this);
    mAccPreferenceDescLabel->setAlignment(Qt::AlignCenter);
    connect(mAccPreferenceSlider, &QSlider::valueChanged,
            this, &PerformanceSettingsWidget::updateAccPreferenceDesc);
    gpuGroupLayout->addWidget(mAccPreferenceLabel);
    gpuGroupLayout->addWidget(sliderWidget);
    gpuGroupLayout->addWidget(mAccPreferenceDescLabel);

    mPathGpuAccCheck = new QCheckBox(tr("Path GPU acceleration"), this);
    gpuGroupLayout->addWidget(mPathGpuAccCheck);
    addWidget(gpuGroup);

    const auto audioWidget = new QGroupBox(this);
    audioWidget->setTitle(tr("Audio"));

    const auto audioLayout = new QHBoxLayout(audioWidget);

    QLabel *audioLabel = new QLabel(tr("Output"), this);
    mAudioDevicesCombo = new QComboBox(this);
    mAudioDevicesCombo->setSizePolicy(QSizePolicy::Preferred,
                                      QSizePolicy::Preferred);
    mAudioDevicesCombo->setFocusPolicy(Qt::NoFocus);
    mAudioDevicesCombo->setEnabled(false);

    audioLayout->addWidget(audioLabel);
    audioLayout->addWidget(mAudioDevicesCombo);

    addSeparator();
    addWidget(audioWidget);

    setupRasterEffectWidgets();

    QTimer::singleShot(250, this,
                       &PerformanceSettingsWidget::updateAudioDevices);
    connect(AudioHandler::sInstance, &AudioHandler::deviceChanged,
            this, [this]() { updateAudioDevices(); });
}

void PerformanceSettingsWidget::applySettings()
{
    mSett.fCpuThreadsCap = mCpuThreadsCapCheck->isChecked() ?
                mCpuThreadsCapSlider->value() : 0;
    mSett.fRamMBCap = intMB(mRamMBCapCheck->isChecked() ?
                mRamMBCapSpin->value() : 0);
    mSett.fAccPreference = static_cast<AccPreference>(
                mAccPreferenceSlider->value());
    mSett.fPathGpuAcc = mPathGpuAccCheck->isChecked();

    saveRasterEffectsSupport();

    const auto audioHandler = AudioHandler::sInstance;
    if (mAudioDevicesCombo->currentText() != audioHandler->getDeviceName()) {
        audioHandler->initializeAudio(mAudioDevicesCombo->currentText(), true);
    }
}

void PerformanceSettingsWidget::updateSettings(bool restore)
{
    const bool capCpu = mSett.fCpuThreadsCap > 0;
    mCpuThreadsCapCheck->setChecked(capCpu);
    const int nThreads = capCpu ? mSett.fCpuThreadsCap :
                                  HardwareInfo::sCpuThreads();
    mCpuThreadsCapSlider->setValue(nThreads);

    const bool capRam = mSett.fRamMBCap.fValue > 250;
    mRamMBCapCheck->setChecked(capRam);
    const int nRamMB = capRam ? mSett.fRamMBCap.fValue :
                                intMB(HardwareInfo::sRamKB()).fValue;
    mRamMBCapSpin->setValue(nRamMB);

    mAccPreferenceSlider->setValue(static_cast<int>(mSett.fAccPreference));
    updateAccPreferenceDesc();
    mPathGpuAccCheck->setChecked(mSett.fPathGpuAcc);

    if (restore) {
        AudioHandler::sInstance->initializeAudio(QString(), true);
        restoreDefaultRasterEffectsSupport();
    }
}

void PerformanceSettingsWidget::updateAudioDevices()
{
    const auto mAudioHandler = AudioHandler::sInstance;
    mAudioDevicesCombo->blockSignals(true);
    mAudioDevicesCombo->clear();
    mAudioDevicesCombo->addItems(mAudioHandler->listDevices());
    if (mAudioDevicesCombo->count() > 0) {
        mAudioDevicesCombo->setCurrentText(mAudioHandler->getDeviceName());
        mAudioDevicesCombo->setEnabled(true);
    } else { mAudioDevicesCombo->setEnabled(false); }
    mAudioDevicesCombo->blockSignals(false);
}

void PerformanceSettingsWidget::setupRasterEffectWidgets()
{
    QStringList effects = {"Blur",
                           "BrightnessContrast",
                           "Colorize",
                           "MotionBlur",
                           "NoiseFade",
                           "Shadow",
                           "Wipe"};
    HardwareSupport defaultSupport = HardwareSupport::gpuPreffered;

    addSeparator();

    const auto area = new QScrollArea(this);
    const auto container = new QGroupBox(this);
    const auto containerLayout = new QVBoxLayout(container);
    const auto containerInner = new QWidget(this);
    const auto containerInnerLayout = new QVBoxLayout(containerInner);

    area->setWidget(containerInner);
    area->setWidgetResizable(true);
    area->setContentsMargins(0, 0, 0, 0);

    container->setTitle(tr("Raster Effects"));

    container->setContentsMargins(0, 0, 0, 0);

    containerInnerLayout->setMargin(5);
    containerLayout->setMargin(0);

    containerLayout->addWidget(area);

    for (const auto &effect : effects) {
        const auto box = new QComboBox(this);
        box->addItem(tr("CPU-only"), static_cast<int>(HardwareSupport::cpuOnly));
        box->addItem(tr("CPU preffered"), static_cast<int>(HardwareSupport::cpuPreffered));
        box->addItem(tr("GPU-only"), static_cast<int>(HardwareSupport::gpuOnly));
        box->addItem(tr("GPU preffered"), static_cast<int>(HardwareSupport::gpuPreffered));
        box->setItemData(0, effect, RASTER_HW_SUPPORT_ID);
        box->setCurrentText(AppSupport::getRasterEffectHardwareSupportString(effect, defaultSupport));
        mRasterEffectsHardwareSupport << box;

        const auto label = new QLabel(effect, this);
        const auto wid = new QWidget(this);
        const auto lay = new QHBoxLayout(wid);

        wid->setContentsMargins(0, 0, 0, 0);
        lay->setMargin(0);
        lay->addWidget(label);
        lay->addWidget(box);

        containerInnerLayout->addWidget(wid);
    }

    containerLayout->addStretch();

    addWidget(container);
}

void PerformanceSettingsWidget::saveRasterEffectsSupport()
{
    QSettings settings;
    settings.beginGroup("RasterEffects");
    for (const auto &box : mRasterEffectsHardwareSupport) {
        settings.setValue(QString("%1HardwareSupport")
                          .arg(box->itemData(0,
                                             RASTER_HW_SUPPORT_ID).toString()),
                          box->currentData());
    }
    settings.endGroup();
}

void PerformanceSettingsWidget::restoreDefaultRasterEffectsSupport()
{
    for (const auto &box : mRasterEffectsHardwareSupport) {
        box->setCurrentText(tr("GPU preffered"));
    }
}

void PerformanceSettingsWidget::updateAccPreferenceDesc()
{
    const int value = mAccPreferenceSlider->value();
    QString toolTip;
    if (value == 0) {
        mAccPreferenceDescLabel->setText(tr("Strong CPU preference"));
        toolTip = tr("Use the GPU only for tasks not supported by the CPU");
    } else if (value == 1) {
        mAccPreferenceDescLabel->setText(tr("Soft CPU preference"));
        toolTip = tr("Use the GPU only for tasks marked as preferred for the GPU");
    } else if (value == 2) {
        mAccPreferenceDescLabel->setText(tr(" Hardware agnostic (recommended) "));
        toolTip = tr("Adhere to the default hardware preference");
    } else if (value == 3) {
        mAccPreferenceDescLabel->setText(tr("Soft GPU preference"));
        toolTip = tr("Use the CPU only for tasks marked as preferred for the CPU");
    } else if (value == 4) {
        mAccPreferenceDescLabel->setText(tr("Strong GPU preference"));
        toolTip = tr("Use the CPU only for tasks not supported by the GPU");
    }
    mAccPreferenceDescLabel->setToolTip(gSingleLineTooltip(toolTip));
    mAccPreferenceSlider->setToolTip(gSingleLineTooltip(toolTip));
}
