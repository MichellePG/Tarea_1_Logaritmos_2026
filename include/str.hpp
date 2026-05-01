#pragma once

#include <vector>
#include "node.hpp"

// Construye un R-tree usando el método Sort-Tile-Recursive.
// Debe retornar un vector de nodos donde:
// - arbol[0] es la raíz.
// - los hijos se referencian por índice dentro del vector.

std::vector<Nodo> construir_str(const std::vector<Punto>& puntos);