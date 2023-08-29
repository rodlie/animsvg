#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>

class SettingsWidget;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget * const parent = nullptr);
    ~SettingsDialog();

private:
    void addSettingsWidget(SettingsWidget* const widget,
                           const QString& name);
    void updateSettings(bool restore = false);

    QTabWidget* mTabWidget = nullptr;
    QList<SettingsWidget*> mSettingWidgets;
};

#endif // SETTINGSDIALOG_H
