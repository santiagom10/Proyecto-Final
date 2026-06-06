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

// ─────────────────────────────────────────────────────────────
//  actualizarFisica()
//
//  Aplica el modelo físico elegido cada tick de juego.
//
//  LINEAL:      X -= velocidadX         (base, movimiento uniforme)
//  PARABOLICO:  X -= velocidadX         (misma X)
//               Vy += gravedad          (aceleración constante → parabola)
//               Y  += Vy
//  OSCILATORIO: X -= velocidadX
//               Y  = yBase + A·sin(ω·t + φ)   (movimiento armónico)
// ─────────────────────────────────────────────────────────────
void Obstaculo::actualizarFisica()
{
    tickPropio++;

    switch (modeloFisico) {

    case FISICA_LINEAL:
        setX(x() - velocidadX);
        break;

    case FISICA_PARABOLICA:
    {
        velocidadY += gravedad;

        setX(x() - velocidadX);
        setY(y() + velocidadY);

        break;
    }

    case FISICA_OSCILATORIA:
        // Movimiento Armónico Simple en Y
        // y(t) = yBase + A * sin(ω*t + φ)
        setX(x() - velocidadX);
        setY(yBase + amplitud * qSin(frecuencia * tickPropio + fase));
        break;
    }
}

bool Obstaculo::fueraDePantalla() const
{
    return x() < -80 || y() > 600;
}
