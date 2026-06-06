#ifndef DRONIA_H
#define DRONIA_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QRandomGenerator>
#include <QtMath>
#include <QVector>
#include <QPainter>
#include <map>

class Jugador;

// ─── Proyectil lanzado por el dron ───────────────────────────
// Usa sprite PNG (Caramelo/Paleta); si no carga, usa elipse
struct Proyectil {
    QGraphicsPixmapItem *itemSprite = nullptr;
    qreal vx = 0.0;
    qreal vy = 0.0;
};

// ─── Charco de gelatina con física de ralentización ──────────
struct Charco {
    QGraphicsPixmapItem *item      = nullptr;
    QRectF               rect;
    int                  ticksVida = 300;
};

// ─────────────────────────────────────────────────────────────
//  Clase DronIA  –  Agente inteligente del Nivel 2
//
//  a) PERCEPCIÓN  → histograma 3×3 de zonas donde está el jugador
//  b) RAZONAMIENTO → elige estrategia: DIRECTO / EMBOSCADA / CORTE
//  c) ACCIÓN      → se mueve + dispara proyectiles (Caramelo/Paleta)
//  d) APRENDIZAJE → tras cada golpe sube velocidad y frecuencia de disparo
// ─────────────────────────────────────────────────────────────
class DronIA : public QObject
{
    Q_OBJECT

public:
    explicit DronIA(QGraphicsScene *scene, Jugador *jugador,
                    int dificultad = 1, QObject *parent = nullptr);
    ~DronIA();

    void iniciar();
    void detener();
    void actualizar();
    void setDificultad(int d);

    void actualizarHUDVidas(int v);
    void actualizarHUDPuntos(int p);

    bool  juegoTerminado()         const { return terminado; }
    bool  jugadorGano()            const { return gano; }
    qreal factorVelocidadJugador() const { return factorVelocidad; }

signals:
    void jugadorMurio();
    void nivelCompletado();
    void tiempoActualizado(int seg);

private slots:
    void tickSegundo();

private:
    QGraphicsScene *scene;
    Jugador        *jugador;
    int             dificultad;

    // ─── Sprites precargados ─────────────────────────────────
    QPixmap pixDronBase;
    QPixmap pixCaramelo;
    QPixmap pixPaleta;
    QPixmap pixCharcoSheet;
    QVector<QRect> charcoFrames;

    static constexpr int DRON_W = 60;
    static constexpr int DRON_H = 60;

    // ─── Visual del dron ─────────────────────────────────────
    QGraphicsPixmapItem *spriteDron = nullptr;

    // ─── Movimiento del dron ─────────────────────────────────
    qreal dronX = 400.0;
    qreal dronY = 100.0;
    qreal velocidadBase   = 2.0;
    qreal velocidadActual = 2.0;

    // ─── Estado ──────────────────────────────────────────────
    int   segundosRestantes = 60;
    bool  terminado = false;
    bool  gano      = false;
    int   ticksInvuln  = 0;
    const int TICKS_INVULN = 60;

    // ─── Aprendizaje ─────────────────────────────────────────
    std::map<int, int> histogramaZonas;
    std::map<int, int> pesosAtaque;
    int zonaPreferida = 4;

    void reforzarAprendizaje(int zona);
    int  zonaDelJugador()    const;
    int  zonaMasFrecuentada() const;
    void elegirEstrategiaMovimiento();

    enum Estrategia { DIRECTO, EMBOSCADA, CORTE };
    Estrategia estrategia   = DIRECTO;
    int   ticksEstrategia   = 0;
    qreal objetivoX = 400;
    qreal objetivoY = 250;

    // ─── Proyectiles ─────────────────────────────────────────
    QList<Proyectil*> proyectiles;
    int ticksProximoDisparo = 0;
    int intervalDisparo     = 120;

    void dispararProyectil();
    void actualizarProyectiles();
    void verificarColisionesProyectiles();

    // ─── Charcos ─────────────────────────────────────────────
    QList<Charco*> charcos;
    int   ticksProximoCharco = 0;
    int   maxCharcos         = 4;
    qreal factorVelocidad    = 1.0;

    // forzarX/forzarY: coordenadas del impacto (-1 = posición aleatoria)
    void generarCharco(qreal forzarX = -1, qreal forzarY = -1);
    void actualizarCharcos();
    void verificarJugadorEnCharco();

    // ─── Coleccionables (sprites Caramelo/Paleta) ────────────
    QList<QGraphicsPixmapItem*> coleccionables;
    int ticksColeccionable = 0;

    void generarColeccionable();
    void verificarColeccionables();

    // ─── Colisión directa dron-jugador ───────────────────────
    void verificarColisionDirecta();

    // ─── HUD ─────────────────────────────────────────────────
    QTimer             *timerSegundo = nullptr;
    QGraphicsTextItem  *textoTiempo  = nullptr;
    QGraphicsRectItem  *fondoTiempo  = nullptr;
    QGraphicsTextItem  *textoVidas   = nullptr;
    QGraphicsTextItem  *textoPuntos  = nullptr;
    QGraphicsTextItem  *textoFin     = nullptr;
    QGraphicsTextItem  *textoSubFin  = nullptr;
    QGraphicsRectItem  *fondoFin     = nullptr;

    void actualizarVisualDron();
    void limpiarHUD();
    void iniciarHUD();
    void mostrarMensajeFin(bool gano);
    void limpiarProyectiles();
    void limpiarCharcos();
};

#endif // DRONIA_H
