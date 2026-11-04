#include <iostream>
#include "MaxHeap.h"
using namespace std;

MaxHeap::MaxHeap(int n) {
    tam = n;
    heap = new int[n];
    last = -1;
}

MaxHeap::~MaxHeap() {
    delete[] heap;
}

void MaxHeap::print() {
    for (int i = 0; i < last + 1; i++)
        cout << heap[i] << " - ";
}

void MaxHeap::insert(int n) {
    if (last + 1 >= tam) {
        cout << "Heap lleno" << endl;
        return;
    }
    last++;
    heap[last] = n;
    int i = last;
    while (i > 0 && heap[i] > heap[(i - 1) / 2]) {
        swap(heap[i], heap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void MaxHeap::SacarHeap() {
    // TODO: aquí va tu SacarHeap del myHeap, pero invertido a MAX
}
