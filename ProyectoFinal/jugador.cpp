#include "jugador.h"
#include <QTransform>

Jugador::Jugador(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidadX(0), velocidadY(0),
    enSueloFlag(true), sueloY(320),
    mirrorX(false),
    estado(IDLE), frameAnim(0), contadorAnim(0),
    fw(0), fh(0)
{}

void Jugador::cargarSprite(const QString &ruta)
{
    spriteFull = QPixmap(ruta);
    if (spriteFull.isNull()) {
        QPixmap tmp(60, 60);
        tmp.fill(Qt::blue);
        setPixmap(tmp);
        return;
    }
    fw = spriteFull.width()  / 7;
    fh = spriteFull.height() / 2;
    estado    = IDLE;
    frameAnim = 0;
    aplicarFrameActual();
}

QPixmap Jugador::obtenerFrame(int col, int fila)
{
    QPixmap frame = spriteFull.copy(col * fw, fila * fh, fw, fh);
    frame = frame.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (mirrorX)
        frame = frame.transformed(QTransform().scale(-1, 1));
    return frame;
}

void Jugador::aplicarFrameActual()
{
    if (spriteFull.isNull() || fw == 0) return;

    // Parpadeo durante invulnerabilidad: ocultar cada 6 ticks
    if (ticksInvulnerable > 0 && (ticksInvulnerable / 6) % 2 == 0) {
        setOpacity(0.3);
    } else {
        setOpacity(1.0);
    }

    switch (estado) {
    case IDLE:
        setPixmap(obtenerFrame(0, 1));
        break;
    case CORRIENDO:
        setPixmap(obtenerFrame(frameAnim % 2, 1));
        break;
    case SALTANDO:
        setPixmap(obtenerFrame(2 + (frameAnim % 2), 1));
        break;
    case ATERRIZANDO:
        setPixmap(obtenerFrame(4, 1));
        break;
    }
}

void Jugador::actualizarSprite()
{
    if (spriteFull.isNull()) return;
    contadorAnim++;

    switch (estado) {
    case IDLE:
        break;
    case CORRIENDO:
        if (contadorAnim >= 2) { contadorAnim = 0; frameAnim++; }
        break;
    case SALTANDO:
        if (contadorAnim >= 2) { contadorAnim = 0; frameAnim++; }
        break;
    case ATERRIZANDO:
        if (contadorAnim >= 3) {
            contadorAnim = 0;
            estado    = IDLE;
            frameAnim = 0;
        }
        break;
    }
    aplicarFrameActual();
}

void Jugador::moverIzquierda()
{
    velocidadX = -velocidadMaxX;
    mirrorX    = true;
    if (enSueloFlag && estado != CORRIENDO) {
        estado = CORRIENDO; frameAnim = 0; contadorAnim = 0;
    }
}

void Jugador::moverDerecha()
{
    velocidadX = velocidadMaxX;
    mirrorX    = false;
    if (enSueloFlag && estado != CORRIENDO) {
        estado = CORRIENDO; frameAnim = 0; contadorAnim = 0;
    }
}

void Jugador::detenerHorizontal()
{
    velocidadX = 0;
    if (enSueloFlag && estado == CORRIENDO) {
        estado = IDLE; frameAnim = 0; contadorAnim = 0;
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

void Jugador::aplicarFisica()
{
    // Bajar contador de invulnerabilidad y actualizar parpadeo a 60 fps
    if (ticksInvulnerable > 0) {
        ticksInvulnerable--;
        aplicarFrameActual();   // refresca la opacidad cada tick, no solo cada 100 ms
    }

    // Gravedad
    velocidadY += gravedad;
    setY(y() + velocidadY);

    // Piso
    if (y() >= sueloY) {
        setY(sueloY);
        velocidadY = 0;
        if (!enSueloFlag) {
            enSueloFlag  = true;
            estado       = ATERRIZANDO;
            frameAnim    = 0;
            contadorAnim = 0;
            aplicarFrameActual();
        }
    }

    // Movimiento horizontal
    setX(x() + velocidadX);
    if (x() < 0)   setX(0);
    if (x() > 710) setX(710);
}

bool Jugador::recibirGolpe()
{
    // Si está en periodo de invulnerabilidad, ignorar el golpe
    if (ticksInvulnerable > 0) return false;

    vidas--;
    ticksInvulnerable = TICKS_INVULN;
    emit vidasCambiaron(vidas);
    return true;
}

void Jugador::recogerItem(int valorPuntos)
{
    puntos += valorPuntos;
    emit puntajeActualizado(puntos);
}

void Jugador::reiniciar()
{
    vidas              = 3;
    puntos             = 0;
    ticksInvulnerable  = 0;
    velocidadX         = 0;
    velocidadY         = 0;
    enSueloFlag        = true;
    estado             = IDLE;
    frameAnim          = 0;
    contadorAnim       = 0;
    setOpacity(1.0);
    aplicarFrameActual();
}

bool  Jugador::enSuelo() const    { return enSueloFlag; }
void  Jugador::setSueloY(qreal y) { sueloY = y; }
void  Jugador::detenerCaida()
{
    velocidadY  = 0;
    enSueloFlag = true;
    estado      = IDLE;
    frameAnim   = 0;
    aplicarFrameActual();
}
