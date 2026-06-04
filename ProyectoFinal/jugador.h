#ifndef JUGADOR_H
#define JUGADOR_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>

class Jugador : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Jugador(QGraphicsItem *parent = nullptr);

    // Acciones
    void moverIzquierda();
    void moverDerecha();
    void detenerHorizontal();
    void saltar();

    // Fisica (llamada cada tick del timer)
    void aplicarFisica();

    // Configuración
    void setSueloY(qreal y);
    void detenerCaida();
    bool enSuelo() const;

    // Sprite: cargar y animar
    void cargarSprite(const QString &ruta);
    void actualizarSprite();   // llamar cada ~100ms para animar

private:
    // Fisica
    qreal velocidadX;
    qreal velocidadY;
    bool  enSueloFlag;
    qreal sueloY;
    const qreal gravedad      = 0.9;
    const qreal fuerzaSalto   = -16.0;
    const qreal velocidadMaxX = 6.0;
    bool  mirrorX;

    // Estados de animacion
    enum Estado { IDLE, CORRIENDO, SALTANDO, ATERRIZANDO };
    Estado estado;
    int    frameAnim;      // frame actual dentro del estado
    int    contadorAnim;   // ticks transcurridos para avanzar frame

    // Spritesheet
    // 7 columnas x 2 filas, cada frame = 100x178 px
    QPixmap spriteFull;
    int fw, fh;            // ancho y alto de cada frame

    // Devuelve el frame (col, fila) escalado y con flip si hace falta
    QPixmap obtenerFrame(int col, int fila);

    void aplicarFrameActual();
};

#endif
