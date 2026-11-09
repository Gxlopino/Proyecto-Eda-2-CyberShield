// CyberShield - main.cpp
// Uso: ./cybershield entrada.csv ranking.csv metricas.txt
//
// SUPUESTOS DE INTERFAZ (ajusta los nombres si los tuyos son distintos):
//   Incidente.h   : struct/clase Incidente { string id; int cantidad_alertas; int prioridad_total; }
//   TablaHash.h   : TablaHash(int capacidadInicial)
//                   Incidente* buscarOInsertar(const string& id)   // devuelve puntero al incidente (nuevo con 0,0 o existente)
//                   int getTam()                                    // capacidad actual
//                   Incidente* getPosicion(int i)                         // incidente en cubeta i, o nullptr si esta vacia/eliminada
//                   int getOcupados()                               // u = incidentes unicos
//                   long long getSondeos()
//   MaxHeap.h     : MaxHeap(int capacidad)
//                   void insertar(Incidente* inc)
//                   Incidente* extraerMax()
//                   bool vacio()
//                   long long getComparaciones()
//                   long long getIntercambios()
//   (El MaxHeap debe comparar por prioridad DESC y, en empate, por id ASC.)


#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>

#include "Incidente.h"
#include "TablaHash.h"
#include "MaxHeap.h"

using namespace std;

// ---------- Lectura del CSV (sin vector) ----------
struct Alerta {
    string incidente;
    int severidad;
};

// Quita '\r' final (archivos con fin de linea de Windows)
static void limpiarCR(string& s) {
    if (!s.empty() && s[s.size() - 1] == '\r') s.erase(s.size() - 1);
}

// Convierte string a entero sin usar stoi (sin excepciones)
static int aEntero(const string& s) {
    int r = 0;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] >= '0' && s[i] <= '9') r = r * 10 + (s[i] - '0');
    }
    return r;
}

// Lee el CSV a un arreglo dinamico de alertas. Devuelve el arreglo y deja n en 'cantidad'.
static Alerta* leerCSV(const string& ruta, int& cantidad) {
    cantidad = 0;
    ifstream in(ruta.c_str());
    if (!in.is_open()) return nullptr;

    int capacidad = 1024;
    Alerta* datos = new Alerta[capacidad];

    string linea;
    getline(in, linea); // cabecera: alerta_id,incidente_id,severidad

    while (getline(in, linea)) {
        limpiarCR(linea);
        if (linea.empty()) continue;

        size_t c1 = linea.find(',');
        if (c1 == string::npos) continue;
        size_t c2 = linea.find(',', c1 + 1);
        if (c2 == string::npos) continue;

        if (cantidad == capacidad) { // duplicar capacidad
            int nueva = capacidad * 2;
            Alerta* tmp = new Alerta[nueva];
            for (int i = 0; i < cantidad; i++) tmp[i] = datos[i];
            delete[] datos;
            datos = tmp;
            capacidad = nueva;
        }

        datos[cantidad].incidente = linea.substr(c1 + 1, c2 - c1 - 1);
        datos[cantidad].severidad = aEntero(linea.substr(c2 + 1));
        cantidad++;
    }
    in.close();
    return datos;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Uso: " << argv[0] << " entrada.csv ranking.csv metricas.txt" << endl;
        return 1;
    }
    string rutaEntrada = argv[1];
    string rutaRanking = argv[2];
    string rutaMetricas = argv[3];

    // 0) Verificar desde el inicio que se pueden crear los archivos de salida
    {
        ofstream testR(rutaRanking.c_str());
        ofstream testM(rutaMetricas.c_str());
        if (!testR.is_open() || !testM.is_open()) {
            cerr << "No se pudieron crear los archivos de salida: "
                 << rutaRanking << " / " << rutaMetricas << endl;
            return 1;
        }
    }

    // 1) Lectura (NO entra en el tiempo medido)
    int n = 0;
    Alerta* alertas = leerCSV(rutaEntrada, n);
    if (alertas == nullptr) {
        cerr << "No se pudo abrir: " << rutaEntrada << endl;
        return 1;
    }

    // 2) Consolidacion + ranking (SI entra en el tiempo medido)
    TablaHash tabla(16); // estado inicial vacio en cada ejecucion
    Incidente** ranking = nullptr;
    int u = 0;
    int pos = 0;
    MaxHeap* heap = nullptr;

    auto t0 = chrono::high_resolution_clock::now();

    // 2a) Consolidar alertas en la tabla hash: O(n) esperado
    for (int i = 0; i < n; i++) {
        Incidente* inc = tabla.buscarOInsertar(alertas[i].incidente);
        inc->cantidad_alertas += 1;
        inc->prioridad_total += alertas[i].severidad;
    }
    u = tabla.getOcupados();

    // 2b) Pasar los u incidentes unicos al max-heap: O(u log u)
    heap = new MaxHeap(u > 0 ? u : 1);
    int cap = tabla.getTam();
    for (int i = 0; i < cap; i++) {
        Incidente* inc = tabla.getPosicion(i);
        if (inc != nullptr) heap->insertar(inc);
    }

    // 2c) Extraer en orden: del mas critico al menos critico: O(u log u)
    ranking = new Incidente*[u > 0 ? u : 1];
    while (!heap->estaVacio()) {
        ranking[pos++] = heap->SacarHeap();
    }

    auto t1 = chrono::high_resolution_clock::now();
    double tiempoMs = chrono::duration<double, milli>(t1 - t0).count();

    // 3) Escribir ranking.csv
    ofstream outR(rutaRanking.c_str());
    if (!outR.is_open()) {
        cerr << "No se pudo escribir: " << rutaRanking << endl;
        return 1;
    }
    outR << "posicion,incidente_id,cantidad_alertas,prioridad_total\n";
    for (int i = 0; i < pos; i++) {
        outR << (i + 1) << "," << ranking[i]->id << ","
             << ranking[i]->cantidad_alertas << "," << ranking[i]->prioridad_total << "\n";
    }
    outR.close();

    // 4) Escribir metricas.txt
    ofstream outM(rutaMetricas.c_str());
    if (!outM.is_open()) {
        cerr << "No se pudo escribir: " << rutaMetricas << endl;
        return 1;
    }
    outM << "sondeos_hash=" << tabla.getSondeos() << "\n";
    outM << "comparaciones_heap=" << heap->getComparaciones() << "\n";
    outM << "intercambios_heap=" << heap->getIntercambios() << "\n";
    outM << fixed << setprecision(3) << "tiempo_proceso_ms=" << tiempoMs << "\n";
    outM.close();

    // Resumen por consola (util para llenar la tabla del informe)
    cout << "n=" << n << " u=" << u
         << " sondeos_hash=" << tabla.getSondeos()
         << " comparaciones_heap=" << heap->getComparaciones()
         << " intercambios_heap=" << heap->getIntercambios()
         << " tiempo_ms=" << fixed << setprecision(3) << tiempoMs << endl;

    delete[] ranking;
    delete heap;
    delete[] alertas;
    return 0;
}
