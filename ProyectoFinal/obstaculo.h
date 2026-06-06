#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QString>
#include <QtMath>
#include "entidadjuego.h"

// ─────────────────────────────────────────────────────────────
//  Clase Obstaculo
//
//  Herencia:
//    EntidadJuego  ← clase base PROPIA (contrato de física)
//    QObject       ← para señales/slots Qt
//    QGraphicsPixmapItem ← para renderizado en escena
//
//  Representa tanto obstáculos (dañinos) como coleccionables
//  con tres modelos físicos distintos:
//
//  FISICA_LINEAL      → movimiento horizontal constante
//  FISICA_PARABOLICA  → lanzamiento parabólico (Vy creciente)
//  FISICA_OSCILATORIA → movimiento sinusoidal en Y
// ─────────────────────────────────────────────────────────────
class Obstaculo : public QObject, public QGraphicsPixmapItem, public EntidadJuego
{
    Q_OBJECT
public:
    explicit Obstaculo(QGraphicsItem *parent = nullptr);

    void cargarSprite(const QString &ruta, int ancho = 60, int alto = 60);

    // Tipo de sprite / propósito
    enum Tipo { GALLETA, CARAMELO, PALETA };
    Tipo tipo;

    // ¿Es coleccionable (true) u obstáculo dañino (false)?
    bool esColeccionable = false;

    // ─── Modelos físicos ──────────────────────────────────────
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

    // Implementación del contrato de EntidadJuego
    void actualizarFisica() override;
    void reiniciar()        override;

    // Indica si ya salió de pantalla por la izquierda
    bool fueraDePantalla() const;
};

#endif // OBSTACULO_H
