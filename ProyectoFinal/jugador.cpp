#include "jugador.h"
#include <QTransform>
#include <stdexcept>   // ← para std::runtime_error

Jugador::Jugador(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    EntidadJuego(),
    enSueloFlag(true), sueloY(320),
    mirrorX(false),
    estado(IDLE), frameAnim(0), contadorAnim(0),
    fw(0), fh(0)
{}

void Jugador::cargarSprite(const QString &ruta)
{
    spriteFull = QPixmap(ruta);

    // ── EXCEPCIÓN 1 ───────────────────────────────────────────
    //  Si la ruta del sprite no existe o está mal escrita,
    //  lanzamos una excepción descriptiva en vez de dejar
    //  al jugador invisible o con un cuadro azul.
    if (spriteFull.isNull()) {
        throw std::runtime_error(
            std::string("No se pudo cargar el sprite del jugador: ")
            + ruta.toStdString()
            );
    }

    fw = spriteFull.width()  / 7;
    fh = spriteFull.height() / 2;
    estado    = IDLE;
    frameAnim = 0;
    aplicarFrameActual();
}

// Agregue esto
void Jugador::actualizarInvulnerabilidad()
{
    if (ticksInvulnerable > 0)
    {
        ticksInvulnerable--;
        aplicarFrameActual();
    }
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
    if (spriteFull.isNull() || fw == 0)
        return;

    if (ticksInvulnerable > 0 && (ticksInvulnerable / 6) % 2 == 0)
        setOpacity(0.3);
    else
        setOpacity(1.0);

    switch (estado)
    {
    case IDLE:

        if(modoPatineta)
        {
            // Patineta quieta
            setPixmap(obtenerFrame(0,1));
        }
        else
        {
            // Quieto parado
            setPixmap(obtenerFrame(0,0));
        }

        break;

    case CORRIENDO:

        if(modoPatineta)
        {
            // Ride1 ↔ Ride2
            if(frameAnim % 2 == 0)
                setPixmap(obtenerFrame(0,1));
            else
                setPixmap(obtenerFrame(1,1));
        }
        else
        {
            // Walk1 ↔ Walk2
            if(frameAnim % 2 == 0)
                setPixmap(obtenerFrame(1,0));
            else
                setPixmap(obtenerFrame(2,0));
        }

        break;

    case SALTANDO:

        // Sprite de salto
        setPixmap(obtenerFrame(5,0));

        break;

    case ATERRIZANDO:

        // Sprite de aterrizaje
        setPixmap(obtenerFrame(4,1));

        break;
    }
}

void Jugador::actualizarSprite()
{
    if (spriteFull.isNull())
        return;

    contadorAnim++;

    switch (estado)
    {
    case IDLE:
        break;

    case CORRIENDO:

        if(contadorAnim >= 4)
        {
            contadorAnim = 0;
            frameAnim++;
        }

        break;

    case SALTANDO:
        break;

    case ATERRIZANDO:

        if(contadorAnim >= 8)
        {
            estado = IDLE;
            contadorAnim = 0;
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
void Jugador::setModoPatineta(bool activo)
{
    modoPatineta = activo;
}
void Jugador::mirarIzquierda()
{
    mirrorX = true;
    aplicarFrameActual();
}

void Jugador::mirarDerecha()
{
    mirrorX = false;
    aplicarFrameActual();
}
void Jugador::animarMovimientoTopDown(bool moviendo)
{
    if(moviendo)
    {
        if(estado != CORRIENDO)
        {
            estado = CORRIENDO;
            frameAnim = 0;
            contadorAnim = 0;
        }
    }
    else
    {
        if(estado != IDLE)
        {
            estado = IDLE;
            frameAnim = 0;
            contadorAnim = 0;
        }
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

void Jugador::actualizarFisica()
{
    if (ticksInvulnerable > 0) {
        ticksInvulnerable--;
        aplicarFrameActual();
    }

    velocidadY += gravedad;
    setY(y() + velocidadY);

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

    setX(x() + velocidadX);
    if (x() < 0)   setX(0);
    if (x() > 710) setX(710);
}

bool Jugador::recibirGolpe()
{
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
    activo             = true;
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
