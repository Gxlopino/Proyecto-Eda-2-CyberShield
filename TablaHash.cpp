#include <iostream>
#include "TablaHash.h"
using namespace std;

    TablaHash::TablaHash(int n) {
        tam = n;
        ocupados = 0;
        sondeos=0;
        tabla = new Incidente*[tam];
        for (int i = 0; i < tam; i++)
            tabla[i] = nullptr;
    }

    TablaHash::~TablaHash() {
        for(int i=0;i<tam;i++){
            if(tabla[i]!=nullptr){
                delete tabla[i];
            }
        }
        delete[] tabla;
    }

    long long TablaHash::getSondeos() const {
        return sondeos;
    }

    int TablaHash::getTam() const {
        return tam;
    }

    int TablaHash::getOcupados() const {
        return ocupados;
    }

    Incidente* TablaHash::getPosicion(int i) const {
        return tabla[i];
    }

    unsigned long TablaHash::funcionHash(const string& id) {
        unsigned long h = 0;
        for (char c : id) {
            h=h*31+c;
        }
        return h % tam;
    }

    Incidente* TablaHash::buscarOInsertar(const string& id){
        if(ocupados*10>tam*5){
            redimensionar();
        }
        int pos=funcionHash(id);
        while(tabla[pos]!=nullptr){
            sondeos++;
            if(tabla[pos]->id==id){
                return tabla[pos];
            }
            pos=(pos+1)%tam;
        }
        sondeos++;
        tabla[pos]=new Incidente(id);
        ocupados++;
        return tabla[pos];
    }

    void TablaHash::print() {
        for (int i = 0; i < tam; i++) {
            if (tabla[i] != nullptr)
                cout << "TablaHash[" << i << "]: " << tabla[i]->id
                     << " | alertas: " << tabla[i]->cantidad_alertas
                     << " | prioridad: " << tabla[i]->prioridad_total << endl;
            else
                cout << "TablaHash[" << i << "]: VACIO" << endl;
        }
    }

    double TablaHash::FactorCarga() const {
        return (double)ocupados / tam;
    }

    void TablaHash::redimensionar() {
        int tamViejo = tam;
        Incidente** tablaVieja = tabla;
        tam = tam * 2+1;
        tabla = new Incidente*[tam];
        for (int i = 0; i < tam; i++) {
        tabla[i] = nullptr;
        }
        for (int i = 0; i < tamViejo; i++){
            if(tablaVieja[i]!=nullptr){
                int pos=funcionHash(tablaVieja[i]->id);
                while(tabla[pos]!=nullptr){
                   pos=(pos+1)%tam;
                }
                tabla[pos]=tablaVieja[i];
            }
        }
        delete[] tablaVieja;
    }
