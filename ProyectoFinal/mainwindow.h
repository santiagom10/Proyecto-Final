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
#include <QResizeEvent>
#include <QSet>
#include "jugador.h"
#include "obstaculo.h"
#include "iaobstaculos.h"
#include "dronia.h"
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
    void keyPressEvent(QKeyEvent *event)     override;
    void keyReleaseEvent(QKeyEvent *event)   override;
    void resizeEvent(QResizeEvent *event)    override;

private slots:
    void tickJuego();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
    QGraphicsView  *view;
    Jugador        *player;

    QTimer    *timerJuego;
    QTimer    *timerAnim;
    QSet<int>  teclasActivas;

    QString personajeSeleccionado;
    int     nivelActual   = 1;
    int     dificultadSel = 1;

    QStackedWidget *stack;
    QWidget        *menuPrincipal;
    QWidget        *menuPersonajes;
    QWidget        *menuDificultad;
    QWidget        *menuNiveles;
    QWidget        *pantallaJuego;

    std::vector<Obstaculo*> obstaculosNivel1;

    QPixmap     bgPixmap;
    qreal       bgOffset  = 0.0;
    const qreal VEL_FONDO = 2.0;

    // Agente Nivel 1
    IAObstaculos *ia   = nullptr;

    // Agente Nivel 2
    DronIA       *dron = nullptr;

    void iniciarNivel(int num);
    void reiniciarNivelActual();
    void volverAlMenu();
    void ajustarEscalaVista();

    // Helpers de construcción de UI
    QWidget* crearMenuPrincipal();
    QWidget* crearMenuPersonajes();
    QWidget* crearMenuDificultad();
    QWidget* crearMenuNiveles();
    QWidget* crearPantallaJuego();
};

#endif
