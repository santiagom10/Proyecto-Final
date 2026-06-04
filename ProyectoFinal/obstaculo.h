#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QString>

class Obstaculo : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Obstaculo(QGraphicsItem *parent = nullptr);

    void cargarSprite(const QString &ruta, int ancho = 60, int alto = 60);

    enum Tipo { GALLETA, CARAMELO, PALETA };
    Tipo tipo;
};

#endif
