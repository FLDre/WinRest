#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QList>
#include <QLineEdit>
#include <QDebug>
#include <typeinfo>
#include "mysettings.h"
#include "mytimer.h"
#include "notifymanager.h"

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(MySettings *settings, QWidget *parent = 0);
    ~SettingDialog();

private:
    Ui::SettingDialog *ui;

    QIntValidator* validator;
    QList<QLineEdit*>* editList;
    MySettings* myset;
    NotifyManager *manager;

signals:
    void signal_settingChanged(MySettings::Items items);

private slots:
    void accept();
};

#endif // SETTINGDIALOG_H
