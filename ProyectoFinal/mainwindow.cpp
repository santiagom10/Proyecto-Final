#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    personajeSeleccionado = ":/Imagenes/Skater.png";
    resize(1000, 600);
    setWindowTitle("Skate en el Universo de los Dulces");

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    // MENU PRINCIPAL
    menuPrincipal = new QWidget();
    menuPrincipal->setStyleSheet(
        "QWidget {"
        "border-image: url(:/Imagenes/Fondo_menu.png) 0 0 0 0 stretch stretch;"
        "}"
        );

    QVBoxLayout *layoutMenu = new QVBoxLayout();
    layoutMenu->setSpacing(20);
    layoutMenu->setAlignment(Qt::AlignCenter);

    QPushButton *btnJugar = new QPushButton();
    btnJugar->setFixedSize(400, 120);
    btnJugar->setStyleSheet(
        "QPushButton {"
        "border-image: url(:/Imagenes/Boton_inicio.png) 0 0 0 0 stretch stretch;"
        "background-color: transparent; border: none;"
        "}"
        );

    QPushButton *btnSalir = new QPushButton();
    btnSalir->setFixedSize(400, 120);
    btnSalir->setStyleSheet(
        "QPushButton {"
        "border-image: url(:/Imagenes/Boton_salir.png) 0 0 0 0 stretch stretch;"
        "background-color: transparent; border: none;"
        "}"
        );

    layoutMenu->addWidget(btnJugar);
    layoutMenu->addWidget(btnSalir);
    menuPrincipal->setLayout(layoutMenu);


    // MENU PERSONAJES
    menuPersonajes = new QWidget();
    menuPersonajes->setStyleSheet("background-color: #16213e;");

    QVBoxLayout *layoutPersonajes = new QVBoxLayout();
    layoutPersonajes->setSpacing(24);
    layoutPersonajes->setAlignment(Qt::AlignCenter);

    QLabel *tituloPersonajes = new QLabel("Elige tu personaje");
    tituloPersonajes->setStyleSheet(
        "color: white; font-size: 30px; font-weight: bold;"
        "background: transparent;"
        );
    tituloPersonajes->setAlignment(Qt::AlignCenter);

    QHBoxLayout *filaPersonajes = new QHBoxLayout();
    filaPersonajes->setSpacing(40);
    filaPersonajes->setAlignment(Qt::AlignCenter);

    auto crearTarjeta = [&](const QString &rutaSprite, const QString &nombre,
                            const QString &colorBorde) -> QPushButton*
    {
        QWidget      *tarjeta = new QWidget();
        QVBoxLayout  *ly      = new QVBoxLayout(tarjeta);
        ly->setAlignment(Qt::AlignCenter);
        ly->setSpacing(8);

        QPushButton *btn = new QPushButton();
        btn->setFixedSize(110, 110);

        QPixmap px(rutaSprite);
        if (!px.isNull()) {
            int fww = px.width() / 7;
            int fhh = px.height() / 2;
            QPixmap frame = px.copy(0, fhh, fww, fhh);
            btn->setIcon(QIcon(frame));
            btn->setIconSize(QSize(100, 100));
        } else {
            btn->setText(nombre.left(1));
            btn->setStyleSheet(
                QString("QPushButton { background-color: %1; color:white;"
                        "font-size:28px; border-radius:12px; }").arg(colorBorde)
                );
        }

        btn->setStyleSheet(
            QString("QPushButton {"
                    "background-color: #0f3460;"
                    "border-radius: 14px;"
                    "border: 3px solid %1;"
                    "}"
                    "QPushButton:hover {"
                    "background-color: %1;"
                    "}").arg(colorBorde)
            );

        QLabel *lbl = new QLabel(nombre);
        lbl->setStyleSheet(
            "color: white; font-size: 18px; font-weight: bold;"
            "background: transparent;"
            );
        lbl->setAlignment(Qt::AlignCenter);

        ly->addWidget(btn);
        ly->addWidget(lbl);
        filaPersonajes->addWidget(tarjeta);
        return btn;
    };

    QPushButton *btnSkater = crearTarjeta(":/Imagenes/Skater.png", "Skater",     "#2e86de");
    QPushButton *btnOzzy   = crearTarjeta(":/Imagenes/Ozzy.png",   "Ozzy",       "#8e44ad");
    QPushButton *btnP3     = crearTarjeta(":/Imagenes/Skater.png", "Misterioso", "#e67e22");

    QPushButton *volverPersonajes = new QPushButton("← Volver");
    volverPersonajes->setFixedSize(160, 46);
    volverPersonajes->setStyleSheet(
        "QPushButton { background-color: #576574; color:white;"
        "border-radius:8px; font-size:16px; }"
        "QPushButton:hover { background-color: #747d8c; }"
        );

    layoutPersonajes->addWidget(tituloPersonajes);
    layoutPersonajes->addLayout(filaPersonajes);
    layoutPersonajes->addWidget(volverPersonajes, 0, Qt::AlignCenter);
    menuPersonajes->setLayout(layoutPersonajes);


    // MENU NIVELES
    menuNiveles = new QWidget();
    menuNiveles->setStyleSheet("background-color: #1a1a2e;");

    QVBoxLayout *layoutNiveles = new QVBoxLayout();
    layoutNiveles->setSpacing(20);
    layoutNiveles->setAlignment(Qt::AlignCenter);

    QLabel *tituloNiveles = new QLabel("Elige un nivel");
    tituloNiveles->setStyleSheet(
        "color: white; font-size: 30px; font-weight: bold;"
        "background: transparent;"
        );
    tituloNiveles->setAlignment(Qt::AlignCenter);

    QPushButton *nivel1        = new QPushButton("Nivel 1 - Ruta de Chocolate");
    QPushButton *nivel2        = new QPushButton("Nivel 2 - Zona de Gelatina");
    QPushButton *volverNiveles = new QPushButton("← Volver");

    nivel1->setFixedSize(380, 90);
    nivel2->setFixedSize(380, 90);
    volverNiveles->setFixedSize(160, 46);

    nivel1->setStyleSheet(
        "QPushButton { background-color: #2e86de; color:white; font-size:20px;"
        "font-weight:bold; border-radius:12px; }"
        "QPushButton:hover { background-color: #54a0ff; }"
        );
    nivel2->setStyleSheet(
        "QPushButton { background-color: #10ac84; color:white; font-size:20px;"
        "font-weight:bold; border-radius:12px; }"
        "QPushButton:hover { background-color: #1dd1a1; }"
        );
    volverNiveles->setStyleSheet(
        "QPushButton { background-color: #576574; color:white; border-radius:8px; font-size:16px; }"
        "QPushButton:hover { background-color: #747d8c; }"
        );

    layoutNiveles->addWidget(tituloNiveles);
    layoutNiveles->addWidget(nivel1);
    layoutNiveles->addWidget(nivel2);
    layoutNiveles->addWidget(volverNiveles, 0, Qt::AlignCenter);
    menuNiveles->setLayout(layoutNiveles);


    // PANTALLA DE JUEGO
    pantallaJuego = new QWidget();

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 500);
    scene->setBackgroundBrush(QColor(100, 180, 100));

    view = new QGraphicsView(scene);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    view->setFixedSize(800, 500);
    view->setSceneRect(0, 0, 800, 500);

    player = new Jugador();
    player->setSueloY(260);
    scene->addItem(player);
    player->setPos(100, 260);
    player->cargarSprite(personajeSeleccionado);

    QVBoxLayout *layoutJuego = new QVBoxLayout();
    layoutJuego->setAlignment(Qt::AlignCenter);
    layoutJuego->setContentsMargins(0, 0, 0, 0);
    layoutJuego->addWidget(view);
    pantallaJuego->setLayout(layoutJuego);
    pantallaJuego->setStyleSheet("background-color: #1a1a2e;");


    // STACK
    stack->addWidget(menuPrincipal);
    stack->addWidget(menuPersonajes);
    stack->addWidget(menuNiveles);
    stack->addWidget(pantallaJuego);
    stack->setCurrentWidget(menuPrincipal);


    // TIMERS
    timerJuego = new QTimer(this);
    timerJuego->setInterval(16);
    connect(timerJuego, &QTimer::timeout, this, &MainWindow::tickJuego);

    timerAnim = new QTimer(this);
    timerAnim->setInterval(100);
    connect(timerAnim, &QTimer::timeout, this, [this](){
        if (stack->currentWidget() == pantallaJuego)
            player->actualizarSprite();
    });


    // CONEXIONES
    connect(btnJugar, &QPushButton::clicked, [=](){
        stack->setCurrentWidget(menuPersonajes);
    });

    connect(btnSalir, &QPushButton::clicked, [=](){ close(); });

    connect(volverPersonajes, &QPushButton::clicked, [=](){
        stack->setCurrentWidget(menuPrincipal);
    });

    connect(volverNiveles, &QPushButton::clicked, [=](){
        stack->setCurrentWidget(menuPersonajes);
    });

    connect(btnSkater, &QPushButton::clicked, [=](){
        personajeSeleccionado = ":/Imagenes/Skater.png";
        stack->setCurrentWidget(menuNiveles);
    });
    connect(btnOzzy, &QPushButton::clicked, [=](){
        personajeSeleccionado = ":/Imagenes/Ozzy.png";
        stack->setCurrentWidget(menuNiveles);
    });
    connect(btnP3, &QPushButton::clicked, [=](){
        personajeSeleccionado = ":/Imagenes/Skater.png";
        stack->setCurrentWidget(menuNiveles);
    });

    connect(nivel1, &QPushButton::clicked, [=](){ iniciarNivel(1); });
    connect(nivel2, &QPushButton::clicked, [=](){ iniciarNivel(2); });
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::iniciarNivel(int num)
{
    player->cargarSprite(personajeSeleccionado);
    player->setPos(100, 260);
    player->detenerCaida();

    QString rutaFondo = (num == 1) ? ":/Imagenes/FondoN1.png"
                                   : ":/Imagenes/FondoN1.png";
    bgPixmap = QPixmap(rutaFondo);
    if (bgPixmap.isNull()) {
        bgPixmap = QPixmap(800, 500);
        bgPixmap.fill(num == 1 ? QColor(100, 180, 100) : QColor(60, 200, 160));
    } else {
        bgPixmap = bgPixmap.scaled(
            800, 500,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );
    }
    bgOffset = 0.0;
    scene->setBackgroundBrush(bgPixmap);

    stack->setCurrentWidget(pantallaJuego);
    setFocus();

    if (!ia) {
        ia = new IAObstaculos(scene, player, this);
        connect(ia, &IAObstaculos::jugadorMurio, [this](){
            timerJuego->stop();
            timerAnim->stop();
        });
        connect(ia, &IAObstaculos::nivelCompletado, [this](){
            timerJuego->stop();
            timerAnim->stop();
        });
    }
    ia->iniciar(60);

    timerJuego->start();
    timerAnim->start();
}


