#pragma once

#include <vector>
#include "node.hpp"

// Crea un MBR vacío para calcular envolventes.
MBR crear_mbr_vacio();

// Retorna true si el MBR todavía no contiene nada.
bool mbr_esta_vacio(const MBR& mbr);

// Expande destino para que también contenga nuevo.
void expandir_mbr(MBR& destino, const MBR& nuevo);

// Calcula el MBR que contiene todas las entradas usadas de un nodo.
MBR calcular_mbr_nodo(const Nodo& nodo);

// Centro del MBR en X e Y.
float centro_x(const MBR& mbr);
float centro_y(const MBR& mbr);

// Indica si dos rectángulos se intersectan.
bool intersectan(const MBR& a, const MBR& b);

// Indica si una entrada hoja, vista como punto, cae dentro de la consulta.
bool mbr_punto_dentro(const MBR& punto, const MBR& consulta);