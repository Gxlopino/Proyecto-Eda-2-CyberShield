#!/usr/bin/env python3
import csv, sys
from pathlib import Path

def read_csv(path):
    with open(path, encoding='utf-8-sig', newline='') as f:
        return list(csv.reader(f))

if __name__ == '__main__':
    if len(sys.argv)!=3:
        print('Uso: python3 comparar.py esperado.csv resultado.csv')
        sys.exit(2)
    a,b=map(Path,sys.argv[1:])
    if not b.exists():
        print('ERROR: no existe el archivo de resultados:',b)
        sys.exit(1)
    exp,got=read_csv(a),read_csv(b)
    if exp==got:
        print(f'CORRECTO: {len(exp)-1} incidentes y ranking identico.')
        sys.exit(0)
    print(f'INCORRECTO: esperado {len(exp)-1} filas, recibido {len(got)-1} filas.')
    for i in range(max(len(exp),len(got))):
        x=exp[i] if i<len(exp) else None
        y=got[i] if i<len(got) else None
        if x!=y:
            print('Primera diferencia, linea',i+1,'esperado=',x,'obtenido=',y)
            break
    sys.exit(1)
