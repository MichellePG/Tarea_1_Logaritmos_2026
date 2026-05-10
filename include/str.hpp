#pragma once

#include <vector>
#include "node.hpp"

// Construye un R-tree usando el método Sort-Tile-Recursive (STR).
//
// Organiza las entradas en tiles y crea nodos que se agrupan recursivamente hasta formar la raíz.
// Parámetros:
//   puntos: vector de "Punto" a partir de los cuales se construye el árbol.
// Retorno:
//   Vector de "Nodo" que representa el R-tree construido. Si "puntos" está
//   vacío, se retorna un vector vacío.
std::vector<Nodo> construir_str(const std::vector<Punto>& puntos);