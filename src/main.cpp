#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "disk.hpp"
#include "nearest.hpp"
#include "rtree.hpp"
#include "str.hpp"

struct Crono {
    std::chrono::high_resolution_clock::time_point inicio;

    void iniciar() {
        inicio = std::chrono::high_resolution_clock::now();
    }

    double ms() const {
        auto fin = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(fin - inicio).count();
    }
};

static void imprimir_uso() {
    std::cerr << "Uso:\n";
    std::cerr << "  ./rtree check-data <archivo_puntos.bin> <N>\n";
    std::cerr << "  ./rtree build <nearest|str> <archivo_puntos.bin> <archivo_salida_tree.bin> <N>\n";
    std::cerr << "  ./rtree query <archivo_tree.bin> <xmin> <xmax> <ymin> <ymax>\n";
}

static int comando_check_data(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Uso: ./rtree check-data <archivo_puntos.bin> <N>\n";
        return 1;
    }

    std::string ruta = argv[2];
    std::size_t N = std::stoull(argv[3]);

    std::vector<Punto> puntos = leer_puntos_bin(ruta, N);

    for (std::size_t i = 0; i < puntos.size(); i++) {
        std::cout << puntos[i].x << " " << puntos[i].y << "\n";
    }

    std::cerr << "Puntos leidos: " << puntos.size() << "\n";
    return 0;
}

static int comando_build(int argc, char** argv) {
    if (argc != 6) {
        std::cerr << "Uso: ./rtree build <nearest|str> <archivo_puntos.bin> <archivo_salida_tree.bin> <N>\n";
        return 1;
    }

    std::string metodo = argv[2];
    std::string ruta_datos = argv[3];
    std::string ruta_salida = argv[4];
    std::size_t N = std::stoull(argv[5]);

    std::vector<Punto> puntos = leer_puntos_bin(ruta_datos, N);

    if (puntos.empty()) {
        std::cerr << "No se leyeron puntos desde el archivo.\n";
        return 1;
    }

    std::vector<Nodo> arbol;

    Crono crono;
    crono.iniciar();

    if (metodo == "nearest") {
        arbol = construir_nearest_x(puntos);
    } else if (metodo == "str") {
        arbol = construir_str(puntos);
    } else {
        std::cerr << "Metodo desconocido: " << metodo << "\n";
        std::cerr << "Metodos validos: nearest, str\n";
        return 1;
    }

    double tiempo_ms = crono.ms();

    if (arbol.empty()) {
        std::cerr << "El metodo de construccion retornó un arbol vacio.\n";
        std::cerr << "nearest.cpp o str.cpp aun no esta implementado.\n";
        return 1;
    }

    IOStats io;
    escribir_nodos_bin(ruta_salida, arbol, &io);

    std::cout << "metodo=" << metodo << "\n";
    std::cout << "puntos=" << puntos.size() << "\n";
    std::cout << "nodos=" << arbol.size() << "\n";
    std::cout << "tiempo_creacion_ms=" << tiempo_ms << "\n";
    std::cout << "escrituras_disco=" << io.escrituras << "\n";
    std::cout << "archivo_salida=" << ruta_salida << "\n";

    return 0;
}

static int comando_query(int argc, char** argv) {
    if (argc != 7) {
        std::cerr << "Uso: ./rtree query <archivo_tree.bin> <xmin> <xmax> <ymin> <ymax>\n";
        return 1;
    }

    std::string ruta_arbol = argv[2];

    float xmin = std::stof(argv[3]);
    float xmax = std::stof(argv[4]);
    float ymin = std::stof(argv[5]);
    float ymax = std::stof(argv[6]);

    MBR consulta{xmin, xmax, ymin, ymax};

    IOStats io;

    Crono crono;
    crono.iniciar();

    std::vector<Punto> resultado = buscar_rango(ruta_arbol, consulta, &io);

    double tiempo_ms = crono.ms();

    std::cout << "puntos_encontrados=" << resultado.size() << "\n";
    std::cout << "lecturas_disco=" << io.lecturas << "\n";
    std::cout << "tiempo_busqueda_ms=" << tiempo_ms << "\n";

    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        imprimir_uso();
        return 1;
    }

    std::string comando = argv[1];

    try {
        if (comando == "check-data") {
            return comando_check_data(argc, argv);
        }

        if (comando == "build") {
            return comando_build(argc, argv);
        }

        if (comando == "query") {
            return comando_query(argc, argv);
        }

        std::cerr << "Comando desconocido: " << comando << "\n";
        imprimir_uso();
        return 1;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
}