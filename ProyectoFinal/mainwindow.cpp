#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QTransform>
#include <QMessageBox>
#include <stdexcept>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    personajeSeleccionado = ":/Imagenes/Skater.png";
    showMaximized();
    setWindowTitle("Skate en el Universo de los Dulces");

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    menuPrincipal  = crearMenuPrincipal();
    menuPersonajes = crearMenuPersonajes();
    menuDificultad = crearMenuDificultad();
    menuNiveles    = crearMenuNiveles();
    pantallaJuego  = crearPantallaJuego();

    stack->addWidget(menuPrincipal);
    stack->addWidget(menuPersonajes);
    stack->addWidget(menuDificultad);
    stack->addWidget(menuNiveles);
    stack->addWidget(pantallaJuego);
    stack->setCurrentWidget(menuPrincipal);

    connect(stack, &QStackedWidget::currentChanged, this, [this](int){
        if (stack->currentWidget() == pantallaJuego)
            ajustarEscalaVista();
    });

    timerJuego = new QTimer(this);
    timerJuego->setInterval(16);
    connect(timerJuego, &QTimer::timeout, this, &MainWindow::tickJuego);

    timerAnim = new QTimer(this);
    timerAnim->setInterval(100);
    connect(timerAnim, &QTimer::timeout, this, [this](){
        if (stack->currentWidget() == pantallaJuego)
            player->actualizarSprite();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ajustarEscalaVista()
{
    if (!view || !pantallaJuego) return;

    int wDisp = pantallaJuego->width();
    int hDisp = pantallaJuego->height();
    if (wDisp <= 0 || hDisp <= 0) return;

    double escalaX = wDisp  / 800.0;
    double escalaY = hDisp  / 500.0;
    double escala  = qMin(escalaX, escalaY);

    view->setFixedSize(qRound(800 * escala), qRound(500 * escala));
    view->setTransform(QTransform::fromScale(escala, escala));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (stack && stack->currentWidget() == pantallaJuego)
        ajustarEscalaVista();
}

QWidget* MainWindow::crearMenuPrincipal()
{
    QWidget *w = new QWidget();
    w->setStyleSheet("QWidget { border-image: url(:/Imagenes/Fondo_menu.png) 0 0 0 0 stretch stretch; }");

    QVBoxLayout *ly = new QVBoxLayout();
    ly->setSpacing(20);
    ly->setAlignment(Qt::AlignCenter);

    QPushButton *btnJugar = new QPushButton();
    btnJugar->setFixedSize(400, 120);
    btnJugar->setStyleSheet(
        "QPushButton { border-image: url(:/Imagenes/Boton_inicio.png) 0 0 0 0 stretch stretch;"
        "background-color: transparent; border: none; }"
        );

    QPushButton *btnSalir = new QPushButton();
    btnSalir->setFixedSize(400, 120);
    btnSalir->setStyleSheet(
        "QPushButton { border-image: url(:/Imagenes/Boton_salir.png) 0 0 0 0 stretch stretch;"
        "background-color: transparent; border: none; }"
        );

    ly->addWidget(btnJugar);
    ly->addWidget(btnSalir);
    w->setLayout(ly);

    connect(btnJugar, &QPushButton::clicked, [=](){ stack->setCurrentWidget(menuPersonajes); });
    connect(btnSalir, &QPushButton::clicked, [=](){ close(); });

    return w;
}

QWidget* MainWindow::crearMenuPersonajes()
{
    QWidget *w = new QWidget();
    w->setStyleSheet("background-color: #16213e;");

    QVBoxLayout *ly = new QVBoxLayout();
    ly->setSpacing(24);
    ly->setAlignment(Qt::AlignCenter);

    QLabel *titulo = new QLabel("Elige tu personaje");
    titulo->setStyleSheet("color: white; font-size: 30px; font-weight: bold; background: transparent;");
    titulo->setAlignment(Qt::AlignCenter);

    QHBoxLayout *fila = new QHBoxLayout();
    fila->setSpacing(40);
    fila->setAlignment(Qt::AlignCenter);

    auto crearTarjeta = [&](const QString &ruta, const QString &nombre,
                            const QString &color) -> QPushButton*
    {
        QWidget     *tarjeta = new QWidget();
        QVBoxLayout *tly     = new QVBoxLayout(tarjeta);
        tly->setAlignment(Qt::AlignCenter);
        tly->setSpacing(8);

        QPushButton *btn = new QPushButton();
        btn->setFixedSize(110, 110);
        QPixmap px(ruta);
        if (!px.isNull()) {
            int fw = px.width() / 7;
            int fh = px.height() / 2;
            btn->setIcon(QIcon(px.copy(0, fh, fw, fh)));
            btn->setIconSize(QSize(100, 100));
        }
        btn->setStyleSheet(
            QString("QPushButton { background-color:#0f3460; border-radius:14px; border:3px solid %1; }"
                    "QPushButton:hover { background-color:%1; }").arg(color)
            );

        QLabel *lbl = new QLabel(nombre);
        lbl->setStyleSheet("color:white; font-size:18px; font-weight:bold; background:transparent;");
        lbl->setAlignment(Qt::AlignCenter);

        tly->addWidget(btn);
        tly->addWidget(lbl);
        fila->addWidget(tarjeta);
        return btn;
    };

    QPushButton *btnSkater = crearTarjeta(":/Imagenes/Skater.png", "Skater",     "#2e86de");
    QPushButton *btnOzzy   = crearTarjeta(":/Imagenes/Ozzy.png",   "Ozzy",       "#8e44ad");
    QPushButton *btnP3     = crearTarjeta(":/Imagenes/Skater.png", "Misterioso", "#e67e22");

    QPushButton *volver = new QPushButton("← Volver");
    volver->setFixedSize(160, 46);
    volver->setStyleSheet(
        "QPushButton { background-color:#576574; color:white; border-radius:8px; font-size:16px; }"
        "QPushButton:hover { background-color:#747d8c; }"
        );

    ly->addWidget(titulo);
    ly->addLayout(fila);
    ly->addWidget(volver, 0, Qt::AlignCenter);
    w->setLayout(ly);

    connect(btnSkater, &QPushButton::clicked, [=](){ personajeSeleccionado = ":/Imagenes/Skater.png"; stack->setCurrentWidget(menuDificultad); });
    connect(btnOzzy,   &QPushButton::clicked, [=](){ personajeSeleccionado = ":/Imagenes/Ozzy.png";   stack->setCurrentWidget(menuDificultad); });
    connect(btnP3,     &QPushButton::clicked, [=](){ personajeSeleccionado = ":/Imagenes/Skater.png"; stack->setCurrentWidget(menuDificultad); });
    connect(volver,    &QPushButton::clicked, [=](){ stack->setCurrentWidget(menuPrincipal); });

    return w;
}

QWidget* MainWindow::crearMenuDificultad()
{
    QWidget *w = new QWidget();
    w->setStyleSheet("background-color: #1a1a2e;");

    QVBoxLayout *ly = new QVBoxLayout();
    ly->setSpacing(22);
    ly->setAlignment(Qt::AlignCenter);

    QLabel *titulo = new QLabel("Elige la dificultad");
    titulo->setStyleSheet("color:white; font-size:30px; font-weight:bold; background:transparent;");
    titulo->setAlignment(Qt::AlignCenter);

    struct DifInfo { QString nombre; QString desc; QString color; int val; };
    QVector<DifInfo> difs = {
                             { "🍬 Fácil",   "Solo obstáculos lineales • Muchos coleccionables", "#27ae60", 1 },
                             { "🍪 Normal",  "Obstáculos parabólicos • Dificultad progresiva",   "#2e86de", 2 },
                             { "🔥 Difícil", "Las 3 físicas • IA aprende de tus movimientos",    "#e74c3c", 3 },
                             };

    for (auto &d : difs) {
        QPushButton *btn = new QPushButton();
        btn->setFixedSize(480, 80);

        QVBoxLayout *bly = new QVBoxLayout(btn);
        bly->setSpacing(2);
        bly->setAlignment(Qt::AlignCenter);

        QLabel *lNombre = new QLabel(d.nombre);
        lNombre->setStyleSheet("color:white; font-size:20px; font-weight:bold; background:transparent;");
        lNombre->setAlignment(Qt::AlignCenter);

        QLabel *lDesc = new QLabel(d.desc);
        lDesc->setStyleSheet("color:#bdc3c7; font-size:12px; background:transparent;");
        lDesc->setAlignment(Qt::AlignCenter);

        bly->addWidget(lNombre);
        bly->addWidget(lDesc);

        btn->setStyleSheet(
            QString("QPushButton { background-color:%1; border-radius:12px; border:none; }"
                    "QPushButton:hover { background-color:%1; opacity:0.8; border: 2px solid white; }").arg(d.color)
            );

        int val = d.val;
        connect(btn, &QPushButton::clicked, [=](){ dificultadSel = val; stack->setCurrentWidget(menuNiveles); });
        ly->addWidget(btn);
    }

    QPushButton *volver = new QPushButton("← Volver");
    volver->setFixedSize(160, 46);
    volver->setStyleSheet(
        "QPushButton { background-color:#576574; color:white; border-radius:8px; font-size:16px; }"
        "QPushButton:hover { background-color:#747d8c; }"
        );
    connect(volver, &QPushButton::clicked, [=](){ stack->setCurrentWidget(menuPersonajes); });

    ly->insertWidget(0, titulo);
    ly->addWidget(volver, 0, Qt::AlignCenter);
    w->setLayout(ly);
    return w;
}

QWidget* MainWindow::crearMenuNiveles()
{
    QWidget *w = new QWidget();
    w->setStyleSheet("background-color: #1a1a2e;");

    QVBoxLayout *ly = new QVBoxLayout();
    ly->setSpacing(20);
    ly->setAlignment(Qt::AlignCenter);

    QLabel *titulo = new QLabel("Elige un nivel");
    titulo->setStyleSheet("color:white; font-size:30px; font-weight:bold; background:transparent;");
    titulo->setAlignment(Qt::AlignCenter);

    QPushButton *n1  = new QPushButton("Nivel 1 - Ruta de Chocolate");
    QPushButton *n2  = new QPushButton("Nivel 2 - Zona de Gelatina");
    QPushButton *vol = new QPushButton("← Volver");

    n1->setFixedSize(380, 90);
    n2->setFixedSize(380, 90);
    vol->setFixedSize(160, 46);

    n1->setStyleSheet(
        "QPushButton { background-color:#2e86de; color:white; font-size:20px; font-weight:bold; border-radius:12px; }"
        "QPushButton:hover { background-color:#54a0ff; }"
        );
    n2->setStyleSheet(
        "QPushButton { background-color:#27ae60; color:white; font-size:18px; font-weight:bold; border-radius:12px; }"
        "QPushButton:hover { background-color:#2ecc71; }"
        );
    vol->setStyleSheet(
        "QPushButton { background-color:#576574; color:white; border-radius:8px; font-size:16px; }"
        "QPushButton:hover { background-color:#747d8c; }"
        );

    ly->addWidget(titulo);
    ly->addWidget(n1);
    ly->addWidget(n2);
    ly->addWidget(vol, 0, Qt::AlignCenter);
    w->setLayout(ly);

    connect(n1,  &QPushButton::clicked, [=](){ iniciarNivel(1); });
    connect(n2,  &QPushButton::clicked, [=](){ iniciarNivel(2); });
    connect(vol, &QPushButton::clicked, [=](){ stack->setCurrentWidget(menuDificultad); });

    return w;
}

QWidget* MainWindow::crearPantallaJuego()
{
    QWidget *w = new QWidget();

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 500);
    scene->setBackgroundBrush(QColor(100, 180, 100));

    view = new QGraphicsView(scene);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setSceneRect(0, 0, 800, 500);

    player = new Jugador();
    player->setSueloY(260);
    scene->addItem(player);
    player->setPos(100, 260);

    QVBoxLayout *ly = new QVBoxLayout();
    ly->setAlignment(Qt::AlignCenter);
    ly->setContentsMargins(0, 0, 0, 0);
    ly->addWidget(view);
    w->setLayout(ly);
    w->setStyleSheet("background-color: #1a1a2e;");

    return w;
}
void MainWindow::iniciarNivel(int num)
{
    nivelActual = num;

    player->reiniciar();

    timerJuego->stop();
    timerAnim->stop();

    if (ia)
    {
        ia->detener();
        delete ia;
        ia = nullptr;
    }

    if (dron)
    {
        dron->detener();
        delete dron;
        dron = nullptr;
    }

    QList<QGraphicsItem*> items = scene->items();

    for (QGraphicsItem* item : items)
    {
        if (item != player)
        {
            scene->removeItem(item);
            delete item;
        }
    }
    try
    {
        player->cargarSprite(personajeSeleccionado);
    }
    catch (const std::runtime_error &e)
    {
        QMessageBox::warning(
            this,
            "Error de sprite",
            QString(
                "No se pudo cargar el personaje:\n%1\n\n"
                "Se usará el personaje por defecto."
                ).arg(e.what())
            );

        try
        {
            player->cargarSprite(":/Imagenes/Skater.png");
        }
        catch (...)
        {
            qDebug() << "No se pudo cargar ningún sprite.";
        }
    }
    if (nivelActual == 1)
    {
        player->setSueloY(380);
        player->setPos(100, 380);

        bgPixmap = QPixmap(":/Imagenes/FondoN1.png");

        if (bgPixmap.isNull())
        {
            bgPixmap = QPixmap(800, 500);
            bgPixmap.fill(QColor(100, 180, 100));
        }
        else
        {
            bgPixmap = bgPixmap.scaled(
                800,
                500,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
        }

        bgOffset = 0.0;
        scene->setBackgroundBrush(bgPixmap);

        stack->setCurrentWidget(pantallaJuego);
        setFocus();

        ia = new IAObstaculos(scene, player, this);

        connect(
            ia,
            &IAObstaculos::jugadorMurio,
            [this]()
            {
                timerJuego->stop();
                timerAnim->stop();
            });

        connect(
            ia,
            &IAObstaculos::nivelCompletado,
            [this]()
            {
                timerJuego->stop();
                timerAnim->stop();
            });

        ia->setDificultadInicial(dificultadSel);

        try
        {
            ia->iniciar(60);
        }
        catch (const std::runtime_error &e)
        {
            QMessageBox::critical(
                this,
                "Error al iniciar nivel",
                QString(
                    "No se pudo iniciar el nivel:\n%1"
                    ).arg(e.what())
                );

            volverAlMenu();
            return;
        }

        timerJuego->start();
        timerAnim->start();
    }
    else if (nivelActual == 2)
    {
        player->setSueloY(9999);
        player->setPos(380, 280);

        bgPixmap = QPixmap(":/Imagenes/FondoN2.png");

        if (bgPixmap.isNull())
        {
            bgPixmap = QPixmap(800, 500);
            bgPixmap.fill(QColor(30, 140, 100));
        }
        else
        {
            bgPixmap = bgPixmap.scaled(
                800,
                500,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
        }

        bgOffset = 0.0;
        scene->setBackgroundBrush(bgPixmap);

        stack->setCurrentWidget(pantallaJuego);
        setFocus();

        dron = new DronIA(
            scene,
            player,
            dificultadSel,
            this
            );

        connect(
            dron,
            &DronIA::jugadorMurio,
            [this]()
            {
                timerJuego->stop();
                timerAnim->stop();
            });

        connect(
            dron,
            &DronIA::nivelCompletado,
            [this]()
            {
                timerJuego->stop();
                timerAnim->stop();
            });

        dron->setDificultad(dificultadSel);
        dron->iniciar();

        timerJuego->start();
        timerAnim->start();
    }
}

void MainWindow::reiniciarNivelActual()
{
    timerJuego->stop();
    timerAnim->stop();
    if (ia   && nivelActual == 1) ia->detener();
    if (dron && nivelActual == 2) dron->detener();

    iniciarNivel(nivelActual);
}

void MainWindow::volverAlMenu()
{
    timerJuego->stop();
    timerAnim->stop();
    if (ia)   ia->detener();
    if (dron) dron->detener();
    stack->setCurrentWidget(menuPrincipal);
}

void MainWindow::tickJuego()
{
    if (stack->currentWidget() != pantallaJuego) return;

    // ── Actualizar agente del nivel activo ───────────────────
    if (nivelActual == 1 && ia)   ia->actualizar();
    if (nivelActual == 2 && dron) dron->actualizar();

    // ── Factor de velocidad: reducido si el jugador está en charco ──
    const qreal VEL_BASE   = 6.0;
    qreal factorCharco     = (nivelActual == 2 && dron)
                             ? dron->factorVelocidadJugador()
                             : 1.0;
    qreal velH = VEL_BASE * factorCharco;

    // ── Movimiento horizontal ────────────────────────────────
    bool moviendose = false;

    if (teclasActivas.contains(Qt::Key_A) || teclasActivas.contains(Qt::Key_Left)) {
        if (nivelActual == 2) {
            // Vista cenital: mover directamente con setX
            qreal nx = player->x() - velH;
            if (nx >= 10) player->setX(nx);
        } else {
            player->moverIzquierda();
        }
        moviendose = true;
    } else if (teclasActivas.contains(Qt::Key_D) || teclasActivas.contains(Qt::Key_Right)) {
        if (nivelActual == 2) {
            qreal nx = player->x() + velH;
            if (nx <= 740) player->setX(nx);
        } else {
            player->moverDerecha();
        }
        moviendose = true;
    }

    if (!moviendose && nivelActual == 1) player->detenerHorizontal();

    // ── Movimiento vertical libre (solo nivel 2 — vista cenital) ──
    if (nivelActual == 2) {
        qreal velV = VEL_BASE * factorCharco;
        if (teclasActivas.contains(Qt::Key_W) || teclasActivas.contains(Qt::Key_Up)) {
            qreal ny = player->y() - velV;
            if (ny >= 60) player->setY(ny);
        } else if (teclasActivas.contains(Qt::Key_S) || teclasActivas.contains(Qt::Key_Down)) {
            qreal ny = player->y() + velV;
            if (ny <= 430) player->setY(ny);
        }
    }

    // ── Dibujar fondo ────────────────────────────────────────
    if (nivelActual == 1) {
        // Scroll horizontal del fondo (nivel 1 sin cambios)
        bgOffset -= 2;
        if (bgOffset <= -800) bgOffset += 800;

        QPixmap canvas(800, 500);
        QPainter painter(&canvas);
        painter.drawPixmap(bgOffset, 0, bgPixmap);
        painter.drawPixmap(bgOffset + 800, 0, bgPixmap);
        painter.end();
        scene->setBackgroundBrush(canvas);
    }
    // Nivel 2: fondo estático (FondoN2.png asignado en iniciarNivel, no se redibuja)

    // ── Física del jugador ───────────────────────────────────
    // En nivel 2 no aplicamos física de gravedad (vista cenital)
    if (nivelActual == 1) {
        player->aplicarFisica();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (stack->currentWidget() == pantallaJuego) {
        if (event->key() == Qt::Key_Escape) { volverAlMenu(); return; }
        if (event->key() == Qt::Key_R && !event->isAutoRepeat()) { reiniciarNivelActual(); return; }

        // Salto solo en nivel 1
        if (nivelActual == 1 && !event->isAutoRepeat() &&
            (event->key() == Qt::Key_W || event->key() == Qt::Key_Up || event->key() == Qt::Key_Space))
        {
            player->saltar();
        }
    }
    teclasActivas.insert(event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    teclasActivas.remove(event->key());
}