void MainWindow::tickJuego()
{
    if (stack->currentWidget() != pantallaJuego) return;

    if (ia) ia->actualizar();

    bool moviendose = false;

    if (teclasActivas.contains(Qt::Key_A) ||
        teclasActivas.contains(Qt::Key_Left))
    {
        player->moverIzquierda();
        moviendose = true;
    }
    else if (teclasActivas.contains(Qt::Key_D) ||
             teclasActivas.contains(Qt::Key_Right))
    {
        player->moverDerecha();
        moviendose = true;
    }

    if (!moviendose)
        player->detenerHorizontal();

    // Scroll del fondo
    bgOffset -= VEL_FONDO;
    if (bgOffset <= -800) bgOffset += 800;

    QPixmap canvas(800, 500);
    QPainter painter(&canvas);
    painter.drawPixmap(bgOffset, 0, bgPixmap);
    painter.drawPixmap(bgOffset + 800, 0, bgPixmap);
    painter.end();
    scene->setBackgroundBrush(canvas);

    // Física del jugador
    player->aplicarFisica();
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape &&
        stack->currentWidget() == pantallaJuego)
    {
        timerJuego->stop();
        timerAnim->stop();
        if (ia) ia->detener();
        stack->setCurrentWidget(menuPrincipal);
        return;
    }

    if (!event->isAutoRepeat() &&
        (event->key() == Qt::Key_W     ||
         event->key() == Qt::Key_Up    ||
         event->key() == Qt::Key_Space))
    {
        if (stack->currentWidget() == pantallaJuego)
            player->saltar();
    }

    teclasActivas.insert(event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    teclasActivas.remove(event->key());
}
