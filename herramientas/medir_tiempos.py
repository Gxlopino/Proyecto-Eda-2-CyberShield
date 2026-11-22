#!/usr/bin/env python3
"""Mide CyberShield desde fuera, sin modificar ni compilar su codigo C++.

Ejemplo desde la raiz del proyecto:
python herramientas/medir_tiempos.py --exe bin/Release/CyberShield.exe

Solo utiliza la biblioteca estandar de Python 3.
"""

import argparse
import csv
import statistics
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path


def leer_csv(ruta):
    with ruta.open(encoding="utf-8-sig", newline="") as archivo:
        return list(csv.reader(archivo))


def guardar_csv(ruta, filas):
    with ruta.open("w", encoding="utf-8-sig", newline="") as archivo:
        escritor = csv.DictWriter(archivo, fieldnames=list(filas[0]))
        escritor.writeheader()
        escritor.writerows(filas)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", required=True, type=Path,
                        help="Ejecutable CyberShield que ya compilaste.")
    parser.add_argument("--datos", type=Path, default=Path("datos"),
                        help="Carpeta de CSV de entrada (predeterminado: datos).")
    parser.add_argument("--esperados", type=Path, default=Path("esperados"),
                        help="Referencias opcionales nombre.esperado.csv.")
    parser.add_argument("--salida", type=Path, default=Path("resultados_mediciones"))
    parser.add_argument("--repeticiones", type=int, default=3)
    args = parser.parse_args()

    if args.repeticiones < 3:
        parser.error("Usa al menos 3 repeticiones para las mediciones.")
    exe = args.exe.resolve()
    datos = args.datos.resolve()
    if not exe.is_file():
        parser.error("No existe el ejecutable: " + str(exe))
    if not datos.is_dir():
        parser.error("No existe la carpeta de entrada: " + str(datos))
    casos = sorted(datos.glob("*.csv"))
    if not casos:
        parser.error("No hay archivos CSV en " + str(datos))

    # Una carpeta nueva evita confundir resultados de sesiones anteriores.
    ahora = datetime.now().astimezone()
    sesion = args.salida.resolve() / ahora.strftime("medicion_%Y%m%d_%H%M%S_%f")
    sesion.mkdir(parents=True, exist_ok=False)
    muestras = []
    resumen = []

    for caso in casos:
        n = max(0, len(leer_csv(caso)) - 1)
        esperado = args.esperados.resolve() / (caso.stem + ".esperado.csv")
        referencia = leer_csv(esperado) if esperado.is_file() else None
        ejecuciones = []

        for repeticion in range(1, args.repeticiones + 1):
            nombre = caso.stem + "_r" + str(repeticion)
            ranking = sesion / (nombre + "_ranking.csv")
            metricas = sesion / (nombre + "_metricas.txt")

            # INICIO DEL INTERVALO EXTERNO: antes de lanzar el programa.
            inicio = time.perf_counter()
            proceso = subprocess.run(
                [str(exe), str(caso), str(ranking), str(metricas)],
                capture_output=True,
            )
            tiempo_externo_ms = (time.perf_counter() - inicio) * 1000
            # FIN: subprocess.run ya espero a que terminara el proceso.

            # Guardar, leer y comparar se hace DESPUES del intervalo medido.
            (sesion / (nombre + "_stdout.txt")).write_bytes(proceso.stdout)
            (sesion / (nombre + "_stderr.txt")).write_bytes(proceso.stderr)
            if proceso.returncode != 0:
                raise RuntimeError(
                    "Fallo " + caso.name + " (codigo " + str(proceso.returncode)
                    + "). Revisa los archivos de consola en " + str(sesion))
            valores = dict(linea.split("=", 1) for linea in
                           metricas.read_text(encoding="utf-8-sig").splitlines()
                           if linea.strip())
            resultado = leer_csv(ranking)
            if referencia is not None and resultado != referencia:
                raise RuntimeError("Ranking distinto al esperado: " + caso.name)
            fila = {
                "caso": caso.stem,
                "repeticion": repeticion,
                "n": n,
                "u": max(0, len(resultado) - 1),
                "sondeos_hash": int(valores["sondeos_hash"]),
                "comparaciones_heap": int(valores["comparaciones_heap"]),
                "intercambios_heap": int(valores["intercambios_heap"]),
                "tiempo_interno_ms": float(valores["tiempo_proceso_ms"]),
                "tiempo_externo_ms": round(tiempo_externo_ms, 6),
                "validacion": "CORRECTO" if referencia is not None else "SIN_REFERENCIA",
            }
            muestras.append(fila)
            ejecuciones.append(fila)
            # Se conserva cada muestra aunque una ejecucion posterior falle.
            guardar_csv(sesion / "muestras.csv", muestras)
            print(caso.name, "repeticion", repeticion,
                  "interno:", fila["tiempo_interno_ms"], "ms",
                  "externo:", fila["tiempo_externo_ms"], "ms", fila["validacion"])

        resumen.append({
            "caso": caso.stem,
            "n": n,
            "u": ejecuciones[0]["u"],
            "repeticiones": args.repeticiones,
            "mediana_interna_ms": statistics.median(
                fila["tiempo_interno_ms"] for fila in ejecuciones),
            "mediana_externa_ms": statistics.median(
                fila["tiempo_externo_ms"] for fila in ejecuciones),
            "validacion": ejecuciones[0]["validacion"],
        })
        guardar_csv(sesion / "resumen_medianas.csv", resumen)

    print("\nMedicion terminada. Resultados en:", sesion)
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, KeyError, RuntimeError) as error:
        print("ERROR:", error, file=sys.stderr)
        sys.exit(1)
