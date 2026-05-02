#include "nearest.hpp"
#include "rtree.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Compara dos entradas según el centro X de sus MBR.
 * Si tienen el mismo centro X, compara por centro Y para dejar un orden estable.
 */
static bool comparar_por_centro_x(const Entrada& a, const Entrada& b)
{
    float ax = centro_x(a.mbr);
    float bx = centro_x(b.mbr);

    if (ax == bx) {
        return centro_y(a.mbr) < centro_y(b.mbr);
    }

    return ax < bx;
}

/**
 * Construye un R-tree usando el método Nearest-X.
 *
 * El árbol se construye en un vector de nodos. La posición 0 queda reservada
 * para la raíz. Los nodos creados en los niveles inferiores se guardan desde
 * la posición 1 en adelante, y sus padres los referencian usando esos índices.
 */
std::vector<Nodo> construir_nearest_x(const std::vector<Punto>& puntos)
{
    std::vector<Nodo> arbol;

    if (puntos.empty()) {
        return arbol;
    }

    arbol.push_back(crear_nodo());

    std::vector<Entrada> nivel_actual;
    nivel_actual.reserve(puntos.size());

    for (const Punto& punto : puntos) {
        nivel_actual.push_back(crear_entrada_punto(punto));
    }

    while (nivel_actual.size() > B) {
        std::sort(nivel_actual.begin(), nivel_actual.end(), comparar_por_centro_x);

        std::vector<Entrada> nivel_siguiente;
        std::size_t cantidad_nodos = (nivel_actual.size() + B - 1) / B;
        nivel_siguiente.reserve(cantidad_nodos);

        for (std::size_t i = 0; i < nivel_actual.size(); i += B) {
            Nodo nodo = crear_nodo();

            std::size_t fin = std::min(i + static_cast<std::size_t>(B),
                                       nivel_actual.size());

            for (std::size_t j = i; j < fin; j++) {
                agregar_entrada(nodo, nivel_actual[j]);
            }

            int32_t indice_nodo = static_cast<int32_t>(arbol.size());
            arbol.push_back(nodo);

            MBR mbr_nodo = calcular_mbr_nodo(nodo);
            Entrada entrada_padre = crear_entrada_interna(mbr_nodo, indice_nodo);
            nivel_siguiente.push_back(entrada_padre);
        }

        nivel_actual = std::move(nivel_siguiente);
    }

    Nodo raiz = crear_nodo();

    for (const Entrada& entrada : nivel_actual) {
        agregar_entrada(raiz, entrada);
    }

    arbol[0] = raiz;

    return arbol;
}