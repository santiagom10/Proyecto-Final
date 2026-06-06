#include "dronia.h"
#include <QPainter>
#include "jugador.h"
#include <QGraphicsScene>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <algorithm>

// ─────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────
DronIA::DronIA(QGraphicsScene *scene, Jugador *jugador, int dificultad, QObject *parent)
    : QObject(parent)
    , scene(scene)
    , jugador(jugador)
    , dificultad(qBound(1, dificultad, 3))
{
    for (int i = 0; i < 9; i++) {
        histogramaZonas[i] = 0;
        pesosAtaque[i]     = 0;
    }

    timerSegundo = new QTimer(this);
    timerSegundo->setInterval(1000);
    connect(timerSegundo, &QTimer::timeout, this, &DronIA::tickSegundo);

    connect(jugador, &Jugador::vidasCambiaron,     this, [this](int v){ actualizarHUDVidas(v);  });
    connect(jugador, &Jugador::puntajeActualizado, this, [this](int p){ actualizarHUDPuntos(p); });

    // ── Precargar sprite del dron ─────────────────────────────
    // ── Precargar sprite del dron ─────────────────────────────
    pixDronBase = QPixmap(":/Imagenes/Dron.png").scaled(
        DRON_W, DRON_H,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    pixGalleta = QPixmap(":/Imagenes/Galleta.png").scaled(
        40, 40,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    // ── Precargar sprites de coleccionables ──────────────────
    pixCaramelo = QPixmap(":/Imagenes/Caramelo.png").scaled(
        32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation
        );

    pixPaleta = QPixmap(":/Imagenes/Paleta.png").scaled(
        32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation
        );

    // ── Cargar hoja de charcos ───────────────────────────────
    // Deja SOLO uno de estos nombres, el que tengas realmente en el .qrc
    pixCharcoSheet = QPixmap(":/Imagenes/Charco.png");

    // Recortes de los 3 charcos visibles en tu PNG
    charcoFrames = {
        QRect(51, 87, 227, 196),   // charco verde grande
        QRect(306, 125, 137, 134),  // charco morado
        QRect(465, 120, 162, 138)   // charco rojo
    };
}
DronIA::~DronIA()
{
    detener();
    limpiarHUD();
}

void DronIA::setDificultad(int d)
{
    dificultad = qBound(1, d, 3);
}

// ─────────────────────────────────────────────────────────────
//  iniciar()
// ─────────────────────────────────────────────────────────────
void DronIA::iniciar()
{
    terminado          = false;
    gano               = false;
    ticksInvuln        = 0;
    ticksColeccionable = 0;
    factorVelocidad    = 1.0;

    switch (dificultad) {
    case 1:
        velocidadBase     = 1.8;
        segundosRestantes = 60;
        intervalDisparo   = 150;   // dispara cada ~2.5s
        maxCharcos        = 3;
        break;
    case 2:
        velocidadBase     = 2.8;
        segundosRestantes = 50;
        intervalDisparo   = 100;   // dispara cada ~1.7s
        maxCharcos        = 5;
        break;
    case 3:
        velocidadBase     = 3.8;
        segundosRestantes = 40;
        intervalDisparo   = 70;    // dispara cada ~1.2s
        maxCharcos        = 7;
        break;
    }
    velocidadActual     = velocidadBase;
    ticksProximoDisparo = intervalDisparo;
    ticksProximoCharco  = 120;

    dronX           = 50.0;
    dronY           = 50.0;
    objetivoX       = 400;
    objetivoY       = 280;
    estrategia      = DIRECTO;
    ticksEstrategia = 0;

    for (int i = 0; i < 9; i++) {
        histogramaZonas[i] = 0;
        pesosAtaque[i]     = 0;
    }
    zonaPreferida = 4;

    limpiarHUD();
    limpiarProyectiles();
    limpiarCharcos();
    iniciarHUD();

    timerSegundo->start();
}

// ─────────────────────────────────────────────────────────────
//  detener()
// ─────────────────────────────────────────────────────────────
void DronIA::detener()
{
    if (timerSegundo) timerSegundo->stop();
}

// ─────────────────────────────────────────────────────────────
//  actualizar()  — llamado cada ~16ms desde tickJuego()
// ─────────────────────────────────────────────────────────────
void DronIA::actualizar()
{
    if (terminado) return;

    if (ticksInvuln > 0) ticksInvuln--;

    // ── PERCEPCIÓN: registrar zona del jugador ────────────────
    int zona = zonaDelJugador();
    histogramaZonas[zona]++;

    // ── RAZONAMIENTO: cada 120 ticks (~2s) reevalúa estrategia ──
    ticksEstrategia++;
    if (ticksEstrategia >= 120) {
        ticksEstrategia = 0;
        elegirEstrategiaMovimiento();
    }

    // ── ACCIÓN: mover el dron hacia el objetivo ───────────────
    qreal dx   = objetivoX - dronX;
    qreal dy   = objetivoY - dronY;
    qreal dist = qSqrt(dx * dx + dy * dy);

    if (dist > 1.0) {
        dronX += (dx / dist) * velocidadActual;
        dronY += (dy / dist) * velocidadActual;
    }

    dronX = qBound(10.0, dronX, 760.0);
    dronY = qBound(60.0, dronY, 450.0);

    if (estrategia == DIRECTO) {
        objetivoX = jugador->x() + 20;
        objetivoY = jugador->y() + 20;
    }

    actualizarVisualDron();

    // ── Disparar proyectil ────────────────────────────────────
    ticksProximoDisparo--;
    if (ticksProximoDisparo <= 0) {
        ticksProximoDisparo = intervalDisparo;
        dispararProyectil();
    }

    actualizarProyectiles();
    verificarColisionesProyectiles();

    // ── Charcos ───────────────────────────────────────────────
    ticksProximoCharco--;
    if (ticksProximoCharco <= 0) {
        ticksProximoCharco = 200 + QRandomGenerator::global()->bounded(200);
        generarCharco();
    }
    actualizarCharcos();
    verificarJugadorEnCharco();

    // ── Colisión directa dron-jugador ─────────────────────────
    verificarColisionDirecta();

    // ── Coleccionables cada ~3s ───────────────────────────────
    ticksColeccionable++;
    if (ticksColeccionable >= 180) {
        ticksColeccionable = 0;
        generarColeccionable();
    }
    verificarColeccionables();
}

// ─────────────────────────────────────────────────────────────
//  zonaDelJugador()
// ─────────────────────────────────────────────────────────────
int DronIA::zonaDelJugador() const
{
    int col  = qBound(0, (int)(jugador->x() / 267), 2);
    int fila = qBound(0, (int)((jugador->y() - 60) / 147), 2);
    return fila * 3 + col;
}

// ─────────────────────────────────────────────────────────────
//  zonaMasFrecuentada()
// ─────────────────────────────────────────────────────────────
int DronIA::zonaMasFrecuentada() const
{
    int maxZona = 0, maxVal = -1;
    for (auto &par : histogramaZonas) {
        if (par.second > maxVal) { maxVal = par.second; maxZona = par.first; }
    }
    return maxZona;
}

// ─────────────────────────────────────────────────────────────
//  elegirEstrategiaMovimiento()  — RAZONAMIENTO del agente
// ─────────────────────────────────────────────────────────────
void DronIA::elegirEstrategiaMovimiento()
{
    zonaPreferida = zonaMasFrecuentada();

    int   col    = zonaPreferida % 3;
    int   fila   = zonaPreferida / 3;
    qreal centroX = col  * 267 + 133;
    qreal centroY = fila * 147 + 60 + 73;

    qreal dx   = jugador->x() - dronX;
    qreal dy   = jugador->y() - dronY;
    qreal dist = qSqrt(dx * dx + dy * dy);

    int maxPeso = 0;
    for (auto &p : pesosAtaque) maxPeso = qMax(maxPeso, p.second);

    if (dist > 300 && histogramaZonas[zonaPreferida] > 50) {
        estrategia = EMBOSCADA;
        objetivoX  = centroX;
        objetivoY  = centroY;
        qDebug() << "DronIA: EMBOSCADA zona" << zonaPreferida;
    } else if (maxPeso > 3 && dist > 150) {
        estrategia = CORTE;
        objetivoX  = (jugador->x() + centroX) / 2.0;
        objetivoY  = (jugador->y() + centroY) / 2.0;
        qDebug() << "DronIA: CORTE";
    } else {
        estrategia = DIRECTO;
        objetivoX  = jugador->x() + 20;
        objetivoY  = jugador->y() + 20;
        qDebug() << "DronIA: DIRECTO";
    }
}

// ─────────────────────────────────────────────────────────────
//  reforzarAprendizaje()
// ─────────────────────────────────────────────────────────────
void DronIA::reforzarAprendizaje(int zona)
{
    pesosAtaque[zona]++;
    velocidadActual = qMin(velocidadBase + pesosAtaque[zona] * 0.3,
                           velocidadBase * 2.0);
    intervalDisparo = qMax(40, intervalDisparo - 5);
    histogramaZonas[zona] += 20;

    qDebug() << "DronIA aprendizaje: zona" << zona
             << "| vel:" << velocidadActual
             << "| intervalo disparo:" << intervalDisparo;
}

// ─────────────────────────────────────────────────────────────
//  dispararProyectil()  — lanza bola de gelatina al jugador
//  Usa Caramelo.png o Paleta.png como sprite del proyectil
// ─────────────────────────────────────────────────────────────
void DronIA::dispararProyectil()
{
    qreal px = dronX + DRON_W / 2.0;
    qreal py = dronY + DRON_H / 2.0;
    qreal tx = jugador->x() + 25;
    qreal ty = jugador->y() + 25;

    qreal dx   = tx - px;
    qreal dy   = ty - py;
    qreal dist = qSqrt(dx * dx + dy * dy);
    if (dist < 1.0) return;

    qreal velProyectil = 5.0 + dificultad * 1.5;

    Proyectil *p = new Proyectil();
    p->vx = (dx / dist) * velProyectil;
    p->vy = (dy / dist) * velProyectil;

    // cambie esto
    /*
    bool usarPaleta = (QRandomGenerator::global()->bounded(2) == 0);
    QPixmap pixProy = usarPaleta ? pixPaleta : pixCaramelo;

    QPixmap small = pixProy.scaled(
        28, 28,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    */

    QPixmap small = pixGalleta.scaled(
        40, 40,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    p->itemSprite = new QGraphicsPixmapItem(small);
    p->itemSprite->setPos(px - 14, py - 14);
    p->itemSprite->setZValue(8);
    scene->addItem(p->itemSprite);

    proyectiles.append(p);
}

// ─────────────────────────────────────────────────────────────
//  actualizarProyectiles()
// ─────────────────────────────────────────────────────────────
void DronIA::actualizarProyectiles()
{
    for (auto it = proyectiles.begin(); it != proyectiles.end(); ) {
        Proyectil *p = *it;

        if (!p->itemSprite) {
            delete p;
            it = proyectiles.erase(it);
            continue;
        }

        p->itemSprite->setPos(p->itemSprite->x() + p->vx,
                              p->itemSprite->y() + p->vy);

        qreal bx = p->itemSprite->x();
        qreal by = p->itemSprite->y();

        if (bx < -40 || bx > 860 || by < 40 || by > 520) {
            scene->removeItem(p->itemSprite);
            delete p->itemSprite;
            delete p;
            it = proyectiles.erase(it);
        } else {
            ++it;
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  verificarColisionesProyectiles()
// ─────────────────────────────────────────────────────────────
void DronIA::verificarColisionesProyectiles()
{
    qreal jx = jugador->x();
    qreal jy = jugador->y();
    qreal jw = 50.0;
    qreal jh = 50.0;

    for (auto it = proyectiles.begin(); it != proyectiles.end(); ) {
        Proyectil *p = *it;

        if (!p->itemSprite) {
            delete p;
            it = proyectiles.erase(it);
            continue;
        }
        // cambie esto para que el sprite este fijo
        /*
        qreal bx = p->itemSprite->x() + 20;
        qreal by = p->itemSprite->y() + 20;
        */

        QRectF r = p->itemSprite->boundingRect();

        qreal bx = p->itemSprite->x() + r.width()  / 2.0;
        qreal by = p->itemSprite->y() + r.height() / 2.0;

        bool golpea = (bx > jx && bx < jx + jw && by > jy && by < jy + jh);

        if (golpea) {
            bool efectivo = jugador->recibirGolpe();

            generarCharco(p->itemSprite->x(), p->itemSprite->y());

            scene->removeItem(p->itemSprite);
            delete p->itemSprite;
            delete p;
            it = proyectiles.erase(it);

            if (efectivo) {
                reforzarAprendizaje(zonaDelJugador());
                if (!jugador->estaVivo()) {
                    terminado = true;
                    detener();
                    mostrarMensajeFin(false);
                    emit jugadorMurio();
                    return;
                }
            }
        } else {
            ++it;
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  verificarColisionDirecta()  — dron toca al jugador
// ─────────────────────────────────────────────────────────────
void DronIA::verificarColisionDirecta()
{
    if (ticksInvuln > 0) return;

    // Centro del dron y del jugador
    qreal cx = dronX + DRON_W / 2.0;
    qreal cy = dronY + DRON_H / 2.0;
    qreal jx = jugador->x() + 25;
    qreal jy = jugador->y() + 25;

    qreal dist = qSqrt((cx - jx) * (cx - jx) + (cy - jy) * (cy - jy));

    // Radio de colisión = mitad dron + mitad jugador aprox.
    if (dist < 45) {
        bool golpe = jugador->recibirGolpe();
        if (golpe) {
            ticksInvuln = TICKS_INVULN;
            reforzarAprendizaje(zonaDelJugador());

            if (!jugador->estaVivo()) {
                terminado = true;
                detener();
                mostrarMensajeFin(false);
                emit jugadorMurio();
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  generarCharco()  — versión con posición opcional (impacto)
// ─────────────────────────────────────────────────────────────
void DronIA::generarCharco(qreal forzarX, qreal forzarY)
{
    if ((int)charcos.size() >= maxCharcos) return;
    if (pixCharcoSheet.isNull() || charcoFrames.isEmpty()) return;

    qreal x, y, w, h;

    if (forzarX >= 0) {
        x = forzarX - 30;
        y = forzarY - 20;
        w = 90;
        h = 70;
    } else {
        x = 60  + QRandomGenerator::global()->bounded(660);
        y = 80  + QRandomGenerator::global()->bounded(340);
        w = 90  + QRandomGenerator::global()->bounded(30);
        h = 70  + QRandomGenerator::global()->bounded(25);
    }

    Charco *c = new Charco();
    c->rect = QRectF(x, y, w, h);
    c->ticksVida = 400 + QRandomGenerator::global()->bounded(200);

    int idx = QRandomGenerator::global()->bounded(charcoFrames.size());
    QRect src = charcoFrames[idx];

    QPixmap charcoSprite = pixCharcoSheet.copy(src).scaled(
        (int)w, (int)h,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    c->item = new QGraphicsPixmapItem(charcoSprite);
    c->item->setPos(x, y);
    c->item->setOpacity(0.90);
    c->item->setZValue(2);

    scene->addItem(c->item);
    charcos.append(c);
}

// ─────────────────────────────────────────────────────────────
//  actualizarCharcos()
// ─────────────────────────────────────────────────────────────
void DronIA::actualizarCharcos()
{
    for (auto it = charcos.begin(); it != charcos.end(); ) {
        Charco *c = *it;
        c->ticksVida--;

        if (c->ticksVida < 60 && c->item)
            c->item->setOpacity(c->ticksVida / 60.0 * 0.80);

        if (c->ticksVida <= 0) {
            if (c->item) { scene->removeItem(c->item); delete c->item; }
            delete c;
            it = charcos.erase(it);
        } else {
            ++it;
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  verificarJugadorEnCharco()  — aplica factor de ralentización
// ─────────────────────────────────────────────────────────────
void DronIA::verificarJugadorEnCharco()
{
    qreal jx = jugador->x() + 25;
    qreal jy = jugador->y() + 25;

    bool enCharco = false;
    for (Charco *c : charcos) {
        if (c->rect.contains(jx, jy)) { enCharco = true; break; }
    }
    factorVelocidad = enCharco ? 0.4 : 1.0;
}

// ─────────────────────────────────────────────────────────────
//  generarColeccionable()  — usa Caramelo.png / Paleta.png
// ─────────────────────────────────────────────────────────────
void DronIA::generarColeccionable()
{
    if ((int)coleccionables.size() >= 5) return;

    qreal x = 60 + QRandomGenerator::global()->bounded(680);
    qreal y = 80 + QRandomGenerator::global()->bounded(360);

    bool    esPaleta = (QRandomGenerator::global()->bounded(2) == 0);
    QPixmap &pixSrc  = esPaleta ? pixPaleta : pixCaramelo;

    QGraphicsPixmapItem *item = nullptr;

    if (!pixSrc.isNull()) {
        QPixmap scaled = pixSrc.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        item = new QGraphicsPixmapItem(scaled);
        item->setPos(x, y);
    } else {
        // Fallback círculo si falta el PNG
        QPixmap fallback(64, 64);
        fallback.fill(Qt::transparent);
        QPainter p(&fallback);
        QColor col = esPaleta ? QColor(255, 210, 0) : QColor(255, 100, 100);
        p.setBrush(QBrush(col));
        p.setPen(QPen(col.darker(150), 2));
        p.drawEllipse(4, 4, 56, 56);
        p.end();
        item = new QGraphicsPixmapItem(fallback);
        item->setPos(x, y);
    }

    item->setZValue(3);
    scene->addItem(item);
    coleccionables.append(item);
}

// ─────────────────────────────────────────────────────────────
//  verificarColeccionables()
// ─────────────────────────────────────────────────────────────
void DronIA::verificarColeccionables()
{
    for (auto it = coleccionables.begin(); it != coleccionables.end(); ) {
        QGraphicsPixmapItem *col = *it;
        qreal cx = col->x() + 18;
        qreal cy = col->y() + 18;
        qreal jx = jugador->x() + 25;
        qreal jy = jugador->y() + 25;
        qreal dx = cx - jx;
        qreal dy = cy - jy;

        if (qSqrt(dx * dx + dy * dy) < 44) {
            jugador->recogerItem(15);
            scene->removeItem(col);
            delete col;
            it = coleccionables.erase(it);
        } else {
            ++it;
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  actualizarVisualDron()
// ─────────────────────────────────────────────────────────────
void DronIA::actualizarVisualDron()
{
    if (spriteDron) spriteDron->setPos(dronX, dronY);
}

// ─────────────────────────────────────────────────────────────
//  tickSegundo()
// ─────────────────────────────────────────────────────────────
void DronIA::tickSegundo()
{
    if (terminado) return;
    segundosRestantes--;
    if (textoTiempo)
        textoTiempo->setPlainText(QString("⏱ %1s").arg(segundosRestantes));
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
//  HUD
// ─────────────────────────────────────────────────────────────
void DronIA::iniciarHUD()
{
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
    textoTiempo->setPlainText(QString("⏱ %1s").arg(segundosRestantes));
    scene->addItem(textoTiempo);

    textoVidas = new QGraphicsTextItem();
    textoVidas->setDefaultTextColor(QColor(255, 100, 100));
    textoVidas->setFont(QFont("Arial", 18, QFont::Bold));
    textoVidas->setPos(10, 10);
    textoVidas->setZValue(11);
    textoVidas->setPlainText(QString("♥ x%1").arg(jugador->getVidas()));
    scene->addItem(textoVidas);

    textoPuntos = new QGraphicsTextItem();
    textoPuntos->setDefaultTextColor(QColor(255, 220, 50));
    textoPuntos->setFont(QFont("Arial", 18, QFont::Bold));
    textoPuntos->setPos(620, 10);
    textoPuntos->setZValue(11);
    textoPuntos->setPlainText(QString("⭐ %1").arg(jugador->getPuntos()));
    scene->addItem(textoPuntos);

    // ─── Sprite del dron ─────────────────────────────────────
    spriteDron = new QGraphicsPixmapItem(pixDronBase);
    spriteDron->setPos(dronX, dronY);
    spriteDron->setZValue(9);
    scene->addItem(spriteDron);
}

void DronIA::limpiarHUD()
{
    auto borrar = [&](auto *&ptr) {
        if (ptr) { scene->removeItem(ptr); delete ptr; ptr = nullptr; }
    };
    borrar(textoTiempo);
    borrar(fondoTiempo);
    borrar(textoVidas);
    borrar(textoPuntos);
    borrar(textoFin);
    borrar(textoSubFin);
    borrar(fondoFin);
    borrar(spriteDron);

    limpiarProyectiles();
    limpiarCharcos();

    for (auto *c : coleccionables) { scene->removeItem(c); delete c; }
    coleccionables.clear();
}

void DronIA::limpiarProyectiles()
{
    for (Proyectil *p : proyectiles) {
        if (p->itemSprite) {
            scene->removeItem(p->itemSprite);
            delete p->itemSprite;
        }
        delete p;
    }
    proyectiles.clear();
}

void DronIA::limpiarCharcos()
{
    for (Charco *c : charcos) {
        if (c->item) { scene->removeItem(c->item); delete c->item; }
        delete c;
    }
    charcos.clear();
}

void DronIA::actualizarHUDVidas(int v)
{
    if (textoVidas) textoVidas->setPlainText(QString("♥ x%1").arg(v));
}

void DronIA::actualizarHUDPuntos(int p)
{
    if (textoPuntos) textoPuntos->setPlainText(QString("⭐ %1").arg(p));
}

void DronIA::mostrarMensajeFin(bool jugadorGano)
{
    fondoFin = new QGraphicsRectItem(130, 170, 540, 160);
    fondoFin->setBrush(QColor(0, 0, 0, 210));
    fondoFin->setPen(QPen(jugadorGano ? QColor(255, 220, 0) : QColor(255, 60, 60), 3));
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
    textoSubFin->setPlainText("  [R] Reintentar   [ESC] Menú principal");
    textoSubFin->setPos(150, 285);
    scene->addItem(textoSubFin);
}
