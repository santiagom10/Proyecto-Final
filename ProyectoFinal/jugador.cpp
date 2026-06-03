#include "jugador.h"
#include <QTransform>

// ─────────────────────────────────────────────────────────────
//  Mapa del spritesheet (7 cols x 2 filas, frame 100x178 px)
//
//  FILA 0:  [0]=parado+skate  [1-4]=caminar  [5]=correr  [6]=en skate
//  FILA 1:  [0]=ride1  [1]=ride2  [2]=trick1  [3]=trick2
//           [4]=land   [5]=fall   [6]=hold
//
//  Lógica de animación:
//    IDLE       → fila 1, col 0          (parado sobre el skate)
//    CORRIENDO  → fila 1, cols 0‒1       (ride1 / ride2, loop)
//    SALTANDO   → fila 1, cols 2‒3       (trick1 / trick2, loop)
//    ATERRIZANDO→ fila 1, col 4          (land, 1 frame y vuelve a IDLE)
// ─────────────────────────────────────────────────────────────

Jugador::Jugador(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidadX(0), velocidadY(0),
    enSueloFlag(true), sueloY(320),
    mirrorX(false),
    estado(IDLE), frameAnim(0), contadorAnim(0),
    fw(0), fh(0)
{}

// ── Carga el spritesheet y muestra el frame inicial ──────────
void Jugador::cargarSprite(const QString &ruta)
{
    spriteFull = QPixmap(ruta);
    if (spriteFull.isNull()) {
        // Fallback: cuadrado de color
        QPixmap tmp(60, 60);
        tmp.fill(Qt::blue);
        setPixmap(tmp);
        return;
    }
    fw = spriteFull.width()  / 7;   // 100 px
    fh = spriteFull.height() / 2;   // 178 px
    estado    = IDLE;
    frameAnim = 0;
    aplicarFrameActual();
}

// ── Devuelve el frame (col, fila) listo para mostrar ─────────
QPixmap Jugador::obtenerFrame(int col, int fila)
{
    QPixmap frame = spriteFull.copy(col * fw, fila * fh, fw, fh);
    // Escalar manteniendo proporción dentro de 80x80
    frame = frame.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (mirrorX)
        frame = frame.transformed(QTransform().scale(-1, 1));
    return frame;
}

// ── Aplica el frame que corresponde al estado actual ─────────
void Jugador::aplicarFrameActual()
{
    if (spriteFull.isNull() || fw == 0) return;

    switch (estado) {
    case IDLE:
        // Fila 1, col 0: parado sobre el skate
        setPixmap(obtenerFrame(0, 1));
        break;

    case CORRIENDO:
        // Fila 1, cols 0‒1: ride1 / ride2
        setPixmap(obtenerFrame(frameAnim % 2, 1));
        break;

    case SALTANDO:
        // Fila 1, cols 2‒3: trick1 / trick2
        setPixmap(obtenerFrame(2 + (frameAnim % 2), 1));
        break;

    case ATERRIZANDO:
        // Fila 1, col 4: land (un solo frame)
        setPixmap(obtenerFrame(4, 1));
        break;
    }
}

// ── Avanza la animación (llamar ~10 fps) ─────────────────────
void Jugador::actualizarSprite()
{
    if (spriteFull.isNull()) return;

    contadorAnim++;

    switch (estado) {
    case IDLE:
        // Sin animación (frame fijo)
        break;

    case CORRIENDO:
        // Alterna frame cada 2 ticks (~200ms a 10fps)
        if (contadorAnim >= 2) {
            contadorAnim = 0;
            frameAnim++;
        }
        break;

    case SALTANDO:
        if (contadorAnim >= 2) {
            contadorAnim = 0;
            frameAnim++;
        }
        break;

    case ATERRIZANDO:
        // Después de 3 ticks (~300ms) vuelve a IDLE
        if (contadorAnim >= 3) {
            contadorAnim = 0;
            estado    = IDLE;
            frameAnim = 0;
        }
        break;
    }

    aplicarFrameActual();
}

// ── Movimiento ───────────────────────────────────────────────
void Jugador::moverIzquierda()
{
    velocidadX = -velocidadMaxX;
    mirrorX    = true;
    if (enSueloFlag && estado != CORRIENDO) {
        estado    = CORRIENDO;
        frameAnim = 0;
        contadorAnim = 0;
    }
}

void Jugador::moverDerecha()
{
    velocidadX = velocidadMaxX;
    mirrorX    = false;
    if (enSueloFlag && estado != CORRIENDO) {
        estado    = CORRIENDO;
        frameAnim = 0;
        contadorAnim = 0;
    }
}

void Jugador::detenerHorizontal()
{
    velocidadX = 0;
    if (enSueloFlag && estado == CORRIENDO) {
        estado    = IDLE;
        frameAnim = 0;
        contadorAnim = 0;
        aplicarFrameActual();
    }
}

void Jugador::saltar()
{
    if (enSueloFlag) {
        velocidadY   = fuerzaSalto;
        enSueloFlag  = false;
        estado       = SALTANDO;
        frameAnim    = 0;
        contadorAnim = 0;
        aplicarFrameActual();
    }
}

// ── Física (60 fps) ──────────────────────────────────────────
void Jugador::aplicarFisica()
{
    // Gravedad
    velocidadY += gravedad;
    setY(y() + velocidadY);

    // Piso
    if (y() >= sueloY) {
        setY(sueloY);
        velocidadY = 0;
        if (!enSueloFlag) {
            // Acababa de caer: frame de aterrizaje
            enSueloFlag  = true;
            estado       = ATERRIZANDO;
            frameAnim    = 0;
            contadorAnim = 0;
            aplicarFrameActual();
        }
    }

    // Movimiento horizontal
    setX(x() + velocidadX);

    // Límites laterales
    if (x() < 0)   setX(0);
    if (x() > 710) setX(710);
}

// ── Helpers ──────────────────────────────────────────────────
bool  Jugador::enSuelo() const   { return enSueloFlag; }
void  Jugador::setSueloY(qreal y){ sueloY = y; }
void  Jugador::detenerCaida()
{
    velocidadY  = 0;
    enSueloFlag = true;
    estado      = IDLE;
    frameAnim   = 0;
    aplicarFrameActual();
}
