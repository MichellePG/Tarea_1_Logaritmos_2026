#include "rtree.hpp"

#include <algorithm>
#include <limits>

MBR crear_mbr_vacio() {
    float infinito = std::numeric_limits<float>::infinity();
    return MBR{infinito, -infinito, infinito, -infinito};
}

bool mbr_esta_vacio(const MBR& mbr) {
    return mbr.x1 > mbr.x2 || mbr.y1 > mbr.y2;
}

void expandir_mbr(MBR& destino, const MBR& nuevo) {
    if (mbr_esta_vacio(destino)) {
        destino = nuevo;
        return;
    }

    destino.x1 = std::min(destino.x1, nuevo.x1);
    destino.x2 = std::max(destino.x2, nuevo.x2);
    destino.y1 = std::min(destino.y1, nuevo.y1);
    destino.y2 = std::max(destino.y2, nuevo.y2);
}

MBR calcular_mbr_nodo(const Nodo& nodo) {
    MBR resultado = crear_mbr_vacio();

    for (int i = 0; i < nodo.k; i++) {
        expandir_mbr(resultado, nodo.entradas[i].mbr);
    }

    return resultado;
}

float centro_x(const MBR& mbr) {
    return (mbr.x1 + mbr.x2) / 2.0f;
}

float centro_y(const MBR& mbr) {
    return (mbr.y1 + mbr.y2) / 2.0f;
}

bool intersectan(const MBR& a, const MBR& b) {
    return a.x1 <= b.x2 &&
           a.x2 >= b.x1 &&
           a.y1 <= b.y2 &&
           a.y2 >= b.y1;
}

bool mbr_punto_dentro(const MBR& punto, const MBR& consulta) {
    return punto.x1 >= consulta.x1 &&
           punto.x1 <= consulta.x2 &&
           punto.y1 >= consulta.y1 &&
           punto.y1 <= consulta.y2;
}