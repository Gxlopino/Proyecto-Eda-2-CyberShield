# CyberShield

**Procesamiento y ranking de alertas de ciberseguridad con una tabla hash y un max-heap implementados desde cero en C++.**

Proyecto de Estructuras de Datos y Algoritmos 2 — Universidad de Lima, 2026-2.

[Repositorio en GitHub](https://github.com/Gxlopino/Proyecto-Eda-2-CyberShield)

CyberShield agrupa las alertas por incidente, suma sus severidades y genera un ranking del incidente más crítico al menos crítico. Cada ejecución produce un archivo CSV con el ranking y un archivo de texto con las métricas.

## Integrantes

- Araujo Llontop, Joshua Jorge
- Chienda Cauzo, Mariana Alexandra
- Galopino Lopez, Yustin Hernan

## Guía para ejecutar y evaluar el proyecto

Se requiere un compilador **C++11 o superior con MinGW/GCC**. La opción **C++14 (`-std=c++14`)** de Code::Blocks es suficiente para este proyecto; también se puede utilizar C++17. Python 3 es opcional para ejecutar el programa C++ y se utiliza en las herramientas auxiliares para comprobar rankings y automatizar las mediciones.

### 1. Descargar el repositorio

En GitHub, seleccionar **Code → Download ZIP** y extraer el contenido. También se puede clonar:

```powershell
git clone https://github.com/Gxlopino/Proyecto-Eda-2-CyberShield.git
cd .\Proyecto-Eda-2-CyberShield
```

La raíz del proyecto es la carpeta que contiene `CyberShield.cbp`, `main.cpp`, `datos` y `esperados`. En los ejemplos se utiliza `D:\EDA2\CyberShield`: **reemplazar esta ruta por la ubicación del proyecto en el equipo del docente**.

### 2. Configurar el compilador en Code::Blocks

1. Abrir **File → Open** y seleccionar `CyberShield.cbp` para cargar todos los archivos.
2. Ir a **Settings → Compiler…** y seleccionar **GNU GCC Compiler**.
3. En **Toolchain executables**, revisar **Compiler's installation directory**. En el equipo de desarrollo es `C:\MinGW`. Colocar la ruta real de MinGW del equipo utilizado, **sin agregar `\bin` en este campo**.
4. Confirmar que los ejecutables del compilador correspondan a esa instalación: normalmente `gcc.exe` para C y `g++.exe` para C++ y el enlazador. Algunas distribuciones utilizan nombres con prefijos.
5. En **Project → Build options…**, seleccionar el nombre del proyecto en el panel izquierdo para aplicar la configuración a Debug y Release. Abrir **Compiler settings → Compiler Flags** y marcar **Have g++ follow the C++14 ISO C++ language standard [-std=c++14]**. Dejar una sola opción de estándar seleccionada. Si ya está marcada en **Settings → Compiler…**, no hace falta agregar otra; revisar también las opciones del proyecto para evitar estándares contradictorios.
6. Elegir el destino **Debug** en la barra de compilación y seleccionar **Build → Rebuild**. Comprobar que termine sin errores.

| Destino | Ejecutable generado             | Uso                                        |
| ------- | ------------------------------- | ------------------------------------------ |
| Debug   | `bin\Debug\CyberShield.exe`   | Ejecución y depuración                   |
| Release | `bin\Release\CyberShield.exe` | Mediciones de rendimiento; incluye `-O2` |

Después de cambiar el código, guardar y recompilar el mismo destino cuyo ejecutable se utilizará.

Si la casilla C++14 no aparece en la versión instalada, agregar `-std=c++14` en **Compiler settings → Other compiler options**. Es una forma alternativa de configurar lo mismo: no se necesita agregarla si la casilla ya está marcada. También se puede elegir C++17, siempre que el compilador lo soporte, dejando un solo estándar activo.

### 3. Configurar los argumentos y ejecutar un caso

El programa necesita exactamente **tres argumentos**, en este orden:

```text
CyberShield.exe entrada.csv ranking.csv metricas.txt
```

| Posición   | Significado                           |
| ----------- | ------------------------------------- |
| `argv[0]` | Ejecutable                            |
| `argv[1]` | CSV de entrada                        |
| `argv[2]` | CSV de ranking que se generará       |
| `argv[3]` | Archivo de métricas que se generará |

`argc` vale 4 porque cuenta también el ejecutable. El programa no solicita datos por teclado durante su ejecución.

1. Ir a **Project → Set programs' arguments…**.
2. Seleccionar el destino **Debug** y pegar en **Program arguments** la siguiente línea, ajustando las rutas:

   ```text
   "D:\EDA2\CyberShield\datos\publico_01_basico.csv" "D:\EDA2\CyberShield\ranking.csv" "D:\EDA2\CyberShield\metricas.txt"
   ```
3. Aceptar y presionar **F9 — Build and run**.

Se mostrará un resumen con `n`, `u`, sondeos, comparaciones, intercambios y tiempo. Se generarán `ranking.csv` y `metricas.txt` en la raíz del proyecto. Las rutas absolutas evitan depender del directorio de trabajo configurado en Code::Blocks. Las carpetas de salida deben existir.

### 4. Ejecutar todos los CSV desde PowerShell

Primero compilar **Debug**. Luego abrir **PowerShell** y pegar todo el bloque. Ajustar las rutas de MinGW y del proyecto si están instalados en otro lugar:

```powershell
$env:Path = "C:\MinGW\bin;$env:Path"
Set-Location 'D:\EDA2\CyberShield'

$exe = Join-Path $PWD.Path 'bin\Debug\CyberShield.exe'
if (!(Test-Path -LiteralPath $exe)) {
    throw 'No existe el ejecutable Debug. Compile el proyecto en Code::Blocks.'
}

New-Item -ItemType Directory -Force '.\resultados' | Out-Null
$casos = @(Get-ChildItem '.\datos' -Filter '*.csv' -File | Sort-Object Name)
if ($casos.Count -eq 0) { throw 'No se encontraron CSV en datos.' }

$fallos = 0
foreach ($caso in $casos) {
    Write-Host "`nProcesando: $($caso.Name)"
    $ranking = Join-Path $PWD.Path "resultados\$($caso.BaseName)_ranking.csv"
    $metricas = Join-Path $PWD.Path "resultados\$($caso.BaseName)_metricas.txt"

    & $exe $caso.FullName $ranking $metricas
    if ($LASTEXITCODE -ne 0) {
        $fallos++
        Write-Host "ERROR. Codigo de salida: $LASTEXITCODE" -ForegroundColor Red
    }
}

