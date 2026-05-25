#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    QGraphicsPixmapItem *player;

    QStackedWidget *stack;

    QWidget *menuPrincipal;
    QWidget *menuNiveles;
    QWidget *menuPersonajes;
    QWidget *pantallaJuego;
};

#endif
