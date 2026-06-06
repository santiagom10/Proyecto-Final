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

// ─────────────────────────────────────────────────────────────
//  Clase IAObstaculos  –  Agente inteligente
//
//  Arquitectura del agente (requisito del proyecto):
//
//  a) PERCEPCIÓN  → observa posición del jugador, tiempo restante,
//                   tasa de colisiones reciente y puntuación.
//
//  b) RAZONAMIENTO → decide qué tipo de obstáculo/coleccionable
//                    generar, con qué física y a qué altura,
//                    basándose en el historial de éxito del jugador.
//
//  c) ACCIÓN      → genera el objeto en pantalla con los parámetros
//                   elegidos.
//
//  d) APRENDIZAJE → mantiene un mapa de "tasas de éxito" por
//                   (TipoObstaculo × ModeloFisico). Cada vez que
//                   un obstáculo golpea al jugador se refuerza esa
//                   combinación (la IA aprende qué configuraciones
//                   son más difíciles para este jugador y las
//                   prefiere más frecuentemente).
//
//  Físicas implementadas:
//    FISICA_LINEAL      → obstáculo se mueve horizontalmente
//    FISICA_PARABOLICA  → obstáculo cae con gravedad propia
//    FISICA_OSCILATORIA → obstáculo oscila en Y (seno)
// ─────────────────────────────────────────────────────────────

class IAObstaculos : public QObject
{
    Q_OBJECT

public:
    explicit IAObstaculos(QGraphicsScene *scene, Jugador *jugador, QObject *parent = nullptr);

    void iniciar(int segundosParaGanar = 60);
    void detener();
    void actualizar();     // llamado cada tick del timer principal

    std::vector<Obstaculo*>& obstaculos();

    bool juegoTerminado() const;
    bool jugadorGano()    const;

    // Configuración de dificultad inicial (1=fácil, 2=normal, 3=difícil)
    void setDificultadInicial(int d);

    void setNivelVertical(bool v);

signals:
    void jugadorMurio();
    void nivelCompletado();
    void tiempoActualizado(int segundosRestantes);
    void puntosActualizados(int puntos);   // propagado desde Jugador

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

    bool nivelVertical = false;

    // ─── HUD ─────────────────────────────────────────────
    QGraphicsTextItem  *textoTiempo   = nullptr;
    QGraphicsRectItem  *fondoTiempo   = nullptr;
    QGraphicsTextItem  *textoVidas    = nullptr;
    QGraphicsTextItem  *textoPuntos   = nullptr;
    QGraphicsTextItem  *textoFin      = nullptr;
    QGraphicsTextItem  *textoSubFin   = nullptr;
    QGraphicsRectItem  *fondoFin      = nullptr;

    // ─── Tipos internos ──────────────────────────────────
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

    // Elige la combinación con peso ponderado (softmax simplificado)
    std::pair<TipoObst, ModeloFis> elegirCombinacionAprendida();

    // ─── Métodos internos ────────────────────────────────
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
