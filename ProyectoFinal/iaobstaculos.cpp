#include "iaobstaculos.h"
#include "obstaculo.h"
#include "jugador.h"
#include <QFont>
#include <QDebug>

IAObstaculos::IAObstaculos(QGraphicsScene *scene, Jugador *jugador, QObject *parent)
    : QObject(parent),
    scene(scene),
    jugador(jugador),
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
}

void IAObstaculos::iniciar(int segundosParaGanar)
{
    nivelDificultad    = 1;
    ticksTranscurridos = 0;
    intervalMinimo     = 1200;
    intervalMaximo     = 3000;
    segundosRestantes  = segundosParaGanar;
    terminado          = false;
    gano               = false;

    for (Obstaculo *o : listaObstaculos) {
        scene->removeItem(o);
        delete o;
    }
    listaObstaculos.clear();

    if (textoTiempo) { scene->removeItem(textoTiempo); textoTiempo = nullptr; }
    if (fondoTiempo) { scene->removeItem(fondoTiempo); fondoTiempo = nullptr; }

    fondoTiempo = new QGraphicsRectItem(0, 0, 200, 44);
    fondoTiempo->setBrush(QColor(0, 0, 0, 160));
    fondoTiempo->setPen(Qt::NoPen);
    fondoTiempo->setPos(300, 10);
    fondoTiempo->setZValue(10);
    scene->addItem(fondoTiempo);

    textoTiempo = new QGraphicsTextItem();
    textoTiempo->setDefaultTextColor(Qt::white);
    QFont font("Arial", 20, QFont::Bold);
    textoTiempo->setFont(font);
    textoTiempo->setPos(310, 12);
    textoTiempo->setZValue(11);
    scene->addItem(textoTiempo);

    actualizarHUD();

    timerDecision->start(decidirIntervalo());
    timerSegundo->start();
}

void IAObstaculos::detener()
{
    timerDecision->stop();
    timerSegundo->stop();
}

void IAObstaculos::actualizar()
{
    if (terminado) return;

    ticksTranscurridos++;

    for (Obstaculo *o : listaObstaculos)
        o->setX(o->x() - (2.0 + nivelDificultad * 0.5));

    limpiarObstaculosFueraDePantalla();
    verificarColisiones();

    if (ticksTranscurridos % 600 == 0)
        ajustarDificultad();
}

std::vector<Obstaculo*>& IAObstaculos::obstaculos()
{
    return listaObstaculos;
}

bool IAObstaculos::juegoTerminado() const { return terminado; }
bool IAObstaculos::jugadorGano()    const { return gano; }

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

void IAObstaculos::decidirObstaculo()
{
    if (terminado) return;

    TipoObstaculo tipo = decidirTipo();
    qreal altura       = decidirAltura();
    qreal x            = 820;

    generarObstaculo(tipo, x, altura);
    timerDecision->start(decidirIntervalo());
}

IAObstaculos::TipoObstaculo IAObstaculos::decidirTipo()
{
    int r = QRandomGenerator::global()->bounded(100);

    if (nivelDificultad == 1) {
        return GALLETA;
    } else if (nivelDificultad == 2) {
        return (r < 60) ? GALLETA : CARAMELO;
    } else {
        if (r < 40)      return GALLETA;
        else if (r < 70) return CARAMELO;
        else             return PALETA;
    }
}

qreal IAObstaculos::decidirAltura()
{
    if (nivelDificultad == 1) {
        return 280;
    } else {
        int r = QRandomGenerator::global()->bounded(100);
        return (r < 70) ? 280 : 200;
    }
}

int IAObstaculos::decidirIntervalo()
{
    return QRandomGenerator::global()->bounded(intervalMinimo, intervalMaximo);
}

void IAObstaculos::ajustarDificultad()
{
    if (nivelDificultad < 3) nivelDificultad++;
    intervalMinimo = qMax(500,  intervalMinimo - 200);
    intervalMaximo = qMax(1200, intervalMaximo - 400);
}

void IAObstaculos::generarObstaculo(TipoObstaculo tipo, qreal x, qreal y)
{
    Obstaculo *o = new Obstaculo();
    o->tipo = static_cast<Obstaculo::Tipo>(tipo);

    switch (tipo) {
    case GALLETA:
        o->cargarSprite(":/Imagenes/Galleta.png", 60, 60);
        break;
    case CARAMELO:
        o->cargarSprite(":/Imagenes/Caramelo.png", 60, 60);
        break;
    case PALETA:
        o->cargarSprite(":/Imagenes/Paleta.png", 60, 60);
        break;
    }

    o->setPos(x, y);
    scene->addItem(o);
    listaObstaculos.push_back(o);
}

void IAObstaculos::verificarColisiones()
{
    QList<QGraphicsItem*> cols = jugador->collidingItems();
    for (QGraphicsItem *item : cols) {
        for (Obstaculo *o : listaObstaculos) {
            if (item == o) {
                terminado = true;
                detener();
                mostrarMensajeFin(false);
                emit jugadorMurio();
                return;
            }
        }
    }
}

void IAObstaculos::limpiarObstaculosFueraDePantalla()
{
    listaObstaculos.erase(
        std::remove_if(listaObstaculos.begin(), listaObstaculos.end(),
                       [this](Obstaculo *o) {
                           if (o->x() < -80) {
                               scene->removeItem(o);
                               delete o;
                               return true;
                           }
                           return false;
                       }),
        listaObstaculos.end()
        );
}

void IAObstaculos::actualizarHUD()
{
    if (!textoTiempo) return;
    textoTiempo->setPlainText(QString("Tiempo: %1s").arg(segundosRestantes));
}

void IAObstaculos::mostrarMensajeFin(bool jugadorGano)
{
    QGraphicsRectItem *fondo = new QGraphicsRectItem(150, 180, 500, 140);
    fondo->setBrush(QColor(0, 0, 0, 200));
    fondo->setPen(Qt::NoPen);
    fondo->setZValue(20);
    scene->addItem(fondo);

    QGraphicsTextItem *texto = new QGraphicsTextItem();
    texto->setZValue(21);
    QFont font("Arial", 32, QFont::Bold);
    texto->setFont(font);

    if (jugadorGano) {
        texto->setDefaultTextColor(QColor(255, 220, 0));
        texto->setPlainText("  GANASTE!");
    } else {
        texto->setDefaultTextColor(QColor(255, 60, 60));
        texto->setPlainText("  PERDISTE!");
    }
    texto->setPos(190, 195);
    scene->addItem(texto);

    QGraphicsTextItem *sub = new QGraphicsTextItem();
    sub->setZValue(21);
    QFont fontSub("Arial", 16);
    sub->setFont(fontSub);
    sub->setDefaultTextColor(Qt::white);
    sub->setPlainText(jugadorGano ? "  Pasas al Nivel 2 - presiona ESC"
                                  : "  Presiona ESC para volver al menu");
    sub->setPos(190, 265);
    scene->addItem(sub);
}
