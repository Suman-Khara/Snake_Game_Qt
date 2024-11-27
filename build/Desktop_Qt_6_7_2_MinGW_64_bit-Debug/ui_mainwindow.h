/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *workArea;
    QPushButton *New_Game;
    QLabel *Prompt;
    QLabel *Score;
    QLabel *Stopwatch;
    QGroupBox *Mode;
    QVBoxLayout *verticalLayout;
    QRadioButton *Mode_1;
    QRadioButton *Mode_2;
    QRadioButton *Mode_3;
    QGroupBox *Difficulty;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *Easy;
    QRadioButton *Medium;
    QRadioButton *Hard;
    QLabel *Bomb;
    QMenuBar *menubar;
    QMenu *menuSnake_Game;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1241, 861);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        workArea = new QLabel(centralwidget);
        workArea->setObjectName("workArea");
        workArea->setGeometry(QRect(20, 20, 901, 751));
        workArea->setFrameShape(QFrame::Shape::Box);
        New_Game = new QPushButton(centralwidget);
        New_Game->setObjectName("New_Game");
        New_Game->setGeometry(QRect(1030, 50, 141, 29));
        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        font.setUnderline(true);
        New_Game->setFont(font);
        New_Game->setMouseTracking(false);
        Prompt = new QLabel(centralwidget);
        Prompt->setObjectName("Prompt");
        Prompt->setGeometry(QRect(1030, 100, 181, 20));
        Score = new QLabel(centralwidget);
        Score->setObjectName("Score");
        Score->setGeometry(QRect(1030, 160, 181, 20));
        Stopwatch = new QLabel(centralwidget);
        Stopwatch->setObjectName("Stopwatch");
        Stopwatch->setGeometry(QRect(1030, 270, 181, 20));
        Mode = new QGroupBox(centralwidget);
        Mode->setObjectName("Mode");
        Mode->setGeometry(QRect(1010, 350, 140, 143));
        verticalLayout = new QVBoxLayout(Mode);
        verticalLayout->setObjectName("verticalLayout");
        Mode_1 = new QRadioButton(Mode);
        Mode_1->setObjectName("Mode_1");

        verticalLayout->addWidget(Mode_1);

        Mode_2 = new QRadioButton(Mode);
        Mode_2->setObjectName("Mode_2");

        verticalLayout->addWidget(Mode_2);

        Mode_3 = new QRadioButton(Mode);
        Mode_3->setObjectName("Mode_3");

        verticalLayout->addWidget(Mode_3);

        Difficulty = new QGroupBox(centralwidget);
        Difficulty->setObjectName("Difficulty");
        Difficulty->setGeometry(QRect(1030, 520, 140, 143));
        verticalLayout_2 = new QVBoxLayout(Difficulty);
        verticalLayout_2->setObjectName("verticalLayout_2");
        Easy = new QRadioButton(Difficulty);
        Easy->setObjectName("Easy");

        verticalLayout_2->addWidget(Easy);

        Medium = new QRadioButton(Difficulty);
        Medium->setObjectName("Medium");

        verticalLayout_2->addWidget(Medium);

        Hard = new QRadioButton(Difficulty);
        Hard->setObjectName("Hard");

        verticalLayout_2->addWidget(Hard);

        Bomb = new QLabel(centralwidget);
        Bomb->setObjectName("Bomb");
        Bomb->setGeometry(QRect(1020, 220, 151, 20));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1241, 25));
        menuSnake_Game = new QMenu(menubar);
        menuSnake_Game->setObjectName("menuSnake_Game");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuSnake_Game->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        workArea->setText(QString());
        New_Game->setText(QCoreApplication::translate("MainWindow", "NEW GAME", nullptr));
        Prompt->setText(QCoreApplication::translate("MainWindow", "Start New Game", nullptr));
        Score->setText(QCoreApplication::translate("MainWindow", "Score", nullptr));
        Stopwatch->setText(QCoreApplication::translate("MainWindow", "Time", nullptr));
        Mode->setTitle(QCoreApplication::translate("MainWindow", "Mode", nullptr));
        Mode_1->setText(QCoreApplication::translate("MainWindow", "Unbound", nullptr));
        Mode_2->setText(QCoreApplication::translate("MainWindow", "Jailed", nullptr));
        Mode_3->setText(QCoreApplication::translate("MainWindow", "Trick O' Treat", nullptr));
        Difficulty->setTitle(QCoreApplication::translate("MainWindow", "Difficulty", nullptr));
        Easy->setText(QCoreApplication::translate("MainWindow", "Easy", nullptr));
        Medium->setText(QCoreApplication::translate("MainWindow", "Medium", nullptr));
        Hard->setText(QCoreApplication::translate("MainWindow", "Hard", nullptr));
        Bomb->setText(QCoreApplication::translate("MainWindow", "Bomb", nullptr));
        menuSnake_Game->setTitle(QCoreApplication::translate("MainWindow", "Snake Game", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
