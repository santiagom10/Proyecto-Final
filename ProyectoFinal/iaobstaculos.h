#ifndef IAOBSTACULOS_H
#define IAOBSTACULOS_H

#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <vector>
#include <QRandomGenerator>

class Obstaculo;
class Jugador;

class IAObstaculos : public QObject
{
    Q_OBJECT

public:
    explicit IAObstaculos(QGraphicsScene *scene, Jugador *jugador, QObject *parent = nullptr);

    void iniciar(int segundosParaGanar = 60);
    void detener();
    void actualizar();

    std::vector<Obstaculo*>& obstaculos();

    bool juegoTerminado() const;
    bool jugadorGano()    const;

signals:
    void jugadorMurio();
    void nivelCompletado();
    void tiempoActualizado(int segundosRestantes);

private slots:
    void decidirObstaculo();
    void tickSegundo();

private:
    QGraphicsScene          *scene;
    Jugador                 *jugador;
    QTimer                  *timerDecision;
    QTimer                  *timerSegundo;
    std::vector<Obstaculo*>  listaObstaculos;

    int  nivelDificultad;
    int  ticksTranscurridos;
    int  intervalMinimo;
    int  intervalMaximo;
    int  segundosRestantes;
    bool terminado;
    bool gano;

    QGraphicsTextItem  *textoTiempo  = nullptr;
    QGraphicsRectItem  *fondoTiempo  = nullptr;

    enum TipoObstaculo { GALLETA, CARAMELO, PALETA };

    void generarObstaculo(TipoObstaculo tipo, qreal x, qreal y);
    void limpiarObstaculosFueraDePantalla();
    void ajustarDificultad();
    void verificarColisiones();
    void actualizarHUD();
    void mostrarMensajeFin(bool gano);

    int           decidirIntervalo();
    qreal         decidirAltura();
    TipoObstaculo decidirTipo();
};

#endif
