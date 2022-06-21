#include "mytimer.h"

MyTimer::MyTimer(MySettings *settings, QObject *parent)
    : QTimer(parent)
{   
    this->setTimerType(Qt::TimerType::CoarseTimer);    //设置定时器的精度
    this->setInterval(1000);    //设置1000ms的定时间隔
    connect(this,&MyTimer::timeout, this, &MyTimer::nextSecond);

    setWorkTime(settings->value("Wh"), settings->value("Wm"), settings->value("Ws"));
    setRestTime(settings->value("Wh"), settings->value("Wm"), settings->value("Ws"));
    setState(STATE_WORK);
}

void MyTimer::Lock()
{
    LockWorkStation();  //windows API 对桌面进行锁屏
}

void MyTimer::nextSecond()
{
    qDebug() << "m_state = " << m_state;
    qDebug() << "m_currSeconds = " << m_currSeconds;
    if(m_state == STATE_WORK)
    {
        m_currSeconds--;
        if(m_currSeconds <= 0)
            setState(STATE_REST);
        else if(m_currSeconds == 60)    //剩余60s时，显示窗口
            emit signal_Show();
        else if(m_currSeconds == m_workSeconds - 1) //工作状态开始后1s，隐藏窗口
            emit signal_Hide();
    }
    else if(m_state == STATE_REST)
    {
        m_currSeconds--;
        if(m_currSeconds <= 0)
            setState(STATE_WORK);
        else if(m_currSeconds == m_restSeconds - 1)
            Lock();
    }
    emit signal_secondChanged(m_state, m_currSeconds);
}

//设置工作时间
void MyTimer::setWorkTime(QVariant h, QVariant m, QVariant s)
{
    m_workSeconds = h.toInt() * 3600 + m.toInt() * 60 + s.toInt();
}

//设置休息时间
void MyTimer::setRestTime(QVariant h, QVariant m, QVariant s)
{
    m_restSeconds = h.toInt() * 3600 + m.toInt() * 60 + s.toInt();
}

//设置定时器状态
void MyTimer::setState(timerState state)
{
    m_state = state;
    if(state == STATE_WORK)
        m_currSeconds = m_workSeconds;
    else if(state == STATE_REST)
        m_currSeconds = m_restSeconds;
    this->start();
}






