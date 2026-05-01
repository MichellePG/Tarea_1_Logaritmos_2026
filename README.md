# Tarea 1 – Bulk-loading de R-trees

Implementación de R-trees en memoria externa para la Tarea 1 de CC4102 – Diseño y Análisis de Algoritmos.

## Integrantes

- Andrés Franz M.
- Nahuel L. Sanhueza
- Michelle Pérez G.

## Descripción

Este proyecto implementa dos métodos de bulk-loading para construir R-trees sobre puntos 2D:

- Nearest-X
- Sort-Tile-Recursive (STR)

Los puntos se leen desde archivos binarios, donde cada punto está representado por dos valores `float`: coordenada `x` y coordenada `y`.

El árbol se construye inicialmente en memoria principal como un `vector` de nodos. La raíz siempre queda en la posición `0`. Luego, el árbol se serializa a un archivo binario, escribiendo cada nodo de manera secuencial. Las consultas por rango se realizan leyendo nodos desde el archivo binario generado.

Cada nodo del R-tree ocupa exactamente 4096 bytes y puede almacenar hasta 204 entradas.

## Estructura del proyecto

```
Tarea_1_Logaritmos_2026/
│
├── include/                    # Archivos de cabecera (.hpp)
│   ├── node.hpp                # Definición de Punto, MBR, Entrada y Nodo
│   ├── disk.hpp                # Lectura/escritura de puntos y nodos en binario
│   ├── rtree.hpp               # Funciones comunes del R-tree y búsqueda
│   ├── nearest.hpp             # Construcción por Nearest-X
│   └── str.hpp                 # Construcción por STR
│
├── src/                        # Código fuente (.cpp)
│   ├── main.cpp                # CLI principal: build / query / check-data
│   ├── disk.cpp                # Implementación de lectura/escritura
│   ├── rtree.cpp               # Búsqueda, MBRs, utilidades comunes
│   ├── nearest.cpp             # Implementación de Nearest-X
│   └── str.cpp                 # Implementación de STR
│
├── data/                       # Datos locales no versionados
│   ├── random.bin              # No se sube al repositorio
│   ├── europa.bin              # No se sube al repositorio
│   └── samples/
│       ├── random_sample.txt
│       └── europa_sample.txt
│
├── scripts/                    # Scripts de experimentación y gráficos
│   ├── run_build_experiments.sh
│   ├── run_query_experiments.sh
│   └── plot_results.py
│
├── outputs/                    # Archivos generados
│   ├── trees/                  # R-trees serializados
│   ├── results/                # CSVs de resultados
│   └── plots/                  # Gráficos generados
│
├── docs/                       # Informe y material auxiliar
│   └── informe/
│
├── Makefile                    # Compilación y comandos útiles
└── README.md                   # Descripción del proyecto
```

## Datos

Los archivos grandes no se suben al repositorio, deben descargarse desde la sección de Releases del repositorio oficial de la tarea.

data/random.bin
data/europa.bin
data/samples/random_sample.txt
data/samples/europa_sample.txt

## Compilación

Para compilar los programas, ejecute:

```bash
make
```

Esto generará el ejecutable:

```bash
./rtree
```

Para recompilar desde cero:

```bash
make clean && make
```

## Uso

### Verificar lectura de datos:

Antes de construir árboles grandes, se puede verificar que los primeros puntos del archivo binario coincidan con los archivos de sample:

```bash
./rtree check-data data/random.bin 32
./rtree check-data data/europa.bin 32
```

### Construir un árbol:

Formato general:

```bash
./rtree build <nearest|str> <archivo_entrada.bin> <archivo_salida.bin> <N>
```

Ejemplo con N = 32768:

```bash
./rtree build nearest data/random.bin outputs/trees/random_nearest_32768.bin 32768
./rtree build str data/random.bin outputs/trees/random_str_32768.bin 32768
```

Esto construye un R-tree usando los primeros N puntos del archivo de entrada, lo guarda en RAM como un vector de nodos y luego lo serializa a disco en el archivo de salida.

### Consultar un árbol:

Formato general:

```bash
./rtree query <archivo_tree.bin> <xmin> <xmax> <ymin> <ymax>
```

Ejemplo:

```bash
./rtree query outputs/trees/random_nearest_32768.bin 0.1 0.3 0.1 0.3
```

La consulta retorna los puntos contenidos en el rectángulo indicado y reporta la cantidad de lecturas a disco realizadas.

### Experimentación

La experimentación se encuentra en la carpeta scripts/.

### Construcción

Para correr los experimentos de construcción:

```bash
make run-build-experiments
```

Este experimento construye árboles para:

```bash
N = 2^15, 2^16, ..., 2^24
```

Para cada tamaño se construyen cuatro árboles:

random + Nearest-X
random + STR
europa + Nearest-X
europa + STR

Los resultados se guardan en:

outputs/results/build_times.csv

### Consultas

Para correr los experimentos de consultas:

```bash
make run-query-experiments
```

Este experimento usa los árboles construidos con N = 2^24 y realiza consultas con cuadrados aleatorios de lado:

```bash
0.0025, 0.005, 0.01, 0.025, 0.05
```

Para cada tamaño de cuadrado se generan 100 consultas aleatorias.

Los resultados se guardan en:

outputs/results/query_results.csv

### Gráficos

Para generar gráficos a partir de los resultados:

```bash
make plots
```

Los gráficos se guardan en:

outputs/plots/

## Limpieza

Para limpiar los archivos generados:

```bash
make clean
```