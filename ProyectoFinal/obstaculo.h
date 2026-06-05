#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QString>
#include <QtMath>

// ─────────────────────────────────────────────────────────────
//  Clase Obstaculo
//
//  Representa tanto obstáculos (dañinos) como coleccionables
//  (beneficiosos) con tres modelos físicos distintos:
//
//  FISICA_LINEAL      → movimiento horizontal constante
//  FISICA_PARABOLICA  → lanzamiento parabólico (Vy creciente)
//  FISICA_OSCILATORIA → movimiento sinusoidal en Y
// ─────────────────────────────────────────────────────────────

class Obstaculo : public QObject, public QGraphicsPixmapItem
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

    // ─── Modelos físicos ───────────────────────────────────
    enum ModeloFisico { FISICA_LINEAL, FISICA_PARABOLICA, FISICA_OSCILATORIA };
    ModeloFisico modeloFisico = FISICA_LINEAL;

    // Parámetros compartidos
    qreal velocidadX = 4.0;   // píxeles/tick hacia la izquierda

    // Parabólico: el obstáculo cae con aceleración (gravedad propia)
    qreal velocidadY  = 0.0;
    qreal gravedad    = 0.4;

    // Oscilatorio: senoide en Y alrededor de una posición central
    qreal yBase       = 280.0; // posición Y central
    qreal amplitud    = 60.0;  // píxeles de amplitud
    qreal frecuencia  = 0.05;  // radianes/tick
    qreal fase        = 0.0;   // fase inicial
    int   tickPropio  = 0;     // contador de ticks para este objeto

    // Avanza la física un tick (llamado desde IAObstaculos::actualizar)
    void actualizarFisica();

    // Indica si ya salió de pantalla por la izquierda
    bool fueraDePantalla() const;
};

#endif
