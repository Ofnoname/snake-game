#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , newdir(0)
    , status(1)
    , diffi(DEFAULT_DIF)
    , newmapid(DEFAULY_MAP)
    , timer(new QTimer(this))
{
    ui->setupUi(this);

    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(WINDOW_X, WINDOW_Y);
    setWindowTitle("Snakegame GUI");
    setWindowIcon(QIcon(":/icon.png"));

    ui->actionExpert->setChecked(1);
    ui->actionBorderless->setChecked(1);
    qsrand(time(0));

    connect(timer, &QTimer::timeout, this, &MainWindow::timeoutwork);
    ui->statusbar->addPermanentWidget(new QLabel("Snakegame GUI V0.2"));
    ui->statusbar->addWidget(&scorelabel);
    scorelabel.setFont(QFont(this->font().family(), SCORE_LABEL_SIZE));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::makefood() {
    foodtype = qrand() % 12;
    do {
        food = QPoint(qrand() % (GAME_X/CELL), qrand() % (GAME_Y/CELL)) ;
    }while (snake.contains(food));
}

void MainWindow::timeoutwork() {
    if (status != 0) return;
    if (invcount) invcount--;
    snake.turn(newdir);

    int sig = snake.move(food, mapid, invcount!=0);

    if (sig == 3) {
        // check food effect
        if (foodtype == 9) {
            score += diffi * (diffi-1);
        }
        if (foodtype == 10) {
            int redu_len = qrand() % 4 + 1;
            while (redu_len-- && snake.body.size()>2) snake.body.pop_back();
        }
        if (foodtype == 11) {
            invcount = 10000 / dif_interval[diffi];
        }

        makefood();
        score += diffi;
        scorelabel.setText("Score: " + QString::number(score));
    }
    else if (!invcount && (sig == 1 || sig == 2)) {
        // game end
        timer->stop();
        QMessageBox::warning(this, "Game Over",
                             QString(sig==1 ? "You hit yourself!\n" : "You hit the wall!\n")
                             .append(scorelabel.text()));
        status = 1;
        ui->startgame->show();
        snake.clear();
        ui->actionEasy->setEnabled(1);
        ui->actionHard->setEnabled(1);
        ui->actionMedium->setEnabled(1);
        ui->actionExpert->setEnabled(1);
        ui->actionBorderless->setEnabled(1);
        ui->actionClassic_Box->setEnabled(1);
        ui->actionTrail_Station->setEnabled(1);
        scorelabel.setText("");
    }
    update();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    if (status == 1) return;
    QPainter painter(this);

#define PRINT_LINE(x0, y0, x1, y1) \
    painter.drawLine(x0*CELL+DELTA_X, y0*CELL+DELTA_Y, x1*CELL+DELTA_X, y1*CELL+DELTA_Y)
#define PRINT_REC(x, y, l) \
    painter.drawRect(QRect(x*CELL+DELTA_X+(CELL-l)/2, \
                           y*CELL+DELTA_Y+(CELL-l)/2,l,l))

    // food circle
    QColor foodclr = foodtype==11 ? Qt::blue :
                                    foodtype==10 ? QColor(0x80, 0, 0x80) : //紫色
                                                   foodtype == 9 ? Qt::yellow : Qt::red;
    int csize = foodtype<9 ? 23 : 27;
    painter.setBrush(QBrush(foodclr));
    painter.drawEllipse(food.x()*25+DELTA_X, food.y()*25+DELTA_Y, csize, csize);

    // game border
    painter.setPen(QPen(Qt::gray, 2));
    PRINT_LINE(0, 0, CELL_X, 0);
    PRINT_LINE(0, 0, 0, CELL_Y);
    PRINT_LINE(0, CELL_Y, CELL_X, CELL_Y);
    PRINT_LINE(CELL_X, 0, CELL_X, CELL_Y);

    // map
    painter.setPen(QPen(QColor(0x39, 0xcf, 0xbb), 7));
    for (auto &li: mapline[mapid]) {
        PRINT_LINE(li.x1(), li.y1(), li.x2(), li.y2());
    }

    //snake head
    painter.setPen(QPen(Qt::gray, 0));
    painter.setBrush(QBrush(Qt::darkGray));
    auto head = snake.body[0];
    PRINT_REC(head.x(), head.y(), SCELL);

    // snake
    QColor snakeclr = invcount ? Qt::blue : Qt::black;
    painter.setBrush(QBrush(snakeclr));
    for (auto &node: snake.body) {
        if (node != head)
        PRINT_REC(node.x(), node.y(), SCELL);
    }
#undef PRINT_LINE
#undef PRINT_REC
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key()) {
    case Qt::Key_W: newdir = 0; return;
    case Qt::Key_S: newdir = 1; return;
    case Qt::Key_A: newdir = 2; return;
    case Qt::Key_D: newdir = 3; return;
    case Qt::Key_Space:
        // game pause
        if (status == 0) {
            timer->stop();
            scorelabel.setText("Game paused. Press space to continue.");
            status = 2;
        }
        else if (status == 2) {
            timer->start();
            scorelabel.setText("Score: " + QString::number(score));
            status = 0;
        }
    }
}

