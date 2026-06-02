#ifndef JUGADOR_H
#define JUGADOR_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>

class Jugador : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Jugador(QGraphicsItem *parent = nullptr);

    void moverIzquierda();
    void moverDerecha();
    void saltar();
    void aplicarFisica();
    bool enSuelo() const;

    void setSueloY(qreal y);
    void detenerCaida();

private:
    qreal velocidadX;
    qreal velocidadY;
    bool enSueloFlag;
    qreal sueloY;
    qreal gravedad;
    qreal fuerzaSalto;
    qreal velocidadMax;
};

#endif
