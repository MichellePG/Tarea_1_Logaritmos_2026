#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "node.hpp"

// Contadores de operaciones de entrada/salida.
struct IOStats {
    uint64_t lecturas;
    uint64_t escrituras;

    IOStats() : lecturas(0), escrituras(0) {}

    void reiniciar() {
        lecturas = 0;
        escrituras = 0;
    }
};

// Lee N puntos desde un archivo binario.
std::vector<Punto> leer_puntos_bin(const std::string& ruta, std::size_t N);

// Escribe todos los nodos del árbol en un archivo binario.
void escribir_nodos_bin(const std::string& ruta, const std::vector<Nodo>& nodos, IOStats* io = nullptr);

// Maneja la lectura de nodos desde un archivo de R-tree.
class ArchivoRTree {
public:
    explicit ArchivoRTree(const std::string& ruta);

    Nodo leer_nodo(std::size_t indice);
    const IOStats& obtener_io() const;

private:
    std::ifstream archivo;
    IOStats io;
};