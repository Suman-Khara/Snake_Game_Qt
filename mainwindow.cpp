#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>

#define Delay delay(1)
QElapsedTimer bombTimer;
bool nextBomb=false;
QString filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/high_scores.txt";
struct HighScoreEntry {
    QString name;
    int score;
    QString time; // Format: "hh:mm:ss"

    // Comparison for sorting (higher score is better; lower time breaks ties)
    bool operator<(const HighScoreEntry &other) const {
        if (score == other.score)
            return time < other.time; // Lexicographical comparison works for "hh:mm:ss"
        return score > other.score;
    }
};

std::map<QString, std::vector<HighScoreEntry>> highScores; // Key: "Mode-Difficulty"

void MainWindow::initializeHighScoresFile() {
    QFile file("high_scores.txt");
    if (!file.exists()) {
        // Create an empty file if it doesn't exist
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.close();
        }
    }
}

void MainWindow::loadHighScores() {
    qDebug() << "Loading high scores from: " << QFileInfo("high_scores.txt").absoluteFilePath();
    QFile file("high_scores.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    highScores.clear();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(',');
        if (parts.size() == 5) {
            QString key = parts[0]; // "Mode-Difficulty"
            HighScoreEntry entry = { parts[1], parts[2].toInt(), parts[3] };
            highScores[key].push_back(entry);
        }
    }

    file.close();

    // Sort the scores
    for (auto &pair : highScores) {
        std::sort(pair.second.begin(), pair.second.end());
        if (pair.second.size() > 5)
            pair.second.resize(5); // Keep only top 5
    }
}

void MainWindow::saveHighScores() {
    QFile file("high_scores.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing: " << file.errorString();
        return;
    }

    QTextStream out(&file);
    for (const auto &pair : highScores) {
        for (const HighScoreEntry &entry : pair.second) {
            out << pair.first << ',' << entry.name << ',' << entry.score << ',' << entry.time << '\n';
        }
    }

    file.close();
    qDebug() << "High scores saved successfully!";
    qDebug() << "Saving high scores to: " << QFileInfo("high_scores.txt").absoluteFilePath();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveHighScores();
    QMainWindow::closeEvent(event); // Call base class implementation
}

void MainWindow::updateHighScores() {
    // Determine key based on mode and difficulty
    QString mode = ui->Mode_1->isChecked() ? "Mode_1" : ui->Mode_2->isChecked() ? "Mode_2" : "Mode_3";
    QString difficulty = ui->Easy->isChecked() ? "Easy" : ui->Medium->isChecked() ? "Medium" : "Hard";
    QString key = mode + "-" + difficulty;

    // Create the new entry
    HighScoreEntry newEntry = { playerName, score, ui->Stopwatch->text() };

    // Update the high scores
    auto &scores = highScores[key];
    scores.push_back(newEntry);
    std::sort(scores.begin(), scores.end());
    if (scores.size() > 5)
        scores.resize(5);

    // Check rank
    int rank = -1;
    for (int i = 0; i < scores.size(); ++i) {
        if (scores[i].name == playerName && scores[i].score == score && scores[i].time == ui->Stopwatch->text()) {
            rank = i + 1;
            break;
        }
    }

    // Show rank in congrats label
    if (rank != -1) {
        ui->congrats->setText(playerName + " has got rank " + QString::number(rank));
    }

    // Save high scores
    saveHighScores();

    // Update rank labels
    updateRankLabels(key);
}

void MainWindow::updateRankLabels(const QString &key) {
    const auto &scores = highScores[key];

    QLabel *rankLabels[] = { ui->rank_1, ui->rank_2, ui->rank_3, ui->rank_4, ui->rank_5 };
    for (int i = 0; i < 5; ++i) {
        if (i < scores.size()) {
            rankLabels[i]->setText(QString("%1. %2 - %3 pts - %4")
                                       .arg(i + 1)
                                       .arg(scores[i].name)
                                       .arg(scores[i].score)
                                       .arg(scores[i].time));
        } else {
            rankLabels[i]->clear();
        }
    }
}

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
    drawTextOnWorkArea("SIMPLE SNAKE GAME", 50, QColor(0, 0, 0));
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::moveSnake);
    timer->start(interval);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateWatch);
    ui->nameInput->clear();
    initializeHighScoresFile();
    loadHighScores();
    qDebug() << "Current Working Directory: " << QDir::currentPath();

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
    // Check if the name field is empty
    playerName = ui->nameInput->text().trimmed();
    if (playerName.isEmpty()) {
        ui->Prompt->setText("Please enter your name to start the game.");
        return;
    }

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
    ui->Prompt->setText("Rendering Playground");
    if (ui->Mode_2->isChecked()) {
        createMode2();
    }
    else if (ui->Mode_3->isChecked()) {
        createMode3();
    }
    for (int i = -2; i < 3; i++) {
        snake.push_back({i, 0});
        snakePoints.insert({i, 0});
        colorPointRelative(i, 0, 0, 0, 0);
        Delay;
    }
    // Reset game state
    direction = {0, 0};
    score = 0;
    food = {INT_MAX, INT_MAX};
    bomb = {INT_MAX, INT_MAX};
    started = 0;
    elapsedTime = 0;
    ui->Bomb->clear();
    ui->congrats->clear();
    QString mode = ui->Mode_1->isChecked() ? "Mode_1" : ui->Mode_2->isChecked() ? "Mode_2" : "Mode_3";
    QString difficulty = ui->Easy->isChecked() ? "Easy" : ui->Medium->isChecked() ? "Medium" : "Hard";
    QString key = mode + "-" + difficulty;
    updateRankLabels(key);
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
        updateHighScores();
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

void MainWindow::drawTextOnWorkArea(const QString &text, int fontSize, QColor color) {
    QPixmap canvas = ui->workArea->pixmap(Qt::ReturnByValue);
    if (canvas.isNull()) {
        canvas = QPixmap(ui->workArea->size());
        canvas.fill(Qt::white);
    }

    QPainter painter(&canvas);
    painter.setPen(QPen(color));
    QFont font = painter.font();
    font.setPointSize(fontSize);
    font.setBold(true);
    painter.setFont(font);

    // Center the text on the workArea
    QRect rect = canvas.rect();
    painter.drawText(rect, Qt::AlignCenter, text);

    painter.end();
    ui->workArea->setPixmap(canvas);
}
