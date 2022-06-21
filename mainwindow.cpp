#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    this->setStyleSheet("border-radius:50px;background-color:#aaff7f;");

    //初始化参数设置
    m_settings = new MySettings("rest_settings.ini", QSettings::IniFormat, this);

    timerInit();        //定时器初始化
    animationInit();    //动画变量初始化
    buttonInit();       //按钮初始化

    //菜单设置
    setWidgetMenu();
    setSystemMenu();

    //设置窗口无边框 以及 窗口显示在其他窗口的顶部
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //设置窗口透明背景
//    this->setAttribute(Qt::WA_TranslucentBackground);

    screenRect = QApplication::primaryScreen()->geometry(); //获取当前屏幕的尺寸
//    screenRect = QApplication::primaryScreen()->availableGeometry(); //获取当前屏幕除任务栏等外的尺寸
    this->move(QPoint((screenRect.width() - this->width()) / 2, 0));    //窗口移动到屏幕正上方
}

MainWindow::~MainWindow()
{
    delete ui;
}

//定时器初始化
void MainWindow::timerInit()
{
    myTimer = new MyTimer(m_settings, this);
    connect(myTimer, &MyTimer::signal_secondChanged, this, &MainWindow::slot_nextSecond);
    connect(myTimer, &MyTimer::signal_Hide, this, &MainWindow::hideWindow);
    connect(myTimer, &MyTimer::signal_Show, this, &MainWindow::showWindow);
    connect(this, &MainWindow::signal_rest, myTimer, &MyTimer::setState);
}

//窗口动画初始化
void MainWindow::animationInit()
{
    animation = new QPropertyAnimation(this, "pos");
//    animation->setTargetObject(this);     //设置使用动画的控件
//    animation->setPropertyName("pos");    //指定动画属性名
}

//按钮初始化
void MainWindow::buttonInit()
{
    //连接按钮
    connect(ui->lockButton, &QPushButton::clicked, this, &MainWindow::slot_lockButton_clicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::slot_pauseButton_clicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &MainWindow::slot_closeButton_clicked);

    //设置按钮的提示信息
    ui->lockButton->setToolTip("锁屏");
    ui->pauseButton->setToolTip("暂停");
    ui->closeButton->setToolTip("退出");
}

//添加右键菜单
void MainWindow::setWidgetMenu()
{
    widgetMenu = new QMenu();
    setAction = new QAction("设置", this);
    infoAction = new QAction("关于", this);
    connect(setAction, &QAction::triggered, this, [=]()
    {
        enterSettings();
    });
    connect(infoAction, &QAction::triggered, this, [=]()
    {
        QDesktopServices::openUrl(QUrl("http://github.com"));
    });
    widgetMenu->addAction(setAction);
    widgetMenu->addSeparator();   //设置分隔符
    widgetMenu->addAction(infoAction);
}

//添加系统托盘
void MainWindow::setSystemMenu()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/res/logo.ico"));

    systemMenu = new QMenu();
    systemMenu->addAction(setAction);
    systemMenu->addSeparator();   //设置分隔符
    systemMenu->addAction(infoAction);
    systemMenu->addAction("退出", [=]()
    {
        QApplication::exit();
    });

    trayIcon->setContextMenu(systemMenu);
    trayIcon->show();
}

//锁屏按钮点击槽函数
void MainWindow::slot_lockButton_clicked()
{
    emit signal_rest(MyTimer::STATE_REST);
    QThread::msleep(1000);
    MyTimer::Lock();
}

//暂停按钮点击槽函数
void MainWindow::slot_pauseButton_clicked()
{
    if(myTimer->isActive())
        myTimer->stop();
    else
        myTimer->start();
}

//关闭按钮点击槽函数
void MainWindow::slot_closeButton_clicked()
{
    QApplication::exit();
}

//右键菜单函数
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
//    widgetMenu->exec(QCursor::pos());//鼠标当前位置
    widgetMenu->exec(event->globalPos());

}

//重写鼠标按压事件函数
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isMove = true;
        startPos = QCursor::pos();
        dPos = startPos - this->geometry().topLeft();
    }

    QMainWindow::mousePressEvent(event);
}

//重写鼠标移动事件函数
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(isMove)
    {
        edgeDetect(QCursor::pos());
//        qDebug() << "x = " << QCursor::pos().x() << " y = " << QCursor::pos().y() << " edgeSide = " << edgeSide;
        endPos = QCursor::pos();
        this->move(endPos - dPos);
    }

    QMainWindow::mouseMoveEvent(event);
}

