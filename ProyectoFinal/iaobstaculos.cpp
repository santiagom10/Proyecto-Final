#include "iaobstaculos.h"
#include "obstaculo.h"
#include "jugador.h"
#include <QPixmap>
#include <QTimer>
#include <QFont>
#include <QDebug>
#include <algorithm>
#include <numeric>

// ─────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────
IAObstaculos::IAObstaculos(QGraphicsScene *scene, Jugador *jugador, QObject *parent)
    : QObject(parent),
    scene(scene),
    jugador(jugador),
    dificultadInicial(1),
    nivelDificultad(1),
    ticksTranscurridos(0),
    intervalMinimo(1200),
    intervalMaximo(3000),
    segundosRestantes(60),
    terminado(false),
    gano(false)
{
    timerDecision = new QTimer(this);
    connect(timerDecision, &QTimer::timeout, this, &IAObstaculos::decidirObstaculo);

    timerSegundo = new QTimer(this);
    timerSegundo->setInterval(1000);
    connect(timerSegundo, &QTimer::timeout, this, &IAObstaculos::tickSegundo);

    // Inicializar pesos de aprendizaje con valor neutro 1.0
    // Clave = TipoObst * 10 + ModeloFis
    for (int t = 0; t <= 2; t++)
        for (int m = 0; m <= 2; m++)
            pesosCombinacion[t * 10 + m] = 1.0;

    // Conectar señales del jugador al HUD
    connect(jugador, &Jugador::vidasCambiaron, this, [this](int v){
        if (textoVidas) textoVidas->setPlainText(QString("♥ x%1").arg(v));
    });
    connect(jugador, &Jugador::puntajeActualizado, this, [this](int p){
        if (textoPuntos) textoPuntos->setPlainText(QString("⭐ %1").arg(p));
        emit puntosActualizados(p);
    });
}

void IAObstaculos::setDificultadInicial(int d)

{
    dificultadInicial = qBound(1, d, 3);
}

void IAObstaculos::setNivelVertical(bool v)
{
    nivelVertical = v;
}


// ─────────────────────────────────────────────────────────────
//  iniciar()
// ─────────────────────────────────────────────────────────────
void IAObstaculos::iniciar(int segundosParaGanar)
{
    nivelDificultad    = dificultadInicial;
    ticksTranscurridos = 0;
    segundosRestantes  = segundosParaGanar;
    terminado          = false;
    gano               = false;
    obstaculosEnVistaReciente = 0;
    colisionesRecientes       = 0;

    // Ajustar intervalos según dificultad inicial
    switch (dificultadInicial) {
    case 1: intervalMinimo = 1400; intervalMaximo = 3200; break;
    case 2: intervalMinimo = 1000; intervalMaximo = 2400; break;
    case 3: intervalMinimo =  700; intervalMaximo = 1800; break;
    }

    // Limpiar obstáculos anteriores
    for (Obstaculo *o : listaObstaculos) {
        scene->removeItem(o);
        delete o;
    }
    listaObstaculos.clear();

    limpiarHUD();

    // ── HUD: tiempo ──────────────────────────────────────
    fondoTiempo = new QGraphicsRectItem(270, 6, 260, 42);
    fondoTiempo->setBrush(QColor(0, 0, 0, 160));
    fondoTiempo->setPen(Qt::NoPen);
    fondoTiempo->setZValue(10);
    scene->addItem(fondoTiempo);

    textoTiempo = new QGraphicsTextItem();
    textoTiempo->setDefaultTextColor(Qt::white);
    textoTiempo->setFont(QFont("Arial", 18, QFont::Bold));
    textoTiempo->setPos(285, 10);
    textoTiempo->setZValue(11);
    scene->addItem(textoTiempo);

    // ── HUD: vidas ───────────────────────────────────────
    textoVidas = new QGraphicsTextItem();
    textoVidas->setDefaultTextColor(QColor(255, 100, 100));
    textoVidas->setFont(QFont("Arial", 18, QFont::Bold));
    textoVidas->setPos(10, 10);
    textoVidas->setZValue(11);
    scene->addItem(textoVidas);
    textoVidas->setPlainText(QString("♥ x%1").arg(jugador->getVidas()));

    // ── HUD: puntos ──────────────────────────────────────
    textoPuntos = new QGraphicsTextItem();
    textoPuntos->setDefaultTextColor(QColor(255, 220, 50));
    textoPuntos->setFont(QFont("Arial", 18, QFont::Bold));
    textoPuntos->setPos(620, 10);
    textoPuntos->setZValue(11);
    scene->addItem(textoPuntos);
    textoPuntos->setPlainText(QString("⭐ %1").arg(jugador->getPuntos()));

    actualizarHUD();

    timerDecision->start(decidirIntervalo());
    timerSegundo->start();
}

