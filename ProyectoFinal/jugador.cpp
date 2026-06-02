#include "jugador.h"

Jugador::Jugador(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidadX(0),
    velocidadY(0),
    enSueloFlag(false),
    sueloY(0),
    gravedad(0.8),
    fuerzaSalto(-14),
    velocidadMax(6)
{
}

void Jugador::moverIzquierda()
{
    setX(x() - velocidadMax);
}

void Jugador::moverDerecha()
{
    setX(x() + velocidadMax);
}

void Jugador::saltar()
{
    if (enSueloFlag) {
        velocidadY = fuerzaSalto;
        enSueloFlag = false;
    }
}

void Jugador::aplicarFisica()
{
    velocidadY += gravedad;
    setY(y() + velocidadY);

    if (y() >= sueloY) {
        setY(sueloY);
        velocidadY = 0;
        enSueloFlag = true;
    }
}

bool Jugador::enSuelo() const
{
    return enSueloFlag;
}

void Jugador::setSueloY(qreal y)
{
    sueloY = y;
}

void Jugador::detenerCaida()
{
    velocidadY = 0;
}
