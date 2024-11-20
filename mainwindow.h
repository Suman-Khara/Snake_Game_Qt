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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    //bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    //void on_showAxis_clicked();

    //void on_gridlines_clicked();

    void on_New_Game_clicked();

private:
    int gridOffset=15;
    int centerX=0;
    int centerY=0;
    int width=0;
    int height=0;
    int started=-1;
    int elapsedTime=0;
    double score;
    double speed;
    QTimer *timer;
    QTimer *gameTimer;
    std::deque<QPoint> snake;
    QSet<QPoint> snakePoints;
    QVector<int> direction={0, 0};
    Ui::MainWindow *ui;
    void colorPointAbsolute(int x, int y, int r, int g, int b, int penwidth);
    void colorPointRelative(int x, int y, int r, int g, int b);
    void delay(int ms);
    QPoint food;
    void startGame();
    void moveSnake();
    QColor getPixelColor(int x, int y);
    void growFood();
    bool inSnake(int x, int y);
    void updateWatch();
};
#endif // MAINWINDOW_H