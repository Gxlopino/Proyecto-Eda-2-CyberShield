#ifndef TABLAHASH_H
#define TABLAHASH_H
#include "Incidente.h"

class TablaHash {
private:
    Incidente** tabla;
    int tam;
    int ocupados;
    long long sondeos;
    unsigned long funcionHash(const string& id);
    void redimensionar();
public:
    TablaHash(int n);
    ~TablaHash();
    long long getSondeos() const;
    int getOcupados() const;
    int getTam() const;
    Incidente* getPosicion(int i) const;
    Incidente* buscarOInsertar(const string& id);
    double FactorCarga() const;
    void print();
};

#endif
