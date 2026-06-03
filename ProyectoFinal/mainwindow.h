#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QStackedWidget>
#include <QKeyEvent>
#include <QSet>
#include "jugador.h"
#include "obstaculo.h"
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void tickJuego();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
    QGraphicsView  *view;
    Jugador        *player;

    QTimer *timerJuego;
    QTimer *timerAnim;

    QSet<int> teclasActivas;

    QString        personajeSeleccionado;
    QStackedWidget *stack;
    QWidget        *menuPrincipal;
    QWidget        *menuPersonajes;
    QWidget        *menuNiveles;
    QWidget        *pantallaJuego;

    std::vector<Obstaculo*> obstaculosNivel1;

    void iniciarNivel(int num);
};

#endif