//重写鼠标释放事件函数
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    isMove = false;

    QMainWindow::mouseReleaseEvent(event);
}

//重写鼠标进入事件
void MainWindow::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(edgeSide != SIDE_NONE)
        showWindow();
}

//重写鼠标离开事件
void MainWindow::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(edgeSide != SIDE_NONE)
        hideWindow();
}

//屏幕边缘检测
void MainWindow::edgeDetect(QPoint pos)
{
    if(pos.x() < 5)
    {
        edgeSide = SIDE_LEFT;
    }
    else if(pos.x() > screenRect.width()-5)
    {
        edgeSide = SIDE_RIGHT;
    }
    else if(pos.y() < 5)
    {
        edgeSide = SIDE_TOP;
    }
    else if(pos.y() > screenRect.height()-5)
    {
        edgeSide = SIDE_BOTTOM;
    }
    else
    {
        edgeSide= SIDE_NONE;
    }
}

//隐藏窗口
void MainWindow::hideWindow()
{
    if(edgeSide != SIDE_NONE)
    {
        animation->setStartValue(this->pos());
        if(edgeSide == SIDE_TOP)
            animation->setEndValue(QPoint(x(), 2 - height()));
        else if(edgeSide == SIDE_BOTTOM)
            animation->setEndValue(QPoint(x(), screenRect.height() - 2));
        else if(edgeSide == SIDE_LEFT)
            animation->setEndValue(QPoint(2 - width(), y()));
        else if(edgeSide == SIDE_RIGHT)
            animation->setEndValue(QPoint(screenRect.width() - 2, y()));
        animation->setDuration(250);
        animation->start();
    }
}

//显示窗口
void MainWindow::showWindow()
{
    if(edgeSide != SIDE_NONE)
    {
        animation->setStartValue(this->pos());
        if(edgeSide == SIDE_TOP)
            animation->setEndValue(QPoint(x(), 0));
        else if(edgeSide == SIDE_BOTTOM)
            animation->setEndValue(QPoint(x(), screenRect.height() - height()));
        else if(edgeSide == SIDE_LEFT)
            animation->setEndValue(QPoint(0, y()));
        else if(edgeSide == SIDE_RIGHT)
            animation->setEndValue(QPoint(screenRect.width() - width(), y()));
        animation->setDuration(250);
        animation->start();
    }
}

//进入设置窗口
void MainWindow::enterSettings()
{
    SettingDialog* settingDialog = new SettingDialog(m_settings, this);
    connect(settingDialog, &SettingDialog::signal_settingChanged, this, &MainWindow::slot_settingChanged);
    settingDialog->show();
}

//设置更改槽函数
void MainWindow::slot_settingChanged(MySettings::Items items)
{
    if(items["isSimple"].toBool())
    {
        ui->lockButton->setVisible(false);
        ui->pauseButton->setVisible(false);
        ui->closeButton->setVisible(false);
        this->setFixedWidth(5 * 2 + ui->timeLabel->width());
    }
    else
    {
        ui->lockButton->setVisible(true);
        ui->pauseButton->setVisible(true);
        ui->closeButton->setVisible(true);
        this->setFixedWidth(5 * 5
                            + ui->timeLabel->width()
                            + ui->lockButton->width()
                            + ui->pauseButton->width()
                            + ui->closeButton->width());
    }

    myTimer->setWorkTime(items["Wh"], items["Wm"], items["Ws"]);
    myTimer->setRestTime(items["Rh"], items["Rm"], items["Rs"]);

    myTimer->setState(MyTimer::STATE_WORK);

}

//Label显示槽函数
void MainWindow::slot_nextSecond(MyTimer::timerState state, int currSeconds)
{
    if(state == MyTimer::STATE_WORK)
    {
        ui->timeLabel->setText(QString("%1 %2:%3:%4")
                               .arg("W")
                               .arg(currSeconds / 60 / 60, 2, 10, QLatin1Char('0'))
                               .arg(currSeconds / 60 % 60, 2, 10, QLatin1Char('0'))
                               .arg(currSeconds % 60, 2, 10, QLatin1Char('0')));
    }
    else if(state == MyTimer::STATE_REST)
    {
        ui->timeLabel->setText(QString("%1 %2:%3:%4")
                               .arg("R")
                               .arg(currSeconds / 60 / 60, 2, 10, QLatin1Char('0'))
                               .arg(currSeconds / 60 % 60, 2, 10, QLatin1Char('0'))
                               .arg(currSeconds % 60, 2, 10, QLatin1Char('0')));
    }
}