// UI Slots
void MainWindow::on_startgame_clicked()
{
    //game start
    makefood();
    ui->startgame->hide();
    status = score = invcount = foodtype = 0;

    timer->setInterval(dif_interval[diffi]);
    timer->start();
    mapid = newmapid;
    snake.reset();
    scorelabel.setText("Score: 0");

    ui->actionEasy->setEnabled(0);
    ui->actionHard->setEnabled(0);
    ui->actionMedium->setEnabled(0);
    ui->actionExpert->setEnabled(0);
    ui->actionBorderless->setEnabled(0);
    ui->actionClassic_Box->setEnabled(0);
    ui->actionTrail_Station->setEnabled(0);
    update();
}

void MainWindow::on_actionEasy_triggered()
{
    ui->actionEasy->setChecked(1);
    ui->actionHard->setChecked(0);
    ui->actionMedium->setChecked(0);
    ui->actionExpert->setChecked(0);
    diffi = 1;
}

void MainWindow::on_actionMedium_triggered()
{
    ui->actionEasy->setChecked(0);
    ui->actionHard->setChecked(1);
    ui->actionMedium->setChecked(0);
    ui->actionExpert->setChecked(0);
    diffi = 2;
}

void MainWindow::on_actionHard_triggered()
{
    ui->actionEasy->setChecked(0);
    ui->actionHard->setChecked(0);
    ui->actionMedium->setChecked(1);
    ui->actionExpert->setChecked(0);
    diffi = 3;
}

void MainWindow::on_actionExpert_triggered()
{
    ui->actionEasy->setChecked(0);
    ui->actionHard->setChecked(0);
    ui->actionMedium->setChecked(0);
    ui->actionExpert->setChecked(1);
    diffi = 4;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About", "Snakegame GUI V0.2\n Author: Ofnoname");
}

void MainWindow::on_actionGame_manual_triggered()
{
    static QString s("A snakegame for tutorial with Qt.\n\n"
                     "Operations:\n"
                     "W: moving up\n"
                     "S: moving down\n"
                     "A: moving left\n"
                     "D: moving right\n"
                     "Space: game pause\n\n"
                     "Food of some color has special effect\n"
                     "Red: None\n"
                     "Gold: Extra score bonus\n"
                     "Blue: 10s invicibility(able to cross yourself and wall)\n"
                     "Purple: Random reduce to your length(1~4)\n");

    QMessageBox::information(this, "Game manual", s);
}

void MainWindow::on_actionBorderless_triggered()
{
    ui->actionBorderless->setChecked(1);
    ui->actionClassic_Box->setChecked(0);
    ui->actionTrail_Station->setChecked(0);
    newmapid = 0;
}

void MainWindow::on_actionClassic_Box_triggered()
{
    ui->actionBorderless->setChecked(0);
    ui->actionClassic_Box->setChecked(1);
    ui->actionTrail_Station->setChecked(0);
    newmapid = 1;
}

void MainWindow::on_actionTrail_Station_triggered()
{
    ui->actionBorderless->setChecked(0);
    ui->actionClassic_Box->setChecked(0);
    ui->actionTrail_Station->setChecked(1);
    newmapid = 2;
}
