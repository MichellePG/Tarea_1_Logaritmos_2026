#pragma once

#include <vector>
#include "node.hpp"

// Construye un R-tree usando el método Nearest-X.
//
// Toma una lista de puntos y construye un R-tree en memoria usando el metodo Nearest-X.
// Parámetros:
//   puntos: vector de "Puntos" que serán insertados en el árbol.
// Retorno:
//   Vector de "Nodos" que representa el R-tree construido.
std::vector<Nodo> construir_nearest_x(const std::vector<Punto>& puntos);