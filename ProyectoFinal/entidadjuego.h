#ifndef ENTIDADJUEGO_H
#define ENTIDADJUEGO_H

#include <QtGlobal>

class EntidadJuego
{
public:
    EntidadJuego();
    virtual ~EntidadJuego() = default;

    // ── Física (cada subclase implementa su propia lógica) ────
    virtual void actualizarFisica() = 0;
    virtual void reiniciar()        = 0;

    // ── Estado general ────────────────────────────────────────
    bool estaActivo() const    { return activo; }
    void setActivo(bool valor) { activo = valor; }

    // ── Velocidades públicas (IAObstaculos las asigna directo)
    qreal velocidadX = 0.0;
    qreal velocidadY = 0.0;

protected:
    bool activo = true;
};

#endif
