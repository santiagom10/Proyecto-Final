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
#include "iaobstaculos.h"
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
    int            nivelActual    = 1;
    int            dificultadSel  = 1;   // 1=fácil 2=normal 3=difícil

    QStackedWidget *stack;
    QWidget        *menuPrincipal;
    QWidget        *menuPersonajes;
    QWidget        *menuDificultad;  // ← NUEVO
    QWidget        *menuNiveles;
    QWidget        *pantallaJuego;

    std::vector<Obstaculo*> obstaculosNivel1;

    QPixmap     bgPixmap;
    qreal       bgOffset  = 0.0;
    const qreal VEL_FONDO = 2.0;

    void iniciarNivel(int num);
    void reiniciarNivelActual();
    void volverAlMenu();

    IAObstaculos *ia = nullptr;

    // Helpers de construcción de UI
    QWidget* crearMenuPrincipal();
    QWidget* crearMenuPersonajes();
    QWidget* crearMenuDificultad();
    QWidget* crearMenuNiveles();
    QWidget* crearPantallaJuego();
};

#endif
