#ifndef JUGADOR_H
#define JUGADOR_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>

// ─────────────────────────────────────────────────────────────
//  Clase Jugador
//
//  Maneja movimiento, animación, física y estado del personaje.
//  Se agrega:
//    - Sistema de vidas (3 por defecto)
//    - Puntuación
//    - Frames de invulnerabilidad tras golpe (parpadeo)
//    - señales para HUD
// ─────────────────────────────────────────────────────────────

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
    void actualizarSprite();

    // ─── Sistema de vidas y puntuación ───────────────────
    int  getVidas()   const { return vidas; }
    int  getPuntos()  const { return puntos; }

    // Llamar cuando el jugador choca con un obstáculo dañino
    // Retorna true si el golpe fue efectivo (no está en invulnerabilidad)
    bool recibirGolpe();

    // Llamar cuando el jugador recoge un coleccionable
    void recogerItem(int valorPuntos = 10);

    // Reiniciar estado para nuevo nivel/partida
    void reiniciar();

    bool estaVivo() const { return vidas > 0; }

signals:
    void vidasCambiaron(int nuevasVidas);
    void puntajeActualizado(int nuevoPuntaje);

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

    // Vidas y puntos
    int vidas  = 3;
    int puntos = 0;

    // Invulnerabilidad temporal tras golpe (en ticks a 60fps)
    int  ticksInvulnerable  = 0;
    const int TICKS_INVULN  = 90;  // 1.5 segundos

    // Estados de animacion
    enum Estado { IDLE, CORRIENDO, SALTANDO, ATERRIZANDO };
    Estado estado;
    int    frameAnim;
    int    contadorAnim;

    // Spritesheet
    QPixmap spriteFull;
    int fw, fh;

    QPixmap obtenerFrame(int col, int fila);
    void    aplicarFrameActual();
};

#endif
