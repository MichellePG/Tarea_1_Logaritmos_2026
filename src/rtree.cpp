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

std::vector<Punto> buscar_rango(const std::string& ruta_arbol, const MBR& consulta, IOStats* out_io) {
    ArchivoRTree archivo(ruta_arbol);
    std::vector<Punto> resultado;

    buscar_rango_rec(archivo, 0, consulta, resultado);

    if (out_io != nullptr) {
        *out_io = archivo.obtener_io();
    }

    return resultado;
}

void buscar_rango_rec(ArchivoRTree& archivo, std::size_t indice_nodo, const MBR& consulta, std::vector<Punto>& resultado) {
    Nodo nodo = archivo.leer_nodo(indice_nodo);

    if (es_hoja(nodo)) {
        for (int i = 0; i < nodo.k; i++) {
            const Entrada& entrada = nodo.entradas[i];

            if (mbr_punto_dentro(entrada.mbr, consulta)) {
                resultado.push_back(punto_desde_entrada(entrada));
            }
        }

        return;
    }

    for (int i = 0; i < nodo.k; i++) {
        const Entrada& entrada = nodo.entradas[i];

        if (intersectan(entrada.mbr, consulta)) {
            buscar_rango_rec(archivo, static_cast<std::size_t>(entrada.hijo), consulta, resultado);
        }
    }
}