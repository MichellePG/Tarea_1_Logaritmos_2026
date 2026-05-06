#include "str.hpp"
#include "rtree.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

/**
 * Compara dos entradas según el centro X de sus MBR.
 * Si tienen el mismo centro X, compara por centro Y para un orden determinista.
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
 * Compara dos entradas según el centro Y de sus MBR.
 * Si tienen el mismo centro Y, compara por centro X para un orden determinista.
 */
static bool comparar_por_centro_y(const Entrada& a, const Entrada& b)
{
    float ay = centro_y(a.mbr);
    float by = centro_y(b.mbr);

    if (ay == by) {
        return centro_x(a.mbr) < centro_x(b.mbr);
    }

    return ay < by;
}

/**
 * Construye un R-tree usando el método Sort-Tile-Recursive (STR).
 *
 * El algoritmo divide recursivamente los datos en una grilla 2D:
 * 1. Ordena por coordenada X y divide en hasta S = ceil(sqrt(n/B)) franjas verticales.
 * 2. Dentro de cada franja, ordena por Y y divide en hasta S grupos.
 * 3. Cada grupo final forma un nodo con hasta B entradas.
 * 4. Se repite recursivamente hasta que todas las entradas quepan en la raíz.
 *
 * La raíz siempre queda en la posición 0 del vector de nodos.
 */
std::vector<Nodo> construir_str(const std::vector<Punto>& puntos)
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
        std::size_t n = nivel_actual.size();

        // S = ceil(sqrt(n / B)) es el número de franjas en cada dimensión.
        std::size_t S = static_cast<std::size_t>(
            std::ceil(std::sqrt(static_cast<double>(n) / static_cast<double>(B)))
        );

        // Tamaño de cada franja vertical (cantidad de entradas por franja).
        std::size_t tamano_franja = S * B;

        // 1.Ordenar todas las entradas por centro X.
        std::sort(nivel_actual.begin(), nivel_actual.end(), comparar_por_centro_x);

        std::vector<Entrada> nivel_siguiente;
        std::size_t cantidad_nodos_estimada = (n + B - 1) / B;
        nivel_siguiente.reserve(cantidad_nodos_estimada);

        // 2.Dividir en S franjas verticales.
        for (std::size_t franja = 0; franja < S; franja++) {
            std::size_t inicio_franja = franja * tamano_franja;

            if (inicio_franja >= n) {
                break;
            }

            std::size_t fin_franja = std::min(inicio_franja + tamano_franja, n);

            // 3.Ordenar la franja por centro Y.
            std::sort(
                nivel_actual.begin() + static_cast<std::ptrdiff_t>(inicio_franja),
                nivel_actual.begin() + static_cast<std::ptrdiff_t>(fin_franja),
                comparar_por_centro_y
            );

            // 4.Dividir la franja en grupos de B entradas (tiles).
            for (std::size_t i = inicio_franja; i < fin_franja; i += B) {
                Nodo nodo = crear_nodo();

                std::size_t fin_grupo = std::min(i + static_cast<std::size_t>(B), fin_franja);

                for (std::size_t j = i; j < fin_grupo; j++) {
                    agregar_entrada(nodo, nivel_actual[j]);
                }

                int32_t indice_nodo = static_cast<int32_t>(arbol.size());
                arbol.push_back(nodo);

                MBR mbr_nodo = calcular_mbr_nodo(nodo);
                Entrada entrada_padre = crear_entrada_interna(mbr_nodo, indice_nodo);
                nivel_siguiente.push_back(entrada_padre);
            }
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