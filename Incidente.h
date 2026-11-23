#ifndef INCIDENTE_H
#define INCIDENTE_H

#include <string>
using namespace std;

struct Incidente {
    string id;
    int cantidad_alertas;
    long long prioridad_total;

    Incidente(const string& identificador) {
        id = identificador;
        cantidad_alertas = 0;
        prioridad_total = 0;
    }
};

#endif
