from pathlib import Path
import csv
import random
import subprocess
import sys


def crear_carpeta_resultados(raiz: Path) -> Path:
    carpeta = raiz / "outputs" / "results"
    carpeta.mkdir(parents=True, exist_ok=True)
    return carpeta


def obtener_archivos_arboles(raiz: Path, datasets, metodos, n):
    archivos = {}

    for dataset in datasets:
        for metodo in metodos:
            ruta = raiz / "outputs" / "trees" / f"{dataset}_{metodo}_{n}.bin"
            archivos[(dataset, metodo)] = ruta

    return archivos


def interpretar_salida_rtree(salida: str):
    datos = {
        "points_found": None,
        "disk_reads": None,
        "query_time_ms": None,
    }

    for linea in salida.splitlines():
        linea = linea.strip()

        if not linea or "=" not in linea:
            continue

        clave, valor = linea.split("=", 1)
        clave = clave.strip()
        valor = valor.strip()

        if clave == "puntos_encontrados":
            datos["points_found"] = int(valor)
        elif clave == "lecturas_disco":
            datos["disk_reads"] = int(valor)
        elif clave == "tiempo_busqueda_ms":
            datos["query_time_ms"] = float(valor)

    return datos


def ejecutar_consulta(rtree: Path, archivo_arbol: Path, xmin, xmax, ymin, ymax):
    comando = [
        str(rtree),
        "query",
        str(archivo_arbol),
        str(xmin),
        str(xmax),
        str(ymin),
        str(ymax),
    ]

    proceso = subprocess.run(comando, capture_output=True, text=True)

    if proceso.returncode != 0:
        raise RuntimeError(f"Error al ejecutar rtree:\n{proceso.stderr}")

    datos = interpretar_salida_rtree(proceso.stdout)

    if datos["points_found"] is None or datos["disk_reads"] is None or datos["query_time_ms"] is None:
        raise RuntimeError(f"No se pudo interpretar la salida de rtree:\n{proceso.stdout}")

    return datos


def main():
    raiz = Path(__file__).resolve().parent.parent

    n = 16777216
    datasets = ["random", "europa"]
    metodos = ["nearest", "str"]
    tamanos = [0.0025, 0.005, 0.01, 0.025, 0.05]
    consultas_por_caso = 100

    rtree = raiz / "rtree"

    if not rtree.exists():
        print("Error: no se encontró el ejecutable ./rtree.")
        print("Primero compila el proyecto con: make")
        sys.exit(2)

    carpeta_resultados = crear_carpeta_resultados(raiz)
    archivo_csv = carpeta_resultados / "query_results.csv"

    archivos_arboles = obtener_archivos_arboles(raiz, datasets, metodos, n)
    faltantes = [ruta for ruta in archivos_arboles.values() if not ruta.exists()]

    if faltantes:
        print("Error: faltan archivos de árboles.")
        print("Primero ejecuta: make run-build-experiments")
        print("Archivos faltantes:")

        for ruta in faltantes:
            print(f"  - {ruta}")

        sys.exit(3)

    encabezado = [
        "dataset",
        "method",
        "N",
        "s",
        "query_id",
        "xmin",
        "xmax",
        "ymin",
        "ymax",
        "points_found",
        "disk_reads",
        "query_time_ms",
    ]

    with archivo_csv.open("w", newline="") as archivo:
        escritor = csv.writer(archivo)
        escritor.writerow(encabezado)

        for dataset in datasets:
            for metodo in metodos:
                archivo_arbol = archivos_arboles[(dataset, metodo)]

                for s in tamanos:
                    for consulta_id in range(consultas_por_caso):
                        xmin = random.uniform(0.0, 1.0 - s)
                        ymin = random.uniform(0.0, 1.0 - s)
                        xmax = xmin + s
                        ymax = ymin + s

                        print(
                            f"dataset={dataset}, metodo={metodo}, "
                            f"s={s}, consulta={consulta_id}"
                        )

                        try:
                            resultado = ejecutar_consulta(rtree, archivo_arbol, xmin, xmax, ymin, ymax)
                        except Exception as error:
                            print(f"Error ejecutando consulta: {error}")
                            sys.exit(4)

                        fila = [
                            dataset,
                            metodo,
                            n,
                            s,
                            consulta_id,
                            f"{xmin:.8f}",
                            f"{xmax:.8f}",
                            f"{ymin:.8f}",
                            f"{ymax:.8f}",
                            resultado["points_found"],
                            resultado["disk_reads"],
                            resultado["query_time_ms"],
                        ]

                        escritor.writerow(fila)

    print(f"Resultados de consultas guardados en: {archivo_csv}")


if __name__ == "__main__":
    main()