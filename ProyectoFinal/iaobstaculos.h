#ifndef IAOBSTACULOS_H
#define IAOBSTACULOS_H

#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <vector>
#include <map>
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

    // Configuración de dificultad inicial (1=fácil, 2=normal, 3=difícil)
    void setDificultadInicial(int d);

signals:
    void jugadorMurio();
    void nivelCompletado();
    void tiempoActualizado(int segundosRestantes);
    void puntosActualizados(int puntos);

private slots:
    void decidirObstaculo();
    void tickSegundo();

private:
    // ─── Referencias externas ────────────────────────────
    QGraphicsScene          *scene;
    Jugador                 *jugador;
    QTimer                  *timerDecision;
    QTimer                  *timerSegundo;
    std::vector<Obstaculo*>  listaObstaculos;

    // ─── Estado del juego ────────────────────────────────
    int  dificultadInicial;
    int  nivelDificultad;
    int  ticksTranscurridos;
    int  intervalMinimo;
    int  intervalMaximo;
    int  segundosRestantes;
    bool terminado;
    bool gano;

    QGraphicsTextItem  *textoTiempo   = nullptr;
    QGraphicsRectItem  *fondoTiempo   = nullptr;
    QGraphicsTextItem  *textoVidas    = nullptr;
    QGraphicsTextItem  *textoPuntos   = nullptr;
    QGraphicsTextItem  *textoFin      = nullptr;
    QGraphicsTextItem  *textoSubFin   = nullptr;
    QGraphicsRectItem  *fondoFin      = nullptr;

    enum TipoObst { OBS_GALLETA, OBS_CARAMELO, OBS_PALETA };
    enum ModeloFis { FIS_LINEAL, FIS_PARABOLICA, FIS_OSCILATORIA };

    // ─── Aprendizaje del agente ──────────────────────────
    // Clave: (TipoObst * 10 + ModeloFis)
    // Valor: peso acumulado (más alto = más eficaz contra el jugador)
    std::map<int, double> pesosCombinacion;

    // Tasa de colisiones en la última ventana de 10 obstáculos
    int obstaculosEnVistaReciente = 0;
    int colisionesRecientes       = 0;

    // Registra que una combinación fue efectiva (golpeó al jugador)
    void reforzarAprendizaje(TipoObst tipo, ModeloFis modelo);

    std::pair<TipoObst, ModeloFis> elegirCombinacionAprendida();

    void generarObstaculo(TipoObst tipo, ModeloFis modelo, qreal x, qreal y, bool esColeccionable);
    void limpiarObstaculosFueraDePantalla();
    void ajustarDificultad();
    void verificarColisiones();
    void actualizarHUD();
    void mostrarMensajeFin(bool gano);
    void limpiarHUD();

    int      decidirIntervalo();
    qreal    decidirAltura(ModeloFis modelo);
    bool     debeSerColeccionable();
};

#endif
