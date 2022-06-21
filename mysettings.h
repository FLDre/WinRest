#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QSettings>
#include <QMap>
#include <QDebug>
#include <QVariant>

class MySettings : public QSettings
{
public:
    typedef QMap<QString, QVariant> Items;

    MySettings(const QString &fileName, Format format, QObject *parent = nullptr);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void setValue(MySettings::Items items);

private:
    MySettings::Items defaultSettings;
};

#endif // MYSETTINGS_H
