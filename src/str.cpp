#include "str.hpp"
#include "rtree.hpp"

    // Helpers disponibles (borrar comentarios antes de entregar):
    // - crear_nodo()
    // - crear_entrada_punto(punto)
    // - crear_entrada_interna(mbr, indice_hijo)
    // - agregar_entrada(nodo, entrada)
    // - centro_x(mbr)
    // - centro_y(mbr)
    // - calcular_mbr_nodo(nodo)

std::vector<Nodo> construir_str(const std::vector<Punto>& puntos) {
    // TODO: Implementar método STR.
    // 1. Convertir puntos a entradas con MBR de punto e hijo = -1.
    // 2. Ordenar por centro X.
    // 3. Dividir en franjas.
    // 4. Ordenar cada franja por centro Y.
    // 5. Agrupar de a B entradas.
    // 6. Repetir hasta crear la raíz en la posición 0.

    (void)puntos;
    return {};
}