#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include "jugador.h"
#include "obstaculo.h"
#include <vector>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QKeyEvent>

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QStackedWidget>

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
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
    QGraphicsView *view;
    Jugador *player;
    QTimer *timerJuego;

    QStackedWidget *stack;
    QWidget *menuPrincipal;
    QWidget *menuNiveles;
    QWidget *menuPersonajes;
    QWidget *pantallaJuego;

    std::vector<Obstaculo*> obstaculosNivel1;
};

#endif