void IAObstaculos::limpiarHUD()
{
    auto borrar = [&](auto *&ptr){
        if (ptr) { scene->removeItem(ptr); delete ptr; ptr = nullptr; }
    };
    borrar(textoTiempo);
    borrar(fondoTiempo);
    borrar(textoVidas);
    borrar(textoPuntos);
    borrar(textoFin);
    borrar(textoSubFin);
    borrar(fondoFin);
}

void IAObstaculos::detener()
{
    timerDecision->stop();
    timerSegundo->stop();
}

// ─────────────────────────────────────────────────────────────
//  actualizar()  –  llamado cada tick (60 fps)
// ─────────────────────────────────────────────────────────────
void IAObstaculos::actualizar()
{
    if (terminado) return;

    ticksTranscurridos++;

    // Mover cada obstáculo según su propia física
    for (Obstaculo *o : listaObstaculos)
        o->actualizarFisica();

    limpiarObstaculosFueraDePantalla();
    verificarColisiones();

    // Escalar dificultad cada 10 segundos (600 ticks a 60fps)
    if (ticksTranscurridos % 600 == 0)
        ajustarDificultad();
}

std::vector<Obstaculo*>& IAObstaculos::obstaculos()
{
    return listaObstaculos;
}

bool IAObstaculos::juegoTerminado() const { return terminado; }
bool IAObstaculos::jugadorGano()    const { return gano; }

// ─────────────────────────────────────────────────────────────
//  tickSegundo()
// ─────────────────────────────────────────────────────────────
void IAObstaculos::tickSegundo()
{
    if (terminado) return;

    segundosRestantes--;
    actualizarHUD();
    emit tiempoActualizado(segundosRestantes);

    if (segundosRestantes <= 0) {
        terminado = true;
        gano      = true;
        detener();
        mostrarMensajeFin(true);
        emit nivelCompletado();
    }
}

// ─────────────────────────────────────────────────────────────
//  decidirObstaculo()  –  Razonamiento del agente
//
//  PERCEPCIÓN:  ¿Cuánto tiempo queda? ¿Cómo va el jugador?
//  RAZONAMIENTO: elige combinación tipo+física ponderada por aprendizaje
//  ACCIÓN:      genera el objeto con esa combinación
// ─────────────────────────────────────────────────────────────
void IAObstaculos::decidirObstaculo()
{
    if (terminado) return;

    // ── a) PERCEPCIÓN ──────────────────────────────────
    bool jugadorEnPeligro = (jugador->getVidas() == 1);
    bool tiempoEscaso     = (segundosRestantes < 15);

    // ── b) RAZONAMIENTO ────────────────────────────────
    bool esColeccionable = debeSerColeccionable();

    // Elegir combinación (tipo + física) usando pesos aprendidos
    auto [tipo, modelo] = elegirCombinacionAprendida();

    // Si el jugador está en peligro crítico, aumentar coleccionables para darle chance
    if (jugadorEnPeligro && !tiempoEscaso) {
        if (QRandomGenerator::global()->bounded(100) < 50)
            esColeccionable = true;
    }

    // Forzar obstáculo (no coleccionable) si queda poco tiempo y el jugador va bien
    if (tiempoEscaso && jugador->getPuntos() > 50) {
        esColeccionable = false;
    }

    qreal altura = decidirAltura(modelo);

    // La parabólica ya no nace siempre al extremo derecho:
    // aparece un poco más cerca del jugador y con variación horizontal,
    // para que sí tenga interacción real y margen de esquiva.
    qreal xInicial;
    qreal yInicial;
    if(nivelVertical)
    {
        xInicial = QRandomGenerator::global()->bounded(50, 750);

        // Nacen fuera de la pantalla por arriba
        yInicial = -100;

        // En modo vertical todos caen
        modelo = FIS_PARABOLICA;
    }
    else
    {
        xInicial = 820;
        yInicial = altura;
    }

    // ── c) ACCIÓN ──────────────────────────────────────

    if (modelo == FIS_PARABOLICA)
    {
        QGraphicsTextItem *aviso = new QGraphicsTextItem("⚠");

        aviso->setDefaultTextColor(Qt::red);
        aviso->setFont(QFont("Arial", 28, QFont::Bold));
        aviso->setZValue(50);

        aviso->setPos(xInicial, altura);

        scene->addItem(aviso);

        QTimer::singleShot(700, this, [=]()
                           {
                               if (scene)
                               {
                                   scene->removeItem(aviso);
                                   delete aviso;
                               }

                               generarObstaculo(tipo,
                                                modelo,
                                                xInicial,
                                                altura,
                                                esColeccionable);
                           });
    }
    else
    {
        generarObstaculo(tipo,
                         modelo,
                         xInicial,
                         altura,
                         esColeccionable);
    }

    obstaculosEnVistaReciente++;
    if (obstaculosEnVistaReciente > 10) {
        obstaculosEnVistaReciente = 1;
        colisionesRecientes       = 0;
    }

    timerDecision->start(decidirIntervalo());
}

