#include "nearest.hpp"
#include "rtree.hpp"

    // Helpers disponibles (borrar comentarios antes de entregar):
    // - crear_nodo()
    // - crear_entrada_punto(punto)
    // - crear_entrada_interna(mbr, indice_hijo)
    // - agregar_entrada(nodo, entrada)
    // - centro_x(mbr)
    // - calcular_mbr_nodo(nodo)

std::vector<Nodo> construir_nearest_x(const std::vector<Punto>& puntos) {
    // TODO: Implementar método Nearest-X.
    // 1. Convertir puntos a entradas con MBR de punto e hijo = -1.
    // 2. Ordenar por centro X.
    // 3. Agrupar de a B entradas.
    // 4. Crear nodos y generar entradas para el nivel superior.
    // 5. Repetir hasta crear la raíz en la posición 0.

    return {};
}