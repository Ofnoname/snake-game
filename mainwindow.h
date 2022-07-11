#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPoint>
#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>

#include <deque>

#include "argument.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// snake model
struct Snake {
    int dir, cuttail;
    std::deque<QPoint> body;

    Snake() {
        clear();
    }
    void clear() {
        body.resize(0);
        dir = SNAKE_INIT_DIR;
    }
    void reset() {
        body.resize(SNAKE_INIT_LEN);
        dir = SNAKE_INIT_DIR;

        int centerx = CELL_X / 2, centery = CELL_Y / 2;
        for (int i = 0; i < SNAKE_INIT_LEN; i++)
            body[i] = QPoint(centerx, centery+i);
    }
    void turn(int ndir) {
        int t = dir + ndir;
        if (t != 1 && t != 5) dir = ndir;
    }
    int move(QPoint food, int mapid, int inv) {
        const static QPoint d[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

        int nx1 = body[0].x() + (dir == 3), ny1 = body[0].y() + (dir == 1),
            nx2 = nx1 + (dir < 2), ny2 = ny1 + (dir > 1);

        if (!inv && std::count(mapline[mapid].begin(), mapline[mapid].end(), QLine(nx1, ny1, nx2, ny2)))
            return 2; // hit the wall

        auto newhead = body[0] + d[dir];
        if (newhead.rx() < 0 || newhead.rx() >= CELL_X) (newhead.rx() += CELL_X) %= CELL_X;
        if (newhead.ry() < 0 || newhead.ry() >= CELL_Y) (newhead.ry() += CELL_Y) %= CELL_Y;

        body.push_front(newhead);

        if (body[0] == food) return 3; // get food

        body.pop_back();// cut tail
        if (!inv && std::count(body.begin(), body.end(), body[0]) == 2) return 1; //hit self
        return 0; //ok
    }
    int checkfood(QPoint food) {
        if (body[0] == food) {
            cuttail = 0;
            return 1;
        }
        else {
            body.pop_back();
            return 0;
        }
    }
    int checkdead() {
        if (std::count(body.begin(), body.end(), body[0]) == 2) return 1; //撞到自己
        return 0;
    }

    int contains(QPoint food) {
        return std::find(body.begin(), body.end(), food) != body.end();
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void timeoutwork();
    void makefood();

protected slots:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *ev);

private slots:
    void on_startgame_clicked();

    void on_actionEasy_triggered();

    void on_actionMedium_triggered();

    void on_actionHard_triggered();

    void on_actionExpert_triggered();

    void on_actionAbout_triggered();

    void on_actionGame_manual_triggered();

    void on_actionBorderless_triggered();

    void on_actionClassic_Box_triggered();

    void on_actionTrail_Station_triggered();

private:
    Ui::MainWindow *ui;

    QTimer *timer;
    QLabel scorelabel;

    QPoint food; int foodtype; //0~8normal 9bonusscore 10 randomlen 11invincible time
    Snake snake;
    int newdir;

    int status, diffi, score, mapid, newmapid, invcount;
};
#endif // MAINWINDOW_H
