#include "mysettings.h"

MySettings::MySettings(const QString &fileName, QSettings::Format format, QObject *parent)
    : QSettings(fileName, format, parent)
{
    defaultSettings.insert("isSimple", false);
    defaultSettings.insert("Wh", 0);
    defaultSettings.insert("Wm", 40);
    defaultSettings.insert("Ws", 0);
    defaultSettings.insert("Rh", 0);
    defaultSettings.insert("Rm", 1);
    defaultSettings.insert("Rs", 0);
}

//重写value函数，用于读取对应key的value值
QVariant MySettings::value(const QString &key, const QVariant &defaultValue) const
{
    if(defaultValue.isValid())  //判断参数是否为有效数据类型
        return QSettings::value(key, defaultValue);
    else
        return QSettings::value(key, defaultSettings[key]);
}

void MySettings::setValue(MySettings::Items items)
{
    QStringList keys = items.keys();
    for(int i = 0; i < keys.size(); i++)
    {
        QSettings::setValue(keys[i], items[keys[i]]);
    }
}


