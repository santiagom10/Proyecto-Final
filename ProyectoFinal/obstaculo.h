#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QString>
#include <QtMath>
#include "entidadjuego.h"

class Obstaculo : public QObject, public QGraphicsPixmapItem, public EntidadJuego
{
    Q_OBJECT
public:
    explicit Obstaculo(QGraphicsItem *parent = nullptr);

    void cargarSprite(const QString &ruta, int ancho = 60, int alto = 60);

    enum Tipo { GALLETA, CARAMELO, PALETA };
    Tipo tipo;

    bool esColeccionable = false;

    // Modelos físicos
    enum ModeloFisico { FISICA_LINEAL, FISICA_PARABOLICA, FISICA_OSCILATORIA };
    ModeloFisico modeloFisico = FISICA_LINEAL;

    // Parabólico
    qreal gravedad    = 0.4;

    // Oscilatorio
    qreal yBase       = 280.0;
    qreal amplitud    = 60.0;
    qreal frecuencia  = 0.05;
    qreal fase        = 0.0;
    int   tickPropio  = 0;

    void actualizarFisica() override;
    void reiniciar()        override;

    bool fueraDePantalla() const;
};

#endif
