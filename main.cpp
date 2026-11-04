#include <iostream>
#include "TablaHash.h"
#include "MaxHeap.h"
using namespace std;

int main() {
TablaHash t(5);
t.buscarOInsertar("INC-101");
t.buscarOInsertar("INC-102");
t.buscarOInsertar("INC-103");
t.buscarOInsertar("INC-104");
t.buscarOInsertar("INC-105");
t.print();
cout <<"Ocupados: "<<t.getUsados() << " - Sondeos: " << t.getCapacidad() << endl;
}
