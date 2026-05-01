#pragma once

#include <cstdint>
#include <stdexcept>

static const int B = 204; // Capacidad máxima de entradas por nodo.

static const int TAMANO_NODO = 4096; // Tamaño exacto de un bloque en disco.

// Punto 2D leído desde los archivos random.bin / europa.bin.
struct Punto {
    float x;
    float y;
};

// Minimum Bounding Rectangle.
struct MBR {
    float x1; // x1 = mínimo en X
    float x2; // x2 = máximo en X
    float y1; // y1 = mínimo en Y
    float y2; // y2 = máximo en Y
};

// Entrada par clave-valor.
#pragma pack(push, 1)
struct Entrada {
    MBR mbr; // clave
    int32_t hijo; // valor: índice del nodo hijo en el archivo/vector si es interno,-1 si es hoja.
};

// Nodo de R-tree.
struct Nodo {
    int32_t k; // Guarda la cantidad de hijos actualmente contenidos en el nodo, 4 bytes.
    Entrada entradas[B];
    char pad[12]; // pad: 12 bytes libres para completar los 4096 bytes del nodo.
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

// Crea una entrada hoja a partir de un punto.
inline Entrada crear_entrada_punto(const Punto& punto) {
    return Entrada{crear_mbr_punto(punto), -1};
}

// Crea una entrada interna a partir de un MBR y el índice del hijo.
inline Entrada crear_entrada_interna(const MBR& mbr, int32_t hijo) {
    return Entrada{mbr, hijo};
}

// Crea un nodo vacío.
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

// Agrega una entrada al final del nodo.
inline void agregar_entrada(Nodo& nodo, const Entrada& entrada) {
    if (nodo.k >= B) {
        throw std::runtime_error("No se puede agregar entrada: nodo lleno");
    }

    nodo.entradas[nodo.k] = entrada;
    nodo.k++;
}

// Un nodo hoja tiene entradas con hijo == -1.
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