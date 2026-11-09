#ifndef MAXHEAP_H
#define MAXHEAP_H
#include "Incidente.h"

class MaxHeap {
private:
    Incidente** heap;
    int tam;
    int last;
    long long comparaciones;
    long long intercambios;
    bool tieneMayorPrioridad(Incidente* a, Incidente* b);
    void intercambiar(int i , int j);

public:
    MaxHeap(int n);
    ~MaxHeap();

    void insertar(Incidente* n);
    Incidente* SacarHeap();
    bool estaVacio();
    long long getComparaciones();
    long long getIntercambios();
    void print();

};

#endif
