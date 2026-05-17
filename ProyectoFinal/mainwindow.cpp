#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Tamaño de ventana
    resize(800, 600);

    // crear escena
    scene = new QGraphicsScene(this);

    QPixmap background(":/Imagenes/Escenario1.png");

    if(background.isNull())
    {
        qDebug() << "La imagen NO se encontro";
    }
    else
    {
        qDebug() << "Imagen cargada correctamente";
    }

    scene->setBackgroundBrush(background);

    // Tamaño del mundo
    scene->setSceneRect(0, 0, 800, 600);

    // Crear vista
    QGraphicsView *view = new QGraphicsView(scene, this);

    // Mostrar la escena
    setCentralWidget(view);

    // Crear imagen temporal del jugador
    QPixmap pixmap(50, 50);

    pixmap.fill(Qt::blue);

    // Crear jugador
    player = scene->addPixmap(pixmap);

    // Posición inicial
    player->setPos(100, 100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int speed = 10;

    // Mover arriba
    if(event->key() == Qt::Key_W)
    {
        player->moveBy(0, -speed);
    }

    // Mover abajo
    if(event->key() == Qt::Key_S)
    {
        player->moveBy(0, speed);
    }

    // Mover izquierda
    if(event->key() == Qt::Key_A)
    {
        player->moveBy(-speed, 0);
    }

    // Mover derecha
    if(event->key() == Qt::Key_D)
    {
        player->moveBy(speed, 0);
    }
}
