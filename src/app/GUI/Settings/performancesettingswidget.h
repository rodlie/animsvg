#ifndef PERFORMANCESETTINGSWIDGET_H
#define PERFORMANCESETTINGSWIDGET_H

#include "settingswidget.h"
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QList>

class PerformanceSettingsWidget : public SettingsWidget
{
public:
    explicit PerformanceSettingsWidget(QWidget *parent = nullptr);

    void applySettings();
    void updateSettings(bool restore = false);
    void updateAudioDevices();

private:
    void setupRasterEffectWidgets();
    void saveRasterEffectsSupport();
    void restoreDefaultRasterEffectsSupport();
    void updateAccPreferenceDesc();

    QCheckBox* mCpuThreadsCapCheck = nullptr;
    QLabel* mCpuThreadsCapLabel = nullptr;
    QSlider* mCpuThreadsCapSlider = nullptr;

    QCheckBox* mRamMBCapCheck = nullptr;
    QSpinBox* mRamMBCapSpin = nullptr;
    QSlider* mRamMBCapSlider = nullptr;

    QLabel* mAccPreferenceLabel = nullptr;
    QLabel* mAccPreferenceDescLabel = nullptr;
    QLabel* mAccPreferenceCpuLabel = nullptr;
    QSlider* mAccPreferenceSlider = nullptr;
    QLabel* mAccPreferenceGpuLabel = nullptr;

    QCheckBox* mPathGpuAccCheck = nullptr;

    QComboBox *mAudioDevicesCombo;

    QList<QComboBox*> mRasterEffectsHardwareSupport;
};

#endif // PERFORMANCESETTINGSWIDGET_H
