#pragma once

#include <cstdint>
#include <stdexcept>

static const int B = 204; // Capacidad máxima de entradas por nodo.

static const int TAMANO_NODO = 4096; // Tamaño exacto de un bloque en disco.

// Representa un punto 2D en coordenadas flotantes.
// Campos:
//   x: coordenada X.
//   y: coordenada Y.
struct Punto {
    float x;
    float y;
};

// Minimum Bounding Rectangle (MBR)
// Campos:
//   x1: mínimo en X.
//   x2: máximo en X.
//   y1: mínimo en Y.
//   y2: máximo en Y.
struct MBR {
    float x1;
    float x2; 
    float y1;
    float y2; 
};

// Estructura de entrada que asocia un MBR con un puntero/índice a un hijo.
#pragma pack(push, 1)
struct Entrada {
    MBR mbr; // clave
    int32_t hijo; // valor: índice del nodo hijo en el archivo/vector si es interno,-1 si es hoja.
};

// Estructura de nodo de R-tree para almacenamiento en disco.
// Campos:
//   k: número de entradas válidas en el nodo.
//   entradas: arreglo fijo con capacidad máxima "B".
//   pad: relleno para completar el tamaño exacto del bloque en disco.
struct Nodo {
    int32_t k; 
    Entrada entradas[B];
    char pad[12];
};

#pragma pack(pop)

// ----------------------------------------------------------------
// Verificaciones de tamaño en tiempo de compilación.
// ----------------------------------------------------------------

static_assert(sizeof(Punto) == 8, "Punto debe medir 8 bytes");
static_assert(sizeof(MBR) == 16, "MBR debe medir 16 bytes");
static_assert(sizeof(Entrada) == 20, "Entrada debe medir 20 bytes");
static_assert(sizeof(Nodo) == TAMANO_NODO, "Nodo debe medir exactamente 4096 bytes");

// -----------------------------
// ---------HELPERS-------------
// -----------------------------

// Crea un MBR que representa un punto.
inline MBR crear_mbr_punto(float x, float y) {
    return MBR{x, x, y, y}; // En una hoja, x1 = x2 e y1 = y2.
}

inline MBR crear_mbr_punto(const Punto& punto) {
    return crear_mbr_punto(punto.x, punto.y);
}

// Crea una entrada de hoja a partir de un "Punto".
// El campo "hijo" se marca como -1 para indicar que es una hoja.
inline Entrada crear_entrada_punto(const Punto& punto) {
    return Entrada{crear_mbr_punto(punto), -1};
}

// Crea una entrada interna que asocia un "MBR" con el índice del nodo hijo.
inline Entrada crear_entrada_interna(const MBR& mbr, int32_t hijo) {
    return Entrada{mbr, hijo};
}

// Crea un nodo vacío con todos los campos inicializados a valores por defecto.
// El contador "k" se inicializa en 0 y las entradas se marcan como vacías.
inline Nodo crear_nodo() {
    Nodo nodo;
    nodo.k = 0;

    for (int i = 0; i < B; i++) {
        nodo.entradas[i].mbr = MBR{0.0f, 0.0f, 0.0f, 0.0f};
        nodo.entradas[i].hijo = -1;
    }

    for (int i = 0; i < 12; i++) {
        nodo.pad[i] = 0;
    }

    return nodo;
}

// Agrega una entrada al final del nodo. Lanza excepción si el nodo está lleno.
inline void agregar_entrada(Nodo& nodo, const Entrada& entrada) {
    if (nodo.k >= B) {
        throw std::runtime_error("No se puede agregar entrada: nodo lleno");
    }

    nodo.entradas[nodo.k] = entrada;
    nodo.k++;
}

// Indica si el nodo es una hoja (las entradas tienen hijo = -1) o está vacío (k=0).
inline bool es_hoja(const Nodo& nodo) {
    if (nodo.k == 0) {
        return true;
    }

    return nodo.entradas[0].hijo == -1;
}

// Convierte una entrada hoja a punto.
inline Punto punto_desde_entrada(const Entrada& entrada) {
    return Punto{entrada.mbr.x1, entrada.mbr.y1};
}