Write-Host "`nCasos: $($casos.Count). Fallos de ejecucion: $fallos."
Write-Host "Resultados: $(Join-Path $PWD.Path 'resultados')"
```

Con los 13 casos incluidos se generan **26 archivos**, dos por caso. Por ejemplo:

```text
resultados/
├── publico_01_basico_ranking.csv
├── publico_01_basico_metricas.txt
├── publico_02_empates_ranking.csv
├── publico_02_empates_metricas.txt
└── ...
```

La primera línea agrega al `PATH` la carpeta donde se encuentran las DLL de MinGW. Ese cambio dura durante la sesión de PowerShell. Para ejecutar **Release**, compilar ese destino y cambiar la ruta de `$exe` a `bin\Release\CyberShield.exe`.

**No es necesario borrar la carpeta `resultados` para repetir las pruebas.** Los archivos con el mismo nombre se sobrescriben; no se acumulan filas. Los archivos de casos que ya no se ejecuten permanecen. Si una ejecución falla antes de abrir las salidas, podrían quedar resultados anteriores: revisar siempre el código de salida.

Para mostrar todas las métricas en consola:

```powershell
Get-ChildItem '.\resultados' -Filter '*_metricas.txt' | Sort-Object Name | ForEach-Object {
    Write-Host "`n$($_.Name)"
    Get-Content -LiteralPath $_.FullName
}
```

### 5. Validar los rankings

Desde la raíz del proyecto, con Python 3 disponible como `python`:

```powershell
python .\herramientas\comparar.py .\esperados\publico_01_basico.esperado.csv .\resultados\publico_01_basico_ranking.csv
```

Debe mostrar `CORRECTO`. El comparador verifica encabezado, orden y contenido de todas las filas. Para revisar los 13 casos:

```powershell
$fallos = 0
foreach ($caso in Get-ChildItem '.\datos' -Filter '*.csv' -File | Sort-Object Name) {
    Write-Host "`nVerificando: $($caso.Name)"
    python .\herramientas\comparar.py `
        ".\esperados\$($caso.BaseName).esperado.csv" `
        ".\resultados\$($caso.BaseName)_ranking.csv"
    if ($LASTEXITCODE -ne 0) { $fallos++ }
}
Write-Host "`nCasos con errores de verificacion: $fallos"
```

Si el equipo utiliza el lanzador `py -3`, reemplazar `python` por `py -3`. El programa C++ no requiere Python para funcionar.

### 6. Medir tiempos y obtener medianas con Python

La herramienta `herramientas/medir_tiempos.py` automatiza las pruebas de rendimiento. Ejecuta cada CSV de `datos/` **tres veces por defecto**, registra los contadores y los tiempos, compara el ranking con su referencia cuando existe y calcula las **medianas internas y externas por separado**. Con los 13 casos incluidos realiza 39 ejecuciones. Cada ejecución inicia un proceso nuevo, con estructuras y contadores vacíos.

Su finalidad es conservar las muestras y hacer reproducibles las mediciones del informe. La consolidación y el ranking siguen a cargo del programa C++; Python se encarga de ejecutarlo, medir y registrar. El script utiliza únicamente la biblioteca estándar de Python 3 y no compila el ejecutable.

#### Ejecutar la medición

Primero seleccionar **Release** en Code::Blocks y ejecutar **Build → Rebuild**. Después abrir PowerShell en la raíz del proyecto y ejecutar, ajustando la ruta del compilador si fuera necesario:

```powershell
Set-Location 'D:\EDA2\CyberShield'
$env:Path = "C:\MinGW\bin;$env:Path"
python .\herramientas\medir_tiempos.py --exe .\bin\Release\CyberShield.exe
```

La modificación del `PATH` permite localizar las DLL de MinGW y dura solo en esa ventana de PowerShell. Debe utilizarse la carpeta `bin` del compilador que generó el ejecutable.

Para comprobar el funcionamiento con un ejecutable **Debug** ya compilado:

```powershell
python .\herramientas\medir_tiempos.py --exe .\bin\Debug\CyberShield.exe
```

Si el ejecutable se compiló en la raíz, usar `--exe .\CyberShield.exe`. Si `python` no está disponible pero existe el lanzador, reemplazarlo por `py -3`. Las rutas relativas se interpretan desde la carpeta actual de PowerShell. Para comparar tiempos, mantener el mismo destino y las mismas opciones de compilación; identificar por separado las mediciones Debug y Release.

#### Qué mide cada tiempo

| Tiempo  | Cómo se obtiene                                                                                                                                      | Qué incluye                                                                                                                 |
| ------- | ----------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------- |
| Interno | El C++ mide el intervalo con `chrono` y Python lee `tiempo_proceso_ms` de su archivo de métricas.                                                | Consolidación, construcción del heap y extracción del ranking en memoria; excluye lectura del CSV y escritura de salidas. |
| Externo | Python llama a `time.perf_counter()` antes de `subprocess.run()` y después de que el proceso termine; resta los instantes y multiplica por 1000. | Arranque, lectura, procesamiento, escritura, limpieza y el costo de lanzar y esperar el proceso con captura de consola.      |

La compilación, la lectura de métricas desde Python, el guardado de las capturas de consola y la comparación con los esperados quedan **fuera del intervalo externo**. La diferencia entre ambos tiempos no representa exclusivamente lectura y escritura: también contiene otros costos de ejecución.

La **mediana no es el promedio**. Si las tres muestras son `8`, `20` y `9 ms`, se ordenan como `8`, `9`, `20` y se reporta `9 ms`, el valor central. Se calcula una mediana para los tiempos internos y otra para los externos; no se suman ni se mezclan. Un valor interno `0.000 ms` puede deberse a la resolución del reloj o al redondeo del archivo C++.

#### Dónde se guardan los resultados

Cada lanzamiento del script crea una carpeta nueva con fecha y hora. Al finalizar muestra su ruta exacta:

```text
resultados_mediciones/
└── medicion_FECHA_HORA/
    ├── muestras.csv
    ├── resumen_medianas.csv
    ├── publico_01_basico_r1_ranking.csv
    ├── publico_01_basico_r1_metricas.txt
    ├── publico_01_basico_r1_stdout.txt
    ├── publico_01_basico_r1_rstderr.txt
    └── ...
