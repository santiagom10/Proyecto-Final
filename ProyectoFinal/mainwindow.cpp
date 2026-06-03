#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>

// ================================================================
//  Constructor
// ================================================================
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

    // ============================================================
    //  MENÚ PRINCIPAL  — solo 2 botones: JUGAR y SALIR
    //  Flujo: Jugar → elegir personaje → elegir nivel → jugar
    // ============================================================
    menuPrincipal = new QWidget();
    menuPrincipal->setStyleSheet(
        "QWidget {"
        "border-image: url(:/Imagenes/Fondo_menu.png) 0 0 0 0 stretch stretch;"
        "}"
        );

    QVBoxLayout *layoutMenu = new QVBoxLayout();
    layoutMenu->setSpacing(20);
    layoutMenu->setAlignment(Qt::AlignCenter);

    // Botón JUGAR — reutiliza la imagen de Boton_inicio.png
    QPushButton *btnJugar = new QPushButton();
    btnJugar->setFixedSize(400, 120);
    btnJugar->setStyleSheet(
        "QPushButton {"
        "border-image: url(:/Imagenes/Boton_inicio.png) 0 0 0 0 stretch stretch;"
        "background-color: transparent; border: none;"
        "}"
        );

    // Botón SALIR
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

    // ============================================================
    //  MENÚ PERSONAJES  (segunda pantalla después de Jugar)
    // ============================================================
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

    // Fila de personajes
    QHBoxLayout *filaPersonajes = new QHBoxLayout();
    filaPersonajes->setSpacing(40);
    filaPersonajes->setAlignment(Qt::AlignCenter);

    // ── Helper para crear cada tarjeta de personaje ──
    auto crearTarjeta = [&](const QString &rutaSprite,
                            const QString &nombre,
                            const QString &colorBorde) -> QPushButton*
    {
        QWidget      *tarjeta  = new QWidget();
        QVBoxLayout  *ly       = new QVBoxLayout(tarjeta);
        ly->setAlignment(Qt::AlignCenter);
        ly->setSpacing(8);

        QPushButton *btn = new QPushButton();
        btn->setFixedSize(110, 110);

        QPixmap px(rutaSprite);
        if (!px.isNull()) {
            int fww = px.width() / 7;
            int fhh = px.height() / 2;
            // Mostrar el frame de ride1 (fila 1, col 0) en la tarjeta
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

    QPushButton *btnSkater = crearTarjeta(":/Imagenes/Skater.png", "Skater",  "#2e86de");
    QPushButton *btnOzzy   = crearTarjeta(":/Imagenes/Ozzy.png",   "Ozzy",    "#8e44ad");
    // Personaje 3: reutiliza Skater hasta que tengan imagen propia
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

    // ============================================================
    //  MENÚ NIVELES  (tercera pantalla, tras elegir personaje)
    // ============================================================
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

    QPushButton *nivel1        = new QPushButton("Nivel 1 – Ruta de Chocolate");
    QPushButton *nivel2        = new QPushButton("Nivel 2 – Zona de Gelatina");
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

    // ============================================================
    //  PANTALLA DE JUEGO
    // ============================================================
    pantallaJuego = new QWidget();

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 500);

    QPixmap bg(":/Imagenes/Escenario1.png");
    if (!bg.isNull())
        scene->setBackgroundBrush(bg.scaled(800, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    else
        scene->setBackgroundBrush(QColor(100, 180, 100));

    view = new QGraphicsView(scene);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    view->setFixedSize(800, 500);
    view->setSceneRect(0, 0, 800, 500);

    player = new Jugador();
    player->setSueloY(320);
    scene->addItem(player);
    player->setPos(100, 320);
    player->cargarSprite(personajeSeleccionado);

    QVBoxLayout *layoutJuego = new QVBoxLayout();
    layoutJuego->setAlignment(Qt::AlignCenter);
    layoutJuego->setContentsMargins(0, 0, 0, 0);
    layoutJuego->addWidget(view);
    pantallaJuego->setLayout(layoutJuego);
    pantallaJuego->setStyleSheet("background-color: #1a1a2e;");

    // ────────────────────────────────────────────────────────────
    //  STACK
    // ────────────────────────────────────────────────────────────
    stack->addWidget(menuPrincipal);
    stack->addWidget(menuPersonajes);
    stack->addWidget(menuNiveles);
    stack->addWidget(pantallaJuego);
    stack->setCurrentWidget(menuPrincipal);

    // ────────────────────────────────────────────────────────────
    //  TIMERS
    // ────────────────────────────────────────────────────────────
    timerJuego = new QTimer(this);
    timerJuego->setInterval(16);   // ~60 fps
    connect(timerJuego, &QTimer::timeout, this, &MainWindow::tickJuego);

    // Timer de animación del sprite (~10 fps)
    timerAnim = new QTimer(this);
    timerAnim->setInterval(100);
    connect(timerAnim, &QTimer::timeout, this, [this](){
        if (stack->currentWidget() == pantallaJuego)
            player->actualizarSprite();
    });

    // ────────────────────────────────────────────────────────────
    //  CONEXIONES
    // ────────────────────────────────────────────────────────────

    // Jugar → personajes
    connect(btnJugar, &QPushButton::clicked, [=](){
        stack->setCurrentWidget(menuPersonajes);
    });

    // Salir
    connect(btnSalir, &QPushButton::clicked, [=](){ close(); });

    // Volver desde personajes → menú principal
    connect(volverPersonajes, &QPushButton::clicked, [=](){
        stack->setCurrentWidget(menuPrincipal);
    });

    // Volver desde niveles → personajes
    connect(volverNiveles, &QPushButton::clicked, [=](){
        stack->setCurrentWidget(menuPersonajes);
    });

    // Selección de personajes → ir a niveles
    connect(btnSkater, &QPushButton::clicked, [=](){
        personajeSeleccionado = ":/Imagenes/Skater.png";
        stack->setCurrentWidget(menuNiveles);
    });
    connect(btnOzzy, &QPushButton::clicked, [=](){
        personajeSeleccionado = ":/Imagenes/Ozzy.png";
        stack->setCurrentWidget(menuNiveles);
    });
    connect(btnP3, &QPushButton::clicked, [=](){
        personajeSeleccionado = ":/Imagenes/Skater.png"; // cambiar cuando tengan imagen
        stack->setCurrentWidget(menuNiveles);
    });

    // Iniciar niveles
    connect(nivel1, &QPushButton::clicked, [=](){ iniciarNivel(1); });
    connect(nivel2, &QPushButton::clicked, [=](){ iniciarNivel(2); });
}

// ================================================================
//  Destructor
// ================================================================
MainWindow::~MainWindow()
{
    delete ui;
}

// ================================================================
//  Inicia el nivel: carga personaje, fondo y arranca timers
// ================================================================
void MainWindow::iniciarNivel(int num)
{
    player->cargarSprite(personajeSeleccionado);
    player->setPos(100, 320);
    player->detenerCaida();

    if (num == 1) {
        QPixmap bg(":/Imagenes/Escenario1.png");
        scene->setBackgroundBrush(
            bg.isNull() ? QBrush(QColor(100,180,100))
                        : QBrush(bg.scaled(800, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))
            );
    } else {
        // Nivel 2: mismo fondo por ahora (agregar Escenario2.png cuando esté)
        QPixmap bg(":/Imagenes/Escenario1.png");
        scene->setBackgroundBrush(
            bg.isNull() ? QBrush(QColor(60,200,160))
                        : QBrush(bg.scaled(800, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))
            );
    }

    stack->setCurrentWidget(pantallaJuego);
    setFocus();
    timerJuego->start();
    timerAnim->start();
}

// ================================================================
//  Tick principal: lee teclas activas y aplica física
// ================================================================
void MainWindow::tickJuego()
{
    if (stack->currentWidget() != pantallaJuego) return;

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

    // Colisiones con obstáculos antes de aplicar física
    qreal ox = player->x();
    qreal oy = player->y();

    player->aplicarFisica();

    QList<QGraphicsItem*> cols = player->collidingItems();
    if (!cols.isEmpty()) {
        player->setPos(ox, oy);
        player->detenerCaida();
    }
}

// ================================================================
//  Teclado
// ================================================================
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // ESC → volver al menú
    if (event->key() == Qt::Key_Escape &&
        stack->currentWidget() == pantallaJuego)
    {
        timerJuego->stop();
        timerAnim->stop();
        stack->setCurrentWidget(menuPrincipal);
        return;
    }

    // Salto (una sola vez, sin autorepeat)
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