// ─────────────────────────────────────────────────────────────
//  reforzarAprendizaje()
//
//  d) APRENDIZAJE: cuando una combinación (tipo, física) logra
//     golpear al jugador, incrementa su peso para ser elegida más.
//     Usa decaimiento exponencial para evitar sobre-especialización.
// ─────────────────────────────────────────────────────────────
void IAObstaculos::reforzarAprendizaje(TipoObst tipo, ModeloFis modelo)
{
    int clave = tipo * 10 + modelo;
    pesosCombinacion[clave] += 1.5;   // refuerzo positivo

    // Decaimiento suave en el resto para mantener diversidad
    for (auto &par : pesosCombinacion) {
        if (par.first != clave)
            par.second *= 0.95;
        // Peso mínimo para que ninguna combinación desaparezca del todo
        par.second = qMax(par.second, 0.2);
    }

    colisionesRecientes++;
    qDebug() << "IA aprendizaje: clave" << clave
             << "nuevo peso" << pesosCombinacion[clave];
}

// ─────────────────────────────────────────────────────────────
//  elegirCombinacionAprendida()
//
//  Selección ponderada por pesos (ruleta ponderada / roulette wheel).
//  Las combinaciones con mayor peso tienen más probabilidad de salir.
// ─────────────────────────────────────────────────────────────
std::pair<IAObstaculos::TipoObst, IAObstaculos::ModeloFis>
IAObstaculos::elegirCombinacionAprendida()
{
    // En dificultad 1 solo usamos galletas lineales (introducción al juego)
    if (nivelDificultad == 1) {
        return { OBS_GALLETA, FIS_LINEAL };
    }

    // Construir lista de pesos solo para combinaciones habilitadas por dificultad
    std::vector<std::pair<int, double>> candidatos;

    for (int t = 0; t <= 2; t++) {
        for (int m = 0; m <= 2; m++) {
            // Nivel 2: habilitar física parabólica
            // Nivel 3: habilitar también oscilatoria
            if (m == 1 && nivelDificultad < 2) continue;
            if (m == 2 && nivelDificultad < 3) continue;
            candidatos.push_back({ t * 10 + m, pesosCombinacion[t * 10 + m] });
        }
    }

    double total = 0;
    for (auto &c : candidatos) total += c.second;

    double r = QRandomGenerator::global()->generateDouble() * total;
    double acum = 0;
    for (auto &c : candidatos) {
        acum += c.second;
        if (r <= acum) {
            int clave = c.first;
            return { static_cast<TipoObst>(clave / 10),
                    static_cast<ModeloFis>(clave % 10) };
        }
    }

    return { OBS_GALLETA, FIS_LINEAL };
}

