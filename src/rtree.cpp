#include "rtree.hpp"

#include <algorithm>
#include <limits>

// Crea un MBR inicial vacío.
// Retorna:
//   MBR con límites iniciales que indican "vacío".
MBR crear_mbr_vacio() {
    float infinito = std::numeric_limits<float>::infinity();
    return MBR{infinito, -infinito, infinito, -infinito};
}

// Devuelve true si el MBR no contiene ninguna geometría válida.
// Parámetros:
//   mbr: MBR a comprobar.
// Retorna:
//   true si el MBR está vacío, false en caso contrario.
bool mbr_esta_vacio(const MBR& mbr) {
    return mbr.x1 > mbr.x2 || mbr.y1 > mbr.y2;
}

// Expande "destino" para que también contenga "nuevo".
// Parámetros:
//   destino: MBR que será modificado para incluir "nuevo".
//   nuevo: MBR que se debe incluir dentro de "destino".
// Retorna:
//   "destino" se modifica. Si "destino" estaba vacío se asignan los valores de "nuevo".
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

// Calcula el MBR que contiene todas las entradas válidas de "nodo".
// Parámetros:
//   nodo: nodo cuyos MBR de entradas se usarán para calcular la envolvente.
// Retorna:
//   MBR mínimo que contiene todas las entradas válidas del nodo. Si el nodo no tiene entradas se devuelve un MBR vacío.
MBR calcular_mbr_nodo(const Nodo& nodo) {
    MBR resultado = crear_mbr_vacio();

    for (int i = 0; i < nodo.k; i++) {
        expandir_mbr(resultado, nodo.entradas[i].mbr);
    }

    return resultado;
}

// Devuelve la coordenada X del centro del "mbr".
// Parámetros:
//   mbr: rectángulo del cual se quiere el centro en X.
// Retorna:
//   Coordenada X del centro.
float centro_x(const MBR& mbr) {
    return (mbr.x1 + mbr.x2) / 2.0f;
}

// Devuelve la coordenada Y del centro del "mbr".
// Parámetros:
//   mbr: rectángulo del cual se quiere el centro en Y.
// Retorna:
//   Coordenada Y del centro.
float centro_y(const MBR& mbr) {
    return (mbr.y1 + mbr.y2) / 2.0f;
}

// Devuelve true si los dos rectángulos se solapan.
// Parámetros:
//   a, b: rectángulos a comprobar.
// Retorna:
//   true si "a" y "b" tienen intersección no vacía, false en caso contrario.
bool intersectan(const MBR& a, const MBR& b) {
    return a.x1 <= b.x2 &&
           a.x2 >= b.x1 &&
           a.y1 <= b.y2 &&
           a.y2 >= b.y1;
}

// Comprueba si una entrada hoja está totalmente dentro de la región "consulta".
// Parámetros:
//   punto: MBR que representa un punto (x1==x2, y1==y2).
//   consulta: MBR que define la región de búsqueda.
// Retorno:
//   true si el punto está dentro de "consulta", false en caso contrario.
bool mbr_punto_dentro(const MBR& punto, const MBR& consulta) {
    return punto.x1 >= consulta.x1 &&
           punto.x1 <= consulta.x2 &&
           punto.y1 >= consulta.y1 &&
           punto.y1 <= consulta.y2;
}

// Abre un archivo que contiene un árbol y devuelve todos los puntos que están dentro de "consulta".
// Parámetros:
//   ruta_arbol: ruta al archivo binario que contiene los nodos del R-tree.
//   consulta: MBR que define la región de búsqueda.
//   out_io: puntero opcional para recibir estadísticas de lecturas.
// Retorna:
//   vector de "Puntos" encontrados en la región.
std::vector<Punto> buscar_rango(const std::string& ruta_arbol, const MBR& consulta, IOStats* out_io) {
    ArchivoRTree archivo(ruta_arbol);
    std::vector<Punto> resultado;

    buscar_rango_rec(archivo, 0, consulta, resultado);

    if (out_io != nullptr) {
        *out_io = archivo.obtener_io();
    }

    return resultado;
}

// Función auxiliar que opera sobre un "ArchivoRTree" ya abierto.
// Parámetros:
//   archivo: instancia abierta que permite leer nodos por índice.
//   indice_nodo: índice del nodo desde el cual iniciar la búsqueda.
//   consulta: región de búsqueda.
//   resultado: vector donde se irán acumulando los puntos encontrados.
// Retorna:
//   "resultado" se modifica agregando los puntos encontrados en el subárbol.
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