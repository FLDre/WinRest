#ifndef MYTIMER_H
#define MYTIMER_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QVector>
#include "mysettings.h"
#include <Windows.h>

class MyTimer : public QTimer
{
    //需要使用信号/槽函数，必须声明Q_OBJECT
    Q_OBJECT
public:
    MyTimer(MySettings *settings, QObject *parent = nullptr);

    static void Lock();

    enum timerState
    {
        STATE_WORK,
        STATE_REST,
        STATE_PAUSE,
    };
    timerState m_state;

//private:
    int m_currSeconds; //定时器目前的时间
    int m_workSeconds; //定时工作时间
    int m_restSeconds; //定时休息时间

signals:
    void signal_Show();
    void signal_Hide();

    void signal_secondChanged(timerState state, int currSeconds);

public slots:
    void setWorkTime(QVariant h, QVariant m, QVariant s);
    void setRestTime(QVariant h, QVariant m, QVariant s);
    void setState(timerState state);

private slots:
    void nextSecond();



};

#endif // MYTIMER_H
