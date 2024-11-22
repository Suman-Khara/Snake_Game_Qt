#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QThread>
#define Delay delay(1)
QElapsedTimer bombTimer;
bool nextBomb=false;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->workArea->setFocusPolicy(Qt::StrongFocus);
    ui->workArea->setFocus();

    QPixmap canvas = ui->workArea->pixmap(Qt::ReturnByValue);
    if (canvas.isNull()) {
        canvas = QPixmap(ui->workArea->size());
        canvas.fill(Qt::white);
        ui->workArea->setPixmap(canvas);
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::moveSnake);
    timer->start(interval);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateWatch);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::delay(int ms){
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

void MainWindow::colorPointAbsolute(int x, int y, int r, int g, int b, int penwidth) {
    QPixmap canvas = ui->workArea->pixmap();
    QPainter painter(&canvas);
    QPen pen = QPen(QColor(r, g, b), penwidth);
    painter.setPen(pen);
    painter.drawPoint(x, y);
    ui->workArea->setPixmap(canvas);
}

void MainWindow::colorPointRelative(int x, int y, int r, int g, int b) {
    int absX = centerX + x * gridOffset + gridOffset / 2;
    int absY = centerY - y * gridOffset - gridOffset / 2;
    colorPointAbsolute(absX, absY, r, g, b, gridOffset);
}

void MainWindow::on_New_Game_clicked() {
     // "SNAKE GAME"
     renderSnakeGameText();
    // Inside on_New_Game_clicked function

    // Check if level and difficulty are selected
    if (!(ui->Mode_1->isChecked() || ui->Mode_2->isChecked() || ui->Mode_3->isChecked())) {
        ui->Prompt->setText("Select a Level to Start the Game.");
        return;
    }
    if (!(ui->Easy->isChecked() || ui->Medium->isChecked() || ui->Hard->isChecked())) {
        ui->Prompt->setText("Select a Difficulty to Start the Game.");
        return;
    }

    // Set the interval based on selected difficulty
    int interval = 0;
    if (ui->Easy->isChecked()) {
        interval = 85;
    } else if (ui->Medium->isChecked()) {
        interval = 70;
    } else if (ui->Hard->isChecked()) {
        interval = 55;
    }

    // Restart the game timer with the new interval
    timer->start(interval);

    // Initialize game parameters
    width = ui->workArea->width();
    height = ui->workArea->height();
    centerX = width / 2;
    centerY = height / 2;
    score = 0;

    // Update UI components
    ui->Score->setText("Score: " + QString::number(static_cast<int>(score)));
    ui->Stopwatch->setText("00:00:00");

    // Clear the canvas and reset snake data
    QPixmap canvas = ui->workArea->pixmap();
    canvas.fill(Qt::white);
    ui->workArea->setPixmap(canvas);

    snake.clear();
    snakePoints.clear();
    for (int i = -2; i < 3; i++) {
        snake.push_back({i, 0});
        snakePoints.insert({i, 0});
        colorPointRelative(i, 0, 0, 0, 0);
    }

    // Reset game state
    direction = {0, 0};
    score = 0;
    food = {INT_MAX, INT_MAX};
    bomb = {INT_MAX, INT_MAX};
    started = 0;
    elapsedTime = 0;
    ui->Prompt->setText("Rendering Playground");
    if (ui->Mode_2->isChecked()) {
        createMode2();
    }
    else if (ui->Mode_3->isChecked()) {
        createMode3();
    }
    ui->Prompt->setText("Press Enter to Start");
}

// Function to create boundary walls
void MainWindow::createMode2() {
    walls.clear(); // Clear any existing walls

    int outerXStart = -30, outerXEnd = 29;
    int outerYStart = -25, outerYEnd = 24;

    // Deep orange color
    int r = 255, g = 140, b = 0;

    // Add points for the outer rectangle
    for (int x = outerXStart; x <= outerXEnd; x++) {
        for (int i = 0; i < 2; ++i) { // Top and Bottom rows
            walls.insert(QPoint(x, outerYStart + i));
            walls.insert(QPoint(x, outerYEnd - i));
            colorPointRelative(x, outerYStart + i, r, g, b);
            Delay;
            colorPointRelative(x, outerYEnd - i, r, g, b);
            Delay;
        }
    }
    for (int y = outerYStart; y <= outerYEnd; y++) {
        for (int i = 0; i < 2; ++i) { // Left and Right columns
            walls.insert(QPoint(outerXStart + i, y));
            walls.insert(QPoint(outerXEnd - i, y));
            colorPointRelative(outerXStart + i, y, r, g, b);
            Delay;
            colorPointRelative(outerXEnd - i, y, r, g, b);
            Delay;
        }
    }
}

void MainWindow::createMode3() {
    walls.clear(); // Clear any existing walls

    // Define the boundary range
    int outerXStart = -30, outerXEnd = 29;
    int outerYStart = -25, outerYEnd = 24;

    // Divide the lengths and breadths by 5 to get dividing points
    int length = outerXEnd - outerXStart + 1;
    int breadth = outerYEnd - outerYStart + 1;

    int l1 = outerXStart + length / 5;
    int l2 = outerXStart + 2 * length / 5;
    int l3 = outerXStart + 3 * length / 5;
    int l4 = outerXStart + 4 * length / 5;

    int w1 = outerYStart + breadth / 5;
    int w2 = outerYStart + 2 * breadth / 5;
    int w3 = outerYStart + 3 * breadth / 5;
    int w4 = outerYStart + 4 * breadth / 5;

    // Deep orange color
    int r = 255, g = 140, b = 0;

    // Draw upper and lower boundaries
    for (int x = l1; x <= l4; ++x) {
        walls.insert(QPoint(x, outerYStart));
        colorPointRelative(x, outerYStart, r, g, b);
        Delay;
        walls.insert(QPoint(x, outerYEnd));
        colorPointRelative(x, outerYEnd, r, g, b);
        Delay;
    }

    // Draw left and right boundaries
    for (int y = w1; y <= w4; ++y) {
        walls.insert(QPoint(outerXStart, y));
        colorPointRelative(outerXStart, y, r, g, b);
        Delay;
        walls.insert(QPoint(outerXEnd, y));
        colorPointRelative(outerXEnd, y, r, g, b);
        Delay;
    }

    // Draw inner wall system
    // (l1, w1) to (l2, w1)
    for (int x = l1; x <= l2; ++x) {
        walls.insert(QPoint(x, w1));
        colorPointRelative(x, w1, r, g, b);
        Delay;
    }

    // (l3, w1) to (l4, w1)
    for (int x = l3; x <= l4; ++x) {
        walls.insert(QPoint(x, w1));
        colorPointRelative(x, w1, r, g, b);
        Delay;
    }

    // (l4, w1) to (l4, w2)
    for (int y = w1; y <= w2; ++y) {
        walls.insert(QPoint(l4, y));
        colorPointRelative(l4, y, r, g, b);
        Delay;
    }

    // (l4, w3) to (l4, w4)
    for (int y = w3; y <= w4; ++y) {
        walls.insert(QPoint(l4, y));
        colorPointRelative(l4, y, r, g, b);
        Delay;
    }

    // (l4, w4) to (l3, w4)
    for (int x = l4; x >= l3; --x) {
        walls.insert(QPoint(x, w4));
        colorPointRelative(x, w4, r, g, b);
        Delay;
    }

    // (l2, w4) to (l1, w4)
    for (int x = l2; x >= l1; --x) {
        walls.insert(QPoint(x, w4));
        colorPointRelative(x, w4, r, g, b);
        Delay;
    }

    // (l1, w4) to (l1, w3)
    for (int y = w4; y >= w3; --y) {
        walls.insert(QPoint(l1, y));
        colorPointRelative(l1, y, r, g, b);
        Delay;
    }

    // (l1, w2) to (l1, w1)
    for (int y = w2; y >= w1; --y) {
        walls.insert(QPoint(l1, y));
        colorPointRelative(l1, y, r, g, b);
        Delay;
    }
}

QColor MainWindow::getPixelColor(int x, int y) {
    QImage image = ui->workArea->pixmap(Qt::ReturnByValue).toImage();
    return image.pixelColor(x, y);
}

void MainWindow::growFood() {
    int x, y;
    do {
        x = (rand() % (width / gridOffset)) - (width / (2 * gridOffset));
        y = (rand() % (height / gridOffset)) - (height / (2 * gridOffset));
    } while (snakePoints.contains({x, y}) || walls.contains({x, y}) || bomb==QPoint(x, y));

    food = QPoint(x, y);
    colorPointRelative(x, y, 0, 0, 255);
}

void MainWindow::plantBomb() {
    // Check if a bomb should be planted based on the probability
    if (static_cast<double>(rand()) / RAND_MAX > bombProbability) {
        return; // Do not plant a bomb this time
    }

    int x, y;
    do {
        // Generate random coordinates for the bomb
        x = (rand() % (width / gridOffset)) - (width / (2 * gridOffset));
        y = (rand() % (height / gridOffset)) - (height / (2 * gridOffset));
    } while (snakePoints.contains({x, y}) || walls.contains({x, y}) || QPoint(x, y) == food);

    // Set the bomb position and color it red
    bomb = QPoint(x, y);
    colorPointRelative(x, y, 255, 0, 0); // Red color for the bomb
    bombTimer.start();
}

void MainWindow::moveSnake() {
    if (direction[0] == 0 && direction[1] == 0)
        return;

    if (food == QPoint(INT_MAX, INT_MAX))
        growFood();

    if (bomb == QPoint(INT_MAX, INT_MAX) && !nextBomb){
        plantBomb();
        ui->Bomb->setText("BOMB ALERT!!!");
    }

    QPoint head = snake.back();
    int nextX = head.x() + direction[0], nextY = head.y() + direction[1];

    // Wrap around if the snake goes beyond the edges of the window
    if (nextX < -(width / (2 * gridOffset)))
        nextX = width / (2 * gridOffset) - 1;
    else if (nextX >= width / (2 * gridOffset))
        nextX = -(width / (2 * gridOffset));

    if (nextY < -(height / (2 * gridOffset)))
        nextY = height / (2 * gridOffset) - 1;
    else if (nextY >= height / (2 * gridOffset))
        nextY = -(height / (2 * gridOffset));

    if (snakePoints.contains({nextX, nextY}) || walls.contains({nextX, nextY}) || bomb == QPoint(nextX, nextY)) {
        ui->Prompt->setText("Game Over");
        direction = {0, 0};
        started = -1;
        gameTimer->stop();
        return;
    }

    snake.push_back({nextX, nextY});
    snakePoints.insert({nextX, nextY});
    colorPointRelative(nextX, nextY, 0, 0, 0);

    if (food == QPoint(nextX, nextY)) {
        score += 1;
        ui->Score->setText("Score: " + QString::number(static_cast<int>(score)));
        growFood();
    } else {
        QPoint tail = snake.front();
        colorPointRelative(tail.x(), tail.y(), 255, 255, 255);
        snake.pop_front();
        snakePoints.erase(snakePoints.find(tail));
    }

    // Bomb diffusion logic
    if (bombTimer.elapsed() > 12000 && bomb != QPoint(INT_MAX, INT_MAX) && !nextBomb) { // 5 seconds elapsed
        colorPointRelative(bomb.x(), bomb.y(), 255, 255, 255); // Remove bomb by coloring it white
        bomb = QPoint(INT_MAX, INT_MAX);
        nextBomb=true;
        ui->Bomb->setText("BOMB DIFFUSED.");
    }
    if(bombTimer.elapsed()>15000 && nextBomb)
        ui->Bomb->clear();
    if(bombTimer.elapsed()>20000 && nextBomb)
        nextBomb=false;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();

    if (started == 0 && (key == Qt::Key_Enter || key==Qt::Key_Return)) {
        direction = {1, 0};
        started = 1;
        ui->Prompt->setText("Game Started");
        gameTimer->start(1000);
    }

    if (started == 1) {
        QVector<int> newDirection = direction;

        if (key == Qt::Key_Right && !(direction[0] == -1 && direction[1] == 0)) {
            newDirection = {1, 0};
        } else if (key == Qt::Key_Left && !(direction[0] == 1 && direction[1] == 0)) {
            newDirection = {-1, 0};
        } else if (key == Qt::Key_Up && !(direction[0] == 0 && direction[1] == -1)) {
            newDirection = {0, 1};
        } else if (key == Qt::Key_Down && !(direction[0] == 0 && direction[1] == 1)) {
            newDirection = {0, -1};
        }

        if (newDirection != direction) {
            direction = newDirection;
        }
    }
}

void MainWindow::updateWatch() {
    elapsedTime++;
    int hours = elapsedTime / 3600;
    int minutes = (elapsedTime % 3600) / 60;
    int seconds = elapsedTime % 60;

    QString timeString = QString("%1:%2:%3")
                             .arg(hours, 2, 10, QChar('0'))
                             .arg(minutes, 2, 10, QChar('0'))
                             .arg(seconds, 2, 10, QChar('0'));

    ui->Stopwatch->setText(timeString);  // Update the label with the new time
}

void MainWindow::renderSnakeGameText() {
    QPixmap canvas = ui->workArea->pixmap(Qt::ReturnByValue);
    QPainter painter(&canvas);

    // Set the font and color for the text
    QFont font("Arial", 48, QFont::Bold);
    painter.setFont(font);
    painter.setPen(QColor(255, 140, 0)); // Orange color

    // Define the text and position
    QString text = "SNAKE GAME";
    QRect textRect = painter.boundingRect(0, 0, canvas.width(), canvas.height(), Qt::AlignCenter, text);

    // Convert text to a QImage for pixel-level manipulation
    QImage textImage(textRect.size(), QImage::Format_ARGB32);
    textImage.fill(Qt::transparent);
    QPainter textPainter(&textImage);
    textPainter.setFont(font);
    textPainter.setPen(Qt::black);
    textPainter.drawText(textImage.rect(), Qt::AlignCenter, text);
    textPainter.end();
    //"white print"
    // Draw text pixel by pixel
    for (int y = 0; y < textImage.height(); ++y) {
        for (int x = 0; x < textImage.width(); ++x) {
            if (QColor(textImage.pixel(x, y)).alpha() > 0) { // Only draw non-transparent pixels
                painter.drawPoint(textRect.left() + x, textRect.top() + y);
            }
        }
        ui->workArea->setPixmap(canvas); // Update canvas
        QCoreApplication::processEvents(); // Allow UI to update
        QThread::msleep(10); // Delay for the effect
    }
    QFont gameFont("Arial", 48, QFont::Bold);
    QString gameText = "SNAKE GAME";
    QRect textBoundingRect = painter.boundingRect(0, 0, canvas.width(), canvas.height(), Qt::AlignCenter, gameText);

    // First, handle the pixel-based text or other drawing effects
    // Then, after finishing, draw the large text at the end:
    painter.setPen(Qt::white); // Set text color for the large text
    painter.setFont(gameFont); // Use the desired font
    painter.drawText(textBoundingRect, Qt::AlignCenter, gameText);
    ui->workArea->setPixmap(canvas); // Update canvas with both effects


    // Pause for 2 seconds to display the full text
    QEventLoop loop;
    QTimer::singleShot(700, &loop, &QEventLoop::quit);
    loop.exec();

    // Reset canvas pixel by pixel to white
    for (int y = 0; y < canvas.height(); ++y) {
        for (int x = 0; x < canvas.width(); ++x) {
            painter.setPen(Qt::white);
            painter.drawPoint(x, y);
        }
        ui->workArea->setPixmap(canvas); // Update canvas
        QCoreApplication::processEvents(); // Allow UI to update
        QThread::msleep(1); // Delay for the effect
    }
}

