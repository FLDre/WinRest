#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QDesktopServices>
#include <QUrl>
#include <QContextMenuEvent>
#include <QCursor>
#include <QPoint>
#include <QScreen>
#include <QPropertyAnimation>
#include <QThread>
#include <QSystemTrayIcon>
#include "mysettings.h"
#include "mytimer.h"
#include "settingdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    MySettings *m_settings;

private:
    Ui::MainWindow *ui;

    MyTimer *myTimer;

    QPropertyAnimation *animation;

    QMenu *widgetMenu;
    QAction *setAction;
    QAction *infoAction;

    QSystemTrayIcon *trayIcon;
    QMenu *systemMenu;

    QRect screenRect;
    QRect showRect;

    bool isMove = false;
    QPoint startPos;
    QPoint endPos;
    QPoint dPos;

    enum sideType
    {
        SIDE_TOP,
        SIDE_BOTTOM,
        SIDE_LEFT,
        SIDE_RIGHT,
        SIDE_NONE,
    };
    sideType edgeSide = SIDE_TOP;

    void edgeDetect(QPoint pos);
    void timerInit();
    void animationInit();
    void buttonInit();
    void setWidgetMenu();
    void setSystemMenu();

signals:
    void signal_rest(MyTimer::timerState state);

public slots:
    void hideWindow();
    void showWindow();

private slots:
    void slot_lockButton_clicked();
    void slot_pauseButton_clicked();
    void slot_closeButton_clicked();

    void enterSettings();
    void slot_settingChanged(MySettings::Items items);

    void slot_nextSecond(MyTimer::timerState state, int currSeconds);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};

#endif // MAINWINDOW_H
