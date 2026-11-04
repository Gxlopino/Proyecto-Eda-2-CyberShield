#ifndef MAXHEAP_H
#define MAXHEAP_H
#include "Incidente.h"

class MaxHeap {
private:
    int* heap;
    int tam;
    int last;

public:
    MaxHeap(int n);
    ~MaxHeap();

    void insert(int n);
    void print();
    void SacarHeap();
};

#endif
