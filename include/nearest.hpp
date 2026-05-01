#pragma once

#include <vector>
#include "node.hpp"

// Construye un R-tree usando el método Nearest-X.
// Debe retornar un vector de nodos donde:
// - arbol[0] es la raíz.
// - los hijos se referencian por índice dentro del vector.

std::vector<Nodo> construir_nearest_x(const std::vector<Punto>& puntos);