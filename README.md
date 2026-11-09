# CyberShield

Procesamiento y ranking eficiente de alertas de ciberseguridad.
Proyecto grupal de **Estructuras de Datos y Algoritmos 2** (2026-2), Universidad de Lima.

Dado un CSV de alertas, el programa consolida las alertas por incidente y genera un ranking del incidente más crítico al menos crítico, usando una **tabla hash** y un **max-heap** implementados desde cero (sin STL).

## Integrantes

- Araujo Llontop, Joshua Jorge
- Chienda Cauzo, Mariana Alexandra
- Galopino Lopez, Yustin Hernan

## Problema

Cada fila del CSV es una alerta: `alerta_id,incidente_id,severidad` (severidad de 1 a 10). Varias alertas pueden pertenecer al mismo incidente.

1. Por cada incidente se cuenta cuántas alertas recibió y se suman sus severidades (**prioridad total**).
2. Se ordena de mayor a menor prioridad total.
3. En caso de empate, va primero el `incidente_id` lexicográficamente menor.

Una solución directa (buscar secuencialmente cada incidente y seleccionar el máximo repetidamente) cuesta Θ(n²). Esta solución baja el costo a O(n + u log u), con u ≤ n.

## Diseño

| Etapa | Estructura | Costo |
|---|---|---|
| Consolidar alertas por incidente | Tabla hash | O(n) esperado |
| Ordenar los u incidentes únicos | Max-heap | O(u log u) |
| **Total** | | **O(n + u log u) ⊆ O(n log n)** |

### Tabla hash

- Direccionamiento abierto con **sondeo lineal** (arreglo, sin nodos).
- Función hash polinómica con base **31** sobre todos los caracteres del `incidente_id`.
- Redimensionamiento cuando `ocupados * 10 > tam * 5` (factor de carga > 0.5); la nueva capacidad es `tam * 2 + 1`.
- `buscarOInsertar(id)` devuelve el incidente existente o crea uno nuevo, y el `main` acumula cantidad y prioridad.

### Max-heap

- Arreglo de punteros a incidentes, sin nodos.
- Criterio: mayor `prioridad` primero; si hay empate, menor `id` primero.
- Inserción con *sift-up* y extracción con *sift-down*, ambas O(log u).

### Flujo del programa

1. Lee el CSV a un arreglo dinámico (fuera del tiempo medido).
2. Consolida las n alertas en la tabla hash.
3. Inserta los u incidentes únicos en el max-heap.
4. Extrae el máximo u veces para obtener el ranking.
5. Escribe `ranking.csv` y `metricas.txt`.

## Estructura del repositorio

```
CyberShield/
├── main.cpp          # lectura, flujo principal, salida
├── Incidente.h       # id, cantidad, prioridad
├── TablaHash.h/.cpp  # tabla hash propia
├── MaxHeap.h/.cpp    # max-heap propio
├── datos/            # CSV de entrada de prueba
├── esperados/        # rankings esperados
├── herramientas/     # comparar.py
└── README.md
```

## Compilación

```
g++ -std=c++17 -O2 *.cpp -o cybershield
```

En Code::Blocks basta con abrir el proyecto `CyberShield` y compilar. En Windows el ejecutable es `cybershield.exe`.

## Ejecución

```
./cybershield entrada.csv ranking.csv metricas.txt
```

Ejemplo:

```
./cybershield datos/publico_01_basico.csv ranking.csv metricas.txt
```

### Salidas

`ranking.csv`:

```
posicion,incidente_id,cantidad_alertas,prioridad_total
1,INC-102,2,17
2,INC-101,2,11
...
```

`metricas.txt`:

```
sondeos_hash=...
comparaciones_heap=...
intercambios_heap=...
tiempo_proceso_ms=...
```

## Métricas instrumentadas

Los contadores se incrementan en el momento en que la operación ocurre (no se calculan con fórmulas):

- **sondeos_hash:** cada cubeta examinada al buscar o insertar, incluida la cubeta vacía donde termina la búsqueda. No cuenta los sondeos del rehash.
- **comparaciones_heap:** cada comparación de prioridad (con desempate) dentro de `insertar` y `extraerMax`. _(Indicar línea de código.)_
- **intercambios_heap:** cada intercambio de posiciones en el sift-up y sift-down. _(Indicar línea de código.)_
- **tiempo_proceso_ms:** consolidación + ranking con `chrono`, sin lectura del CSV ni escritura de salidas.

## Pruebas

Verificar un caso contra el esperado:

```
python3 herramientas/comparar.py esperados/publico_01_basico.esperado.csv ranking.csv
```

Debe imprimir `CORRECTO`. Casos incluidos: funcionales pequeños (`publico_01` a `publico_05`), escalabilidad (`escala_distintos_01000` a `16000`, con u = n) y distribución (`mixto_10000`, `repetidos_10000`, `empates_8000`).

### Escalabilidad

Con `C(n) = sondeos_hash + comparaciones_heap` y `R(n) = C(2n) / C(n)`. Una solución O(n log n) da R(n) algo mayor que 2; una cuadrática tiende a 4. Umbral orientativo: R(n) ≤ 3.0 en 4 000 → 8 000 y 8 000 → 16 000.

| n | u | Sondeos hash | Comp. heap | Intercambios | Tiempo (ms) | R(n) |
|---|---|---|---|---|---|---|
| 1 000 | | | | | | — |
| 2 000 | | | | | | |
| 4 000 | | | | | | |
| 8 000 | | | | | | |
| 16 000 | | | | | | |

_Mediciones de tiempo: mediana de 3 ejecuciones. Equipo de prueba: procesador, RAM, compilador y opciones (completar)._

## Restricciones del proyecto

- C++ obligatorio. Sin `map`, `set`, `unordered_map`, `priority_queue`, `vector`, `sort` ni equivalentes de la STL.
- Se usan únicamente `string`, `iostream`, `fstream`, `chrono` y utilidades básicas de E/S.
