#include <iostream>
#include "MaxHeap.h"
using namespace std;

MaxHeap::MaxHeap(int n) {
    tam = n;
    heap = new Incidente* [n];
    last = -1;
    comparaciones=0;
    intercambios=0;
}

MaxHeap::~MaxHeap() {
    delete[] heap;
}

void MaxHeap::print() {
    for (int i = 0; i < last + 1; i++)
        cout << heap[i] << " - ";
}

void MaxHeap::insertar(Incidente* n) {
    if (last + 1 >= tam) {
        cout << "Heap lleno" << endl;
        return;
    }
    last++;
    heap[last] = n;
    int i = last;
    while (i > 0 && tieneMayorPrioridad(heap[i],heap[(i-1)/2])) {
        intercambiar(i,(i - 1) / 2);
        i = (i - 1) / 2;
    }
}

void MaxHeap::intercambiar(int i, int j) {
    Incidente* temp = heap[i];
    heap[i]=heap[j];
    heap[j]= temp;
    intercambios++;
}

bool MaxHeap::tieneMayorPrioridad(Incidente* a, Incidente* b) {
    comparaciones++;
    if(a->prioridad_total!=b->prioridad_total){
        return(a->prioridad_total>b->prioridad_total);
    }else{
        return(a->id<b->id);
    }
}
bool MaxHeap::estaVacio() {
    return last < 0;
}

long long MaxHeap::getComparaciones() {
    return comparaciones;
}

long long MaxHeap::getIntercambios()  {
    return intercambios;
}

Incidente* MaxHeap::SacarHeap() {
    if(last<0){
        return nullptr;
    }

    Incidente* root = heap[0];
    heap[0]=heap[last];

    last--;
    int i=0;

    while(2*i+1 <= last){
        int Hizq=2*i+1;
        int Hder=2*i+2;
        int mayor=Hizq;

        if(Hder<=last && tieneMayorPrioridad(heap[Hder],heap[mayor])){
            mayor=Hder;
        }

        if(!tieneMayorPrioridad(heap[mayor],heap[i])){
            break;

        }

        intercambiar(i,mayor);
        i=mayor;
    }

    return root;
}
