#ifndef INCIDENTE_H
#define INCIDENTE_H

#include <string>
using namespace std;

struct Incidente {
    string id;  // identificador, ej. "INC-102".
    int cantidad_alertas;         // cuántas alertas llegaron a este incidente.
    long long prioridad_total;  // se utiliza para almacenar números enteros mucho más
                          // grandes de lo que permiten un int de 4 bits a 64 bits.

    Incidente(const string& identificador) {
        id = identificador;
        cantidad_alertas = 0;
        prioridad_total = 0;
    }
};

#endif
