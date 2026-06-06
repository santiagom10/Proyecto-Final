#ifndef JUGADOR_H
#define JUGADOR_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>
#include "entidadjuego.h"

class Jugador : public QObject, public QGraphicsPixmapItem, public EntidadJuego
{
    Q_OBJECT
public:
    explicit Jugador(QGraphicsItem *parent = nullptr);

    // Acciones
    void moverIzquierda();
    void moverDerecha();
    void detenerHorizontal();
    void saltar();

    void actualizarFisica() override;
    void reiniciar()        override;

    void setSueloY(qreal y);
    void detenerCaida();
    bool enSuelo() const;

    void cargarSprite(const QString &ruta);
    void actualizarSprite();
    void actualizarInvulnerabilidad();
    void setModoPatineta(bool activo);
    void animarMovimientoTopDown(bool moviendo);
    void mirarIzquierda();
    void mirarDerecha();

    int  getVidas()   const { return vidas; }
    int  getPuntos()  const { return puntos; }

    bool recibirGolpe();

    void recogerItem(int valorPuntos = 10);

    bool estaVivo() const { return vidas > 0; }

    void aplicarFisica() { actualizarFisica(); }

signals:
    void vidasCambiaron(int nuevasVidas);
    void puntajeActualizado(int nuevoPuntaje);

private:
    bool  enSueloFlag;
    qreal sueloY;
    const qreal gravedad      = 0.9;
    const qreal fuerzaSalto   = -16.0;
    const qreal velocidadMaxX = 6.0;
    bool  mirrorX;
    bool modoPatineta = true;

    int vidas  = 3;
    int puntos = 0;

    int  ticksInvulnerable  = 0;
    const int TICKS_INVULN  = 90;

    enum Estado { IDLE, CORRIENDO, SALTANDO, ATERRIZANDO };
    Estado estado;
    int    frameAnim;
    int    contadorAnim;

    QPixmap spriteFull;
    int fw, fh;
    QPixmap obtenerFrame(int col, int fila);
    void    aplicarFrameActual();
};

#endif
