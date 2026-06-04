#include "obstaculo.h"

Obstaculo::Obstaculo(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    tipo(GALLETA)
{}

void Obstaculo::cargarSprite(const QString &ruta, int ancho, int alto)
{
    QPixmap px(ruta);
    if (!px.isNull()) {
        setPixmap(px.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        QPixmap fallback(ancho, alto);
        switch (tipo) {
        case GALLETA:  fallback.fill(QColor(139, 90,  43)); break;
        case CARAMELO: fallback.fill(QColor(255, 80,  80)); break;
        case PALETA:   fallback.fill(QColor(255, 200,  0)); break;
        }
        setPixmap(fallback);
    }
}
