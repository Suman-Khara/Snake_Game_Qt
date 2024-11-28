#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <deque>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent* event) override;
private slots:
    void on_New_Game_clicked();

private:
    int gridOffset = 15;
    int centerX = 0;
    int centerY = 0;
    int width = 0;
    int height = 0;
    int started = -1;
    int elapsedTime = 0;
    int score;
    int interval = 500;
    QTimer* timer;
    QTimer* gameTimer;
    std::deque<QPoint> snake;
    QSet<QPoint> snakePoints;
    QVector<int> direction = { 0, 0 };
    Ui::MainWindow* ui;
    void colorPointAbsolute(int x, int y, int r, int g, int b, int penwidth);
    void colorPointRelative(int x, int y, int r, int g, int b);
    void delay(int ms);
    QPoint food;
    void startGame();
    void moveSnake();
    QColor getPixelColor(int x, int y);
    void growFood();
    void plantBomb();
    void updateWatch();
    QSet<QPoint> walls;
    void createMode2();
    void createMode3();
    QVector<QVector<bool>> bitmap;
    double bombProbability = 0.3;
    QPoint bomb;
    //void renderSnakeGameText();
    void drawTextOnWorkArea(const QString& text, int fontSize, QColor color);
    QString playerName;
    void loadHighScores();
    void saveHighScores();
    void updateHighScores();
    void updateRankLabels(const QString& key);
    void initializeHighScoresFile();
    void renderSnakeGameText();
};
#endif // MAINWINDOW_H
