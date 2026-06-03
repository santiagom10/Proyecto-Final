#include "mainwindow.h"
#include "ui_mainwindow.h"

// Libreria para mensajes en consola
// Para pruebas
#include <QDebug>


// Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Inicializar interfaz
    ui->setupUi(this);

    personajeSeleccionado = ":/Imagenes/skater.png";

    // Tamaño de la ventana
    resize(1000,500);

    // Stack para cambiar pantallas
    /*
       - Menu principal
       - Menu niveles
       - Menu personajes
       - Juego
    */

    stack = new QStackedWidget(this);

    // Colocar stack como widget principal
    // para colocar el menu en el centro de la pantalla
    setCentralWidget(stack);


    // Crea la ventana del menu
    menuPrincipal = new QWidget();

    // FONDO DEL MENU PRINCIPAL
    menuPrincipal->setStyleSheet(
        "QWidget {"
        "border-image: url(:/Imagenes/Fondo_menu.png) 0 0 0 0 stretch stretch;"
        "}"
        );


    // Layout vertical
    // Para acomodar automaticamente los botones
    QVBoxLayout *layoutMenu = new QVBoxLayout();


    // Boton inicio
    QPushButton *btnInicio = new QPushButton();

    // Tamaño del boton
    btnInicio->setFixedSize(400,120);

    // Imagen del boton
    btnInicio->setStyleSheet(

        "QPushButton {"

        "border-image: url(:/Imagenes/Boton_inicio.png) 0 0 0 0 stretch stretch;"
        "background-color: transparent;"
        "border: none;"

        "}"

        );


    // BOTON PERSONAJES
    QPushButton *btnPersonajes = new QPushButton();

    // Tamaño del boton
    btnPersonajes->setFixedSize(400,120);

    // Imagen del boton
    btnPersonajes->setStyleSheet(

        "QPushButton {"

        "border-image: url(:/Imagenes/Boton_personajes.png) 0 0 0 0 stretch stretch;"
        "background-color: transparent;"
        "border: none;"

        "}"

        );


    // Boton salir
    QPushButton *btnSalir = new QPushButton();

    // Tamaño del boton
    btnSalir->setFixedSize(400,120);

    // Imagen del boton
    btnSalir->setStyleSheet(

        "QPushButton {"

        "border-image: url(:/Imagenes/Boton_salir.png) 0 0 0 0 stretch stretch;"
        "background-color: transparent;"
        "border: none;"

        "}"

        );


    // Agrega botones al layout
    layoutMenu->addWidget(btnInicio);
    layoutMenu->addWidget(btnPersonajes);
    layoutMenu->addWidget(btnSalir);

    // Espacio entre botones
    layoutMenu->setSpacing(30);

    // Centrar botones
    layoutMenu->setAlignment(Qt::AlignCenter);

    // Asignar layout
    menuPrincipal->setLayout(layoutMenu);



    // Menu niveles
    /*
        Crea una nueva pantalla vacía.

        Esa pantalla será:
        menuNiveles
            Nivel 1
            Nivel 2
    */

    menuNiveles = new QWidget();


    /* Esto crea un organizador vertical.

        Organizar automáticamente los botones:
        Nivel 1
        Nivel 2
        uno debajo del otro.
    */
    QVBoxLayout *layoutNiveles = new QVBoxLayout();



    // BOTON NIVEL 1

    QPushButton *nivel1 =
        new QPushButton("Nivel 1");

    // Tamaño
    nivel1->setFixedSize(300,100);

    // Color visible
    nivel1->setStyleSheet(

        "QPushButton {"

        "background-color: blue;"
        "color: white;"
        "font-size: 30px;"

        "}"

        );


    // BOTON NIVEL 2

    QPushButton *nivel2 =
        new QPushButton("Nivel 2");

    nivel2->setFixedSize(300,100);

    nivel2->setStyleSheet(

        "QPushButton {"

        "background-color: green;"
        "color: white;"
        "font-size: 30px;"

        "}"

        );


    // BOTON VOLVER
    QPushButton *volverNiveles = new QPushButton("Volver");

    volverNiveles->setFixedSize(300,100);


    // Agregar botones
    layoutNiveles->addWidget(nivel1);
    layoutNiveles->addWidget(nivel2);
    layoutNiveles->addWidget(volverNiveles);

    // Centrar
    layoutNiveles->setAlignment( Qt::AlignCenter );

    // Asignar layout
    menuNiveles->setLayout(layoutNiveles);


    // Menu personajes
    menuPersonajes = new QWidget();

    QVBoxLayout *layoutPersonajes = new QVBoxLayout();

    /*
    // Personaje 1
    QPushButton *personaje1 = new QPushButton();

    personaje1->setFixedSize(150,150);

    personaje1->setStyleSheet(

        "QPushButton {"

        "border-image: url(:/Imagenes/personaje1.png);"
        "background-color: transparent;"
        "border: none;"

        "}"

        );


    // Personaje 2
    QPushButton *personaje2 = new QPushButton();

    personaje2->setFixedSize(150,150);

    personaje2->setStyleSheet(

        "QPushButton {"

        "border-image: url(:/Imagenes/personaje2.png);"
        "background-color: transparent;"
        "border: none;"

        "}"

        );


    // Personaje 3
    QPushButton *personaje3 = new QPushButton();

    personaje3->setFixedSize(150,150);

    personaje3->setStyleSheet(

        "QPushButton {"

        "border-image: url(:/Imagenes/personaje3.png);"
        "background-color: transparent;"
        "border: none;"

        "}"

        );
    */

    // BOTONES DE PERSONAJES PROVISSIONALES HASTA QUE DECIDAMOS CUALES COLOCAREMOS

    // Personaje 1
    QPushButton *personaje1 =
        new QPushButton("Personaje 1");

    personaje1->setFixedSize(300,100);

    personaje1->setStyleSheet(

        "QPushButton {"

        "background-color: red;"
        "color: white;"
        "font-size: 25px;"

        "}"

        );

    // Personaje 2
    QPushButton *personaje2 =
        new QPushButton("Personaje 2");

    personaje2->setFixedSize(300,100);

    personaje2->setStyleSheet(

        "QPushButton {"

        "background-color: purple;"
        "color: white;"
        "font-size: 25px;"

        "}"

        );

    // Personaje 3
    QPushButton *personaje3 =
        new QPushButton("Personaje 3");

    personaje3->setFixedSize(300,100);

    personaje3->setStyleSheet(

        "QPushButton {"

        "background-color: orange;"
        "color: white;"
        "font-size: 25px;"

        "}"

        );


    // BOTON VOLVER

    QPushButton *volverPersonajes = new QPushButton("Volver");

    volverPersonajes->setFixedSize(300,100);



    // Agregar personajes
    layoutPersonajes->addWidget(personaje1);
    layoutPersonajes->addWidget(personaje2);
    layoutPersonajes->addWidget(personaje3);
    layoutPersonajes->addWidget(volverPersonajes);

    // Centrar
    layoutPersonajes->setAlignment( Qt::AlignCenter );

    // Asignar layout
    menuPersonajes->setLayout( layoutPersonajes );


    // Pantalla del juegoo
    pantallaJuego = new QWidget();


    // Crear escena
    scene = new QGraphicsScene(this);

    // Tamaño del mundo
    scene->setSceneRect(0,0,800,600);


    // Crear vista
    QGraphicsView *view = new QGraphicsView(scene);

    // Layout del juego
    QVBoxLayout *layoutJuego = new QVBoxLayout();

    // Agregar vista
    layoutJuego->addWidget(view);

    // Asignar layout
    pantallaJuego->setLayout(layoutJuego);


    // Fondo del juego
    QPixmap background( ":/Imagenes/Escenario1.png" );

    // Verificar carga
    if(background.isNull())
    {
        qDebug()
        << "La imagen NO se encontro";
    }
    else
    {
        qDebug()
        << "Imagen cargada correctamente";
    }

    // Colocar fondo
    scene->setBackgroundBrush(background);

    // Crear jugador
    player = new Jugador();

    // Cargar la imagen del personaje seleccionado por defecto
    QPixmap pixmap(personajeSeleccionado);

    if(pixmap.isNull())
    {
        qDebug() << "No se encontró la imagen inicial del jugador:" << personajeSeleccionado;
        // Solo si falla, ponemos un cuadrado azul
        QPixmap temp(50,50);
        temp.fill(Qt::blue);
        player->setPixmap(temp);
    }
    else
    {
        // Aplicamos el recorte
        int anchoFrame = pixmap.width() / 7;
        int altoFrame = pixmap.height() / 2;
        QPixmap idle = pixmap.copy(0, 0, anchoFrame, altoFrame);

        player->setPixmap(idle.scaled(90, 90, Qt::KeepAspectRatio, Qt::FastTransformation));



    }

    /*
    // Cargar imagen real (o temporal si no tienes)
    QPixmap pixmap(":/Imagenes/personaje1.png");

    if(pixmap.isNull())
    {
        qDebug() << "No se encontró la imagen del jugador";

        QPixmap temp(50,50);
        temp.fill(Qt::blue);
        player->setPixmap(temp);
    }
    else
    {
        player->setPixmap(pixmap.scaled(60,60));
    }
    */



    // Posición inicial
    player->setPos(100,320);

    // Agregar a la escena
    scene->addItem(player);

    // Posicion inicial
    player->setPos(100,320);


    // AGREGAR PANTALLAS AL STACK

    stack->addWidget(menuPrincipal);
    stack->addWidget(menuNiveles);
    stack->addWidget(menuPersonajes);
    stack->addWidget(pantallaJuego);

    // Mostrar menu principal
    stack->setCurrentWidget( menuPrincipal );


    // Boton inicio
    /*
    connect(btnInicio,
            &QPushButton::clicked,

            [=](){

                qDebug() << "CLICK EN INICIO";

            });
    */

    connect(btnInicio,
            &QPushButton::clicked,

            [=](){
                stack->setCurrentWidget(
                    menuNiveles
                    );

            });


    // Boton personajes
    connect(btnPersonajes,
            &QPushButton::clicked,

            [=](){
                stack->setCurrentWidget(
                    menuPersonajes
                    );

            });

    connect(volverPersonajes,
            &QPushButton::clicked,

            [=](){

                stack->setCurrentWidget(
                    menuPrincipal
                    );

            });


    // Nivel 1
    connect(nivel1,
            &QPushButton::clicked,

            [=]()
            {
                qDebug() << personajeSeleccionado;

                QPixmap sprite(personajeSeleccionado);

                qDebug() << sprite.isNull();

                if(!sprite.isNull())
                {
                    int anchoFrame =
                        sprite.width() / 7;

                    int altoFrame =
                        sprite.height() / 2;

                    QPixmap idle =
                        sprite.copy(
                            0,
                            0,
                            anchoFrame,
                            altoFrame
                            );

                    player->setPixmap(
                        idle.scaled(
                            90,
                            90,
                            Qt::KeepAspectRatio,
                            Qt::FastTransformation
                            )
                        );


                    // Para comprobar si carga la imagen
                    /*
                    player->setPixmap(
                        sprite.scaled(
                            200,
                            200,
                            Qt::KeepAspectRatio,
                            Qt::FastTransformation
                            )
                        );
                    */

                }
                else
                {
                    qDebug() << "No se encontro el sprite";
                }

                player->setPos(100,320);

                stack->setCurrentWidget(
                    pantallaJuego
                    );
            });

    // Nivel 2
    connect(nivel2,
            &QPushButton::clicked,

            [=]()
            {
                QPixmap sprite(personajeSeleccionado);

                if(!sprite.isNull())
                {
                    int anchoFrame =
                        sprite.width() / 7;

                    int altoFrame =
                        sprite.height() / 2;

                    QPixmap idle =
                        sprite.copy(
                            0,
                            0,
                            anchoFrame,
                            altoFrame
                            );

                    player->setPixmap(
                        idle.scaled(
                            90,
                            90,
                            Qt::KeepAspectRatio,
                            Qt::FastTransformation
                            )
                        );
                }
                else
                {
                    qDebug() << "No se encontro el sprite";
                }

                player->setPos(100,320);

                stack->setCurrentWidget(
                    pantallaJuego
                    );
            });


    connect(volverNiveles,
            &QPushButton::clicked,

            [=](){

                stack->setCurrentWidget(
                    menuPrincipal
                    );
            });

    // Seleccionar Skater
    connect(personaje1, &QPushButton::clicked, [=]() {
        // Revisa si tu imagen se llama Skater.png o skater.png en el archivo .qrc
        personajeSeleccionado = ":/Imagenes/Skater.png";
        qDebug() << "Skater seleccionado";

        stack->setCurrentWidget(menuNiveles);
    });

    // Seleccionar Ozzy
    connect(personaje2, &QPushButton::clicked, [=]() {
        // Revisa si tu imagen se llama Ozzy.png o ozzy.png en el archivo .qrc
        personajeSeleccionado = ":/Imagenes/Ozzy.png";
        qDebug() << "Ozzy seleccionado";

        stack->setCurrentWidget(menuNiveles);
    });

}


// Dstructor
MainWindow::~MainWindow()
{
    delete ui;
}

// Movimiento del jugador
void MainWindow::keyPressEvent(
    QKeyEvent *event)
{
    // Velocidad jugador
    int speed = 10;

    // Guardar posicion anterior
    qreal oldX = player->x();

    qreal oldY = player->y();


    // Arriba
    if(event->key() == Qt::Key_W)
    {
        player->moveBy(0, -speed);
    }


    // Abajo
    if(event->key() == Qt::Key_S)
    {
        player->moveBy(0, speed);
    }


    // Izquierda
    if(event->key() == Qt::Key_A)
    {
        player->moveBy(-speed, 0);
    }


    // Derecha
    if(event->key() == Qt::Key_D)
    {
        player->moveBy(speed, 0);
    }


    // Detectar coliciones
    QList<QGraphicsItem*> collisions =
        player->collidingItems();

    // Si hay colision
    if(!collisions.isEmpty())
    {
        // Regresar posicion
        player->setPos(oldX, oldY);
    }
}
