#include "obstaculo.h"

Obstaculo::Obstaculo(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    EntidadJuego(),
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
//  actualizarFisica()  — implementación del contrato EntidadJuego
//
//  LINEAL:      X -= velocidadX
//  PARABOLICO:  Vy += gravedad  →  Y += Vy  (parábola real)
//  OSCILATORIO: Y = yBase + A·sin(ω·t + φ)
// ─────────────────────────────────────────────────────────────
void Obstaculo::actualizarFisica()
{
    tickPropio++;
    switch (modeloFisico) {
    case FISICA_LINEAL:
        setX(x() - velocidadX);
        break;

    case FISICA_PARABOLICA:
        velocidadY += gravedad;
        setX(x() - velocidadX);
        setY(y() + velocidadY);
        if (y() < 80) {
            setY(80);
            velocidadY = qAbs(velocidadY) * 0.5;
        }
        break;

    case FISICA_OSCILATORIA:
        setX(x() - velocidadX);
        setY(yBase + amplitud * qSin(frecuencia * tickPropio + fase));
        break;
    }
}

// ─────────────────────────────────────────────────────────────
//  reiniciar()  — implementación del contrato EntidadJuego
// ─────────────────────────────────────────────────────────────
void Obstaculo::reiniciar()
{
    velocidadX  = 4.0;
    velocidadY  = 0.0;
    tickPropio  = 0;
    activo      = true;
    setOpacity(1.0);
}

bool Obstaculo::fueraDePantalla() const
{
    return x() < -80;
}