// ─────────────────────────────────────────────────────────────
//  Generación de obstáculos y coleccionables
// ─────────────────────────────────────────────────────────────
void IAObstaculos::generarObstaculo(TipoObst tipo, ModeloFis modelo,
                                    qreal x, qreal y, bool esColeccionable)
{
    Obstaculo *o = new Obstaculo();
    o->esColeccionable = esColeccionable;

    // La velocidad general sigue escalando con dificultad,
    // pero la parabólica va un poco más lenta para que sea esquivable.
    if (modelo == FIS_PARABOLICA) {
        o->velocidadX = 2.1 + nivelDificultad * 0.35;
    } else {
        o->velocidadX = 3.0 + nivelDificultad * 0.8;
    }

    // Asignar modelo físico
    switch (modelo) {
    case FIS_LINEAL:
        o->modeloFisico = Obstaculo::FISICA_LINEAL;
        break;

    case FIS_PARABOLICA:
        o->modeloFisico = Obstaculo::FISICA_PARABOLICA;

        if(nivelVertical)
        {
            // Caída vertical desde arriba
            o->velocidadX = 0;
            o->velocidadY = 2.0;
            o->gravedad   = 0.15;
        }
        else
        {
            // Comportamiento original del nivel lateral
            o->velocidadY = 0.0;
            o->gravedad   = 0.08;
        }
        break;

    case FIS_OSCILATORIA:
        o->modeloFisico = Obstaculo::FISICA_OSCILATORIA;
        o->yBase       = y;
        o->amplitud    = 40.0 + nivelDificultad * 10.0;
        o->frecuencia  = 0.04 + nivelDificultad * 0.01;
        o->fase        = QRandomGenerator::global()->generateDouble() * M_PI * 2;
        break;
    }

    // Sprite según tipo
    // Obstáculos dañinos → galleta
    // Coleccionables     → caramelo (10 pts) o paleta (20 pts)
    if (esColeccionable) {
        bool esPaleta = (QRandomGenerator::global()->bounded(100) < 30);
        o->tipo = esPaleta ? Obstaculo::PALETA : Obstaculo::CARAMELO;
        o->cargarSprite(esPaleta ? ":/Imagenes/Paleta.png"
                                 : ":/Imagenes/Caramelo.png", 50, 50);
    } else {
        o->tipo = Obstaculo::GALLETA;
        o->cargarSprite(":/Imagenes/Galleta.png",
                        70 + nivelDificultad * 5,
                        70 + nivelDificultad * 5);
    }

    // Recordar la combinación para refuerzo posterior
    o->setData(0, static_cast<int>(tipo));
    o->setData(1, static_cast<int>(modelo));

    o->setPos(x, y);
    scene->addItem(o);
    listaObstaculos.push_back(o);
}

// ─────────────────────────────────────────────────────────────
//  verificarColisiones()
//
//  Distingue entre obstáculos dañinos y coleccionables.
//  Si el jugador choca con un obstáculo dañino Y no tiene
//  invulnerabilidad activa: refuerza el aprendizaje de esa
//  combinación (la IA aprendió que funcionó).
// ─────────────────────────────────────────────────────────────
void IAObstaculos::verificarColisiones()
{
    QList<QGraphicsItem*> cols = jugador->collidingItems();

    for (QGraphicsItem *item : cols) {
        for (auto it = listaObstaculos.begin(); it != listaObstaculos.end(); ++it) {
            Obstaculo *o = *it;
            if (item != o) continue;

            if (o->esColeccionable) {
                // Recoger item: dar puntos
                int puntos = (o->tipo == Obstaculo::PALETA) ? 20 : 10;
                jugador->recogerItem(puntos);
                scene->removeItem(o);
                delete o;
                listaObstaculos.erase(it);
                return;  // reiniciar iteración
            } else {
                // Obstáculo dañino
                bool golpeEfectivo = jugador->recibirGolpe();
                if (golpeEfectivo) {
                    QGraphicsPixmapItem *splash =
                        new QGraphicsPixmapItem(
                            QPixmap(":/Imagenes/Splash.png")
                                .scaled(120, 120,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));

                    splash->setZValue(100);

                    splash->setPos(
                        jugador->x() - 20,
                        jugador->y() - 20
                    );

                    scene->addItem(splash);
                    scene->addItem(splash);

                    QGraphicsTextItem *txt =
                        new QGraphicsTextItem("-1");

                    QFont fuente;
                    fuente.setPointSize(18);
                    fuente.setBold(true);

                    txt->setFont(fuente);
                    txt->setDefaultTextColor(Qt::red);
                    txt->setZValue(101);

                    txt->setPos(
                        jugador->x() + 10,
                        jugador->y() - 60
                        );

                    scene->addItem(txt);
                    QTimer::singleShot(500, this, [=]()
                    {
                        scene->removeItem(splash);
                        delete splash;

                        scene->removeItem(txt);
                        delete txt;
                    });
                    TipoObst  t = static_cast<TipoObst>(o->data(0).toInt());
                    ModeloFis m = static_cast<ModeloFis>(o->data(1).toInt());
                    reforzarAprendizaje(t, m);

                    if (!jugador->estaVivo()) {
                        terminado = true;
                        detener();
                        mostrarMensajeFin(false);
                        emit jugadorMurio();
                        return;
                    }
                }
            }
            break;
        }
    }
}