```

| Archivo                                | Para qué sirve                                                                                                                                                  |
| -------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `muestras.csv`                       | Conserva una fila por ejecución: caso, repetición, `n`, `u`, sondeos, comparaciones, intercambios, tiempo interno, tiempo externo y estado de validación. |
| `resumen_medianas.csv`               | Resume cada caso con `n`, `u`, número de repeticiones y medianas interna y externa. Se puede abrir en Excel para preparar tablas y gráficos.               |
| `*_ranking.csv` y `*_metricas.txt` | Conservan las salidas originales de C++ de cada caso y repetición.                                                                                              |
| `*_stdout.txt` y `*_stderr.txt`    | Conservan los mensajes de salida y error para revisar una ejecución o diagnosticar fallos.                                                                      |

Cuando existe `esperados/nombre.esperado.csv`, `medir_tiempos.py` ejecuta el archivo existente `comparar.py`, ubicado en la misma carpeta `herramientas`, para validar el encabezado y todas las filas del ranking. Utiliza el mismo intérprete Python con el que se lanzó la medición. La llamada se realiza después de detener el cronómetro externo, por lo que la validación no afecta los tiempos registrados. `comparar.py` muestra su resultado y, si encuentra diferencias, informa la primera. `CORRECTO` indica coincidencia; `SIN_REFERENCIA` indica que no había archivo esperado y no se verificó la corrección. El script se detiene si el ejecutable o el comparador fallan. En una sesión interrumpida puede haber muestras y resúmenes parciales: comprobar el mensaje final de éxito y que estén todos los casos y repeticiones. Ambos scripts deben conservarse juntos en `herramientas/`.

## Entrada y criterio del ranking

El CSV debe tener encabezado y estas tres columnas, en este orden:

```csv
alerta_id,incidente_id,severidad
ALT-001,INC-101,5
ALT-002,INC-102,9
ALT-003,INC-101,6
```

Se esperan severidades enteras de 1 a 10 e identificadores sin comas. El lector está diseñado para el formato simple de los casos proporcionados; no procesa campos CSV entrecomillados. Descarta la primera línea como encabezado. No conserva `alerta_id` porque no interviene en la consolidación.

Por cada incidente se cuenta la cantidad de alertas y se suman sus severidades (`prioridad_total`). El ranking se ordena por:

1. Mayor prioridad total.
2. En caso de empate, menor identificador en orden lexicográfico.

Para el ejemplo anterior, la salida es:

```csv
posicion,incidente_id,cantidad_alertas,prioridad_total
1,INC-101,2,11
2,INC-102,1,9
```

El archivo de métricas utiliza las claves `sondeos_hash`, `comparaciones_heap`, `intercambios_heap` y `tiempo_proceso_ms`, con una pareja `clave=valor` por línea.

## Diseño y complejidad

### Flujo del programa

1. Leer el CSV en un arreglo dinámico, fuera del tiempo medido.
2. Consolidar las alertas de cada incidente en la tabla hash.
3. Insertar los incidentes únicos en el max-heap.
4. Extraer el máximo repetidamente para construir el ranking.
5. Escribir el ranking y las métricas, fuera del tiempo medido.

Una solución directa con búsqueda secuencial de incidentes y selección repetida del máximo puede costar Θ(n²). La tabla hash y el heap permiten reducir el procesamiento al costo esperado indicado a continuación.

La **tabla hash** usa un arreglo de punteros, direccionamiento abierto y sondeo lineal. Su función hash es polinómica de base 31. Empieza con capacidad 16; antes de buscar o insertar, si `ocupados * 10 > tam * 5`, redimensiona a `tam * 2 + 1` y reubica los incidentes. `buscarOInsertar` devuelve el incidente existente o crea uno nuevo.

El **max-heap** almacena punteros a los incidentes de la tabla. `insertar` reajusta hacia arriba y `SacarHeap` extrae la raíz y reajusta hacia abajo. `tieneMayorPrioridad` compara la prioridad total y resuelve empates por identificador. Los objetos incidente pertenecen a la tabla; el heap y el ranking conservan punteros a ellos.

Con `n` alertas y `u` incidentes únicos:

| Etapa                                       | Costo                                                                                                 |
| ------------------------------------------- | ----------------------------------------------------------------------------------------------------- |
| Consolidación en la tabla hash             | O(n) esperado y amortizado, con identificadores de longitud acotada y distribución adecuada del hash |
| Construcción del heap mediante inserciones | O(u log u)                                                                                            |
| Extracción del ranking completo            | O(u log u)                                                                                            |
| Total del procesamiento                     | O(n + u log u) esperado                                                                               |
| Memoria auxiliar                            | O(n + u)                                                                                              |

El costo esperado del hash no es una garantía de peor caso: muchas colisiones pueden degradar el rendimiento. Las estructuras se implementan sin `vector`, `map`, `set`, `unordered_map`, `priority_queue` ni `sort`. Se utilizan `string`, flujos de entrada y salida, `chrono` e `iomanip`.

## Métricas y mediciones

| Métrica               | Qué registra                                                                                                                                   |
| ---------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| `sondeos_hash`       | Cada cubeta examinada por `buscarOInsertar`, incluida la cubeta vacía donde termina una inserción. Excluye los sondeos del rehash.          |
| `comparaciones_heap` | Cada llamada a `tieneMayorPrioridad`, con el desempate cuando corresponde.                                                                    |
| `intercambios_heap`  | Cada llamada a `intercambiar` durante los reajustes. La asignación del último elemento a la raíz al extraer no se cuenta como intercambio. |
| `tiempo_proceso_ms`  | Consolidación y ranking, incluidas las reservas realizadas dentro del intervalo. Excluye lectura del CSV y escritura de salidas.               |

Los contadores se incrementan durante las operaciones reales. Los tiempos dependen del equipo, compilador, optimización y carga del sistema. `0.000 ms` no significa tiempo nulo: puede deberse a la resolución del reloj o al formato de tres decimales.

Para evaluar rendimiento, usar **Release**, ejecutar cada caso al menos tres veces y reportar la **mediana**, junto con procesador, RAM, versión del compilador y opciones utilizadas.

### Resultados locales de referencia

En la verificación local del 19 de noviembre de 2026, los **13 rankings coincidieron exactamente con los esperados**. Al repetir las ejecuciones se reprodujeron los contadores de sondeos, comparaciones e intercambios.

Los tiempos de esta tabla corresponden a **una ejecución local Debug**, no a medianas ni a una garantía de rendimiento en otros equipos:

| Alertas distintas | Incidentes | Sondeos | Comparaciones | Intercambios | Tiempo (ms) |
| ----------------: | ---------: | ------: | ------------: | -----------: | ----------: |
|             1 000 |      1 000 |   2 805 |        17 115 |        8 495 |       1.016 |
|             2 000 |      2 000 |   6 061 |        38 391 |       19 114 |       2.503 |
|             4 000 |      4 000 |  13 240 |        84 817 |       42 198 |       4.022 |
|             8 000 |      8 000 |  25 065 |       185 472 |       92 351 |      10.885 |
|            16 000 |     16 000 |  50 773 |       402 886 |      200 826 |      15.583 |

Con `C(n) = sondeos_hash + comparaciones_heap`, se obtiene:

- `C(8000) / C(4000) ≈ 2.147`.
- `C(16000) / C(8000) ≈ 2.155`.

Ambos cumplen el umbral orientativo de 3.0 utilizado en el proyecto y son compatibles con el crecimiento esperado. Las mediciones complementan el análisis de complejidad.

## Solución de problemas

| Situación                                                | Solución                                                                                                                                                                                         |
| --------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Aparece `Uso: ... entrada.csv ranking.csv metricas.txt` | Configurar los tres argumentos en **Project → Set programs' arguments…** para el destino activo.                                                                                          |
| Code::Blocks no encuentra el compilador                   | Revisar **Settings → Compiler → Toolchain executables** y la instalación de MinGW.                                                                                                       |
| Falta una DLL o aparece `-1073741515` / `0xC0000135`  | Agregar al `PATH` la carpeta `bin` del mismo MinGW usado para compilar. Si persiste, revisar que su instalación y bibliotecas estén completas.                                              |
| Python informa `Fallo ... (codigo 3221225781)`          | Es la representación sin signo de `0xC0000135`: Windows no pudo cargar una DLL necesaria. Agregar la carpeta `bin` del compilador real al `PATH` en esa misma ventana y volver a ejecutar. |
| `medir_tiempos.py` informa `No existe el ejecutable`  | Compilar el destino indicado en `--exe`. Si solo existe Debug, usar su ruta; para medir Release, seleccionarlo y recompilar primero.                                                            |
| PowerShell no reconoce `python`                         | Probar `py -3`. Si tampoco está disponible, instalar Python 3 y volver a abrir PowerShell.                                                                                                     |
| Solo aparece `Procesando`, sin resultados               | Consultar el código de salida. Ese mensaje lo imprime PowerShell antes de ejecutar el programa y no confirma que haya funcionado.                                                                |
| `No se pudo abrir`                                      | Comprobar que el CSV exista y que su ruta sea correcta. Colocar comillas si contiene espacios.                                                                                                    |
| `No se pudo escribir`                                   | Comprobar que la carpeta de salida exista y tenga permisos de escritura.                                                                                                                          |
| No existe el ejecutable Debug                             | Compilar Debug o, si se compiló Release, usar `bin\Release\CyberShield.exe`.                                                                                                                   |
| Los cambios no aparecen en las salidas                    | Guardar y recompilar el destino cuyo ejecutable se está invocando.                                                                                                                               |
| La consola vuelve a `PS ...>`                           | La ejecución terminó. Revisar el resumen y los errores; no es necesario seguir esperando.                                                                                                       |

Utilizar rutas diferentes para entrada, ranking y métricas. Para volver a generar resultados, ejecutar otra vez después de recompilar los cambios.

## Estructura del proyecto

```text
CyberShield/
├── CyberShield.cbp       # Proyecto de Code::Blocks
├── main.cpp             # Lectura, procesamiento y salidas
├── Incidente.h          # Datos de cada incidente
├── TablaHash.h
├── TablaHash.cpp         # Hash, sondeo lineal y redimensionamiento
├── MaxHeap.h
├── MaxHeap.cpp           # Inserción, extracción y contadores
├── datos/               # 13 casos CSV
├── esperados/           # Rankings de referencia
├── herramientas/
│   ├── comparar.py      # Comparador de rankings con Python 3
│   └── medir_tiempos.py # Repeticiones, tiempo externo y medianas
├── resultados/          # Archivos generados por las pruebas
├── resultados_mediciones/ # Sesiones de medición generadas por Python
└── README.md
```

Los casos cubren cinco pruebas funcionales (`publico_01` a `publico_05`), cinco tamaños de escalabilidad y tres distribuciones (`mixto_10000`, `repetidos_10000`, `empates_8000`). Las carpetas `bin` y `obj` se generan al compilar. El bloque de PowerShell crea `resultados` si aún no existe.
