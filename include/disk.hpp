#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "node.hpp"

// Contadores de operaciones de entrada/salida.

// Estructura que guarda estadísticas sobre operaciones en disco realizadas por las funciones de lectura/escritura de nodos.
// Campos:
//   lecturas: número de bloques leídos desde disco.
//   escrituras: número de bloques escritos en disco.
// Métodos:
//   reiniciar(): pone ambos contadores a cero.
struct IOStats {
    uint64_t lecturas;
    uint64_t escrituras;

    IOStats() : lecturas(0), escrituras(0) {}

    void reiniciar() {
        lecturas = 0;
        escrituras = 0;
    }
};

// Lee hasta N puntos desde un archivo binario y los devuelve.
// Parámetros:
//   ruta: ruta al archivo binario que contiene pares de floats (x,y).
//   N: máximo número de puntos a leer.
// Retorna:
//   vector con los puntos leídos, lanza excepción si no puede abrir el archivo.
std::vector<Punto> leer_puntos_bin(const std::string& ruta, std::size_t N);

// Escribe una lista de nodos en un archivo binario.
// Parámetros:
//   ruta: ruta del archivo de salida.
//   nodos: vector con los nodos a escribir.
//   io: puntero opcional para acumular estadísticas de escrituras.
// Retorna:
//   Sobrescribe el archivo si ya existe y lanza una excepción en caso de error de E/S.
void escribir_nodos_bin(const std::string& ruta, const std::vector<Nodo>& nodos, IOStats* io = nullptr);

// Clase que abre y permite leer nodos desde un archivo que almacena un R-tree.
// Retorna:
//   Estadísticas acumuladas de lecturas.
class ArchivoRTree {
public:
    explicit ArchivoRTree(const std::string& ruta);

    Nodo leer_nodo(std::size_t indice);
    const IOStats& obtener_io() const;

private:
    std::ifstream archivo;
    IOStats io;
};