void IAObstaculos::limpiarObstaculosFueraDePantalla()
{
    listaObstaculos.erase(
        std::remove_if(listaObstaculos.begin(), listaObstaculos.end(),
                       [this](Obstaculo *o) {
                           if (o->fueraDePantalla()) {
                               scene->removeItem(o);
                               delete o;
                               return true;
                           }
                           return false;
                       }),
        listaObstaculos.end()
        );
}

void IAObstaculos::ajustarDificultad()
{
    if (nivelDificultad < 3) nivelDificultad++;
    intervalMinimo = qMax(500,  intervalMinimo - 200);
    intervalMaximo = qMax(1200, intervalMaximo - 300);
    qDebug() << "Dificultad subio a" << nivelDificultad;
}

// ─────────────────────────────────────────────────────────────
//  Helpers de decisión
// ─────────────────────────────────────────────────────────────
bool IAObstaculos::debeSerColeccionable()
{
    // Al inicio más coleccionables, luego más obstáculos
    // ~40% coleccionables en nivel 1, ~25% en nivel 3
    int prob = qMax(15, 40 - (nivelDificultad - 1) * 8);
    return QRandomGenerator::global()->bounded(100) < prob;
}

qreal IAObstaculos::decidirAltura(ModeloFis modelo)
{
    switch (modelo) {
    case FIS_LINEAL:
        // 70% al ras del suelo, 30% a media altura (hay que saltar)
        return (QRandomGenerator::global()->bounded(100) < 70) ? 400 : 330;

    case FIS_PARABOLICA:
        // El obstáculo empieza arriba y cae: punto de entrada en Y arriba
        return 20 + QRandomGenerator::global()->bounded(80);

    case FIS_OSCILATORIA:
        // Centro de la oscilación a media pantalla
        return 320 + QRandomGenerator::global()->bounded(60);
    }
    return 280;
}

int IAObstaculos::decidirIntervalo()
{
    return QRandomGenerator::global()->bounded(intervalMinimo, intervalMaximo);
}

void IAObstaculos::actualizarHUD()
{
    if (textoTiempo)
        textoTiempo->setPlainText(QString("⏱ %1s").arg(segundosRestantes));
}

void IAObstaculos::mostrarMensajeFin(bool jugadorGano)
{
    fondoFin = new QGraphicsRectItem(130, 170, 540, 160);
    fondoFin->setBrush(QColor(0, 0, 0, 210));
    fondoFin->setPen(QPen(jugadorGano ? QColor(255,220,0) : QColor(255,60,60), 3));
    fondoFin->setZValue(20);
    scene->addItem(fondoFin);

    textoFin = new QGraphicsTextItem();
    textoFin->setZValue(21);
    textoFin->setFont(QFont("Arial", 34, QFont::Bold));

    if (jugadorGano) {
        textoFin->setDefaultTextColor(QColor(255, 220, 0));
        textoFin->setPlainText(QString("  ¡GANASTE!  ⭐%1 pts").arg(jugador->getPuntos()));
    } else {
        textoFin->setDefaultTextColor(QColor(255, 60, 60));
        textoFin->setPlainText("  ¡PERDISTE!");
    }
    textoFin->setPos(150, 185);
    scene->addItem(textoFin);

    textoSubFin = new QGraphicsTextItem();
    textoSubFin->setZValue(21);
    textoSubFin->setFont(QFont("Arial", 15));
    textoSubFin->setDefaultTextColor(Qt::white);
    textoSubFin->setPlainText(jugadorGano
                                  ? "  [R] Reintentar   [ESC] Menú principal"
                                  : "  [R] Reintentar   [ESC] Menú principal");
    textoSubFin->setPos(150, 285);
    scene->addItem(textoSubFin);
}
