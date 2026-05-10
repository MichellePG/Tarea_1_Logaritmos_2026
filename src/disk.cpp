#include "disk.hpp"

#include <stdexcept>

// Lee hasta N puntos desde un archivo binario y los devuelve.
// Parámetros:
//   ruta: ruta al archivo binario que contiene pares de floats (x,y).
//   N: máximo número de puntos a leer.
// Retorna:
//   vector con los puntos leídos, lanza excepción si no puede abrir el archivo.
std::vector<Punto> leer_puntos_bin(const std::string& ruta, std::size_t N) {
    std::vector<Punto> puntos;
    puntos.reserve(N);

    std::ifstream in(ruta.c_str(), std::ios::binary);
    if (!in) throw std::runtime_error("No se pudo abrir el archivo de puntos: " + ruta);

    // Itera y lee pares de floats, si la lectura falla se detiene.
    for (std::size_t i = 0; i < N; i++) {
        Punto punto;
        in.read(reinterpret_cast<char*>(&punto.x), sizeof(float));
        in.read(reinterpret_cast<char*>(&punto.y), sizeof(float));

        if (!in) break;
        puntos.push_back(punto);
    }

    return puntos;
}

// Escribe una lista de nodos en un archivo binario.
// Parámetros:
//   ruta: ruta del archivo de salida.
//   nodos: vector con los nodos a escribir.
//   io: puntero opcional para acumular estadísticas de escrituras.
// Retorna:
//   Sobrescribe el archivo si ya existe y lanza una excepción en caso de error de E/S.
void escribir_nodos_bin(const std::string& ruta, const std::vector<Nodo>& nodos, IOStats* io) {
    std::ofstream out(ruta.c_str(), std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("No se pudo abrir el archivo para escritura: " + ruta);

    for (std::size_t i = 0; i < nodos.size(); i++) {
        out.write(reinterpret_cast<const char*>(&nodos[i]), sizeof(Nodo));
        if (!out) throw std::runtime_error("Error al escribir un nodo en: " + ruta);

        if (io != nullptr) io->escrituras++;
    }
}

// Constructor que abre el archivo binario que contiene los nodos del R-tree.
// Parámetros:
//   ruta: ruta del archivo que se va a abrir en modo lectura binaria.
// Retorna:
//   Lanza una excepción si el archivo no puede abrirse.
ArchivoRTree::ArchivoRTree(const std::string& ruta) : archivo(ruta.c_str(), std::ios::binary), io() {
    if (!archivo) throw std::runtime_error("No se pudo abrir el archivo de R-tree: " + ruta);
}

// Lee y devuelve el nodo situado en la posición "indice" dentro del archivo.
// Parámetros:
//   indice: índice del nodo en el archivo.
// Retorna:
//   Nodo leído desde el archivo. Lanza excepción en caso de error de E/S.
Nodo ArchivoRTree::leer_nodo(std::size_t indice) {
    Nodo nodo;

    std::streamoff posicion = static_cast<std::streamoff>(indice) * static_cast<std::streamoff>(sizeof(Nodo));

    archivo.clear();
    archivo.seekg(posicion, std::ios::beg);
    if (!archivo) throw std::runtime_error("No se pudo mover al nodo " + std::to_string(indice));

    archivo.read(reinterpret_cast<char*>(&nodo), sizeof(Nodo));
    if (!archivo) throw std::runtime_error("Lectura incompleta del nodo " + std::to_string(indice));

    io.lecturas++;
    return nodo;
}

// Devuelve las estadísticas de E/S acumuladas.
const IOStats& ArchivoRTree::obtener_io() const {
    return io;
}