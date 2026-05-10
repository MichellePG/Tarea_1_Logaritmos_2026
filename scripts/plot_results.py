from pathlib import Path
import csv
import statistics
import sys

import matplotlib.pyplot as plt


def crear_carpeta_plots(raiz: Path) -> Path:
    """
    Crea la carpeta donde se guardarán los gráficos.

    Parámetros:
        raiz: Ruta raíz del proyecto.

    Retorna:
        Ruta de la carpeta de gráficos.
    """
    carpeta = raiz / "outputs" / "plots"
    carpeta.mkdir(parents=True, exist_ok=True)
    return carpeta


def leer_build_times(archivo_csv: Path):
    """
    Lee los resultados de construcción desde un CSV.

    Parámetros:
        archivo_csv: Ruta al archivo build_times.csv.

    Retorna:
        Lista de filas con los datos de construcción.
    """
    datos = []

    with archivo_csv.open(newline="") as archivo:
        lector = csv.DictReader(archivo)

        for fila in lector:
            datos.append({
                "dataset": fila["dataset"],
                "method": fila["method"],
                "N": int(fila["N"]),
                "nodes": int(fila["nodes"]),
                "build_time_ms": float(fila["build_time_ms"]),
                "writes": int(fila["writes"]),
            })

    return datos


def leer_query_results(archivo_csv: Path):
    """
    Lee los resultados individuales de consultas desde un CSV.

    Parámetros:
        archivo_csv: Ruta al archivo query_results.csv.

    Retorna:
        Lista de filas con los datos de consultas.
    """
    datos = []

    with archivo_csv.open(newline="") as archivo:
        lector = csv.DictReader(archivo)

        for fila in lector:
            datos.append({
                "dataset": fila["dataset"],
                "method": fila["method"],
                "N": int(fila["N"]),
                "s": float(fila["s"]),
                "query_id": int(fila["query_id"]),
                "points_found": int(fila["points_found"]),
                "disk_reads": int(fila["disk_reads"]),
                "query_time_ms": float(fila["query_time_ms"]),
            })

    return datos


def agrupar_build_times(datos):
    """
    Agrupa los resultados de construcción por dataset y método.

    Parámetros:
        datos: Lista de resultados de construcción.

    Retorna:
        Diccionario con los datos agrupados.
    """
    grupos = {}

    for fila in datos:
        clave = (fila["dataset"], fila["method"])
        grupos.setdefault(clave, []).append(fila)

    for clave in grupos:
        grupos[clave].sort(key=lambda fila: fila["N"])

    return grupos


def resumir_consultas(datos):
    """
    Calcula promedios y desviaciones estándar de las consultas.

    Parámetros:
        datos: Lista de resultados individuales de consultas.

    Retorna:
        Lista con los resultados resumidos por dataset, método y s.
    """
    acumulados = {}

    for fila in datos:
        clave = (fila["dataset"], fila["method"], fila["s"])

        if clave not in acumulados:
            acumulados[clave] = {
                "disk_reads": [],
                "points_found": [],
                "query_time_ms": [],
            }

        acumulados[clave]["disk_reads"].append(fila["disk_reads"])
        acumulados[clave]["points_found"].append(fila["points_found"])
        acumulados[clave]["query_time_ms"].append(fila["query_time_ms"])

    resumen = []

    for (dataset, method, s), valores in acumulados.items():
        puntos = valores["points_found"]

        resumen.append({
            "dataset": dataset,
            "method": method,
            "s": s,
            "avg_disk_reads": statistics.mean(valores["disk_reads"]),
            "avg_points_found": statistics.mean(puntos),
            "std_points_found": statistics.stdev(puntos) if len(puntos) > 1 else 0.0,
            "avg_query_time_ms": statistics.mean(valores["query_time_ms"]),
        })

    resumen.sort(key=lambda fila: (fila["dataset"], fila["method"], fila["s"]))
    return resumen


def guardar_resumen_consultas(resumen, archivo_csv: Path):
    """
    Guarda el resumen de consultas en un CSV.

    Parámetros:
        resumen: Lista con resultados resumidos.
        archivo_csv: Ruta del archivo de salida.

    Retorna:
        None.
    """
    columnas = [
        "dataset",
        "method",
        "s",
        "avg_disk_reads",
        "avg_points_found",
        "std_points_found",
        "avg_query_time_ms",
    ]

    with archivo_csv.open("w", newline="") as archivo:
        escritor = csv.DictWriter(archivo, fieldnames=columnas)
        escritor.writeheader()
        escritor.writerows(resumen)


def agrupar_resumen_consultas(resumen):
    """
    Agrupa el resumen de consultas por dataset y método.

    Parámetros:
        resumen: Lista con resultados resumidos.

    Retorna:
        Diccionario con los resultados agrupados.
    """
    grupos = {}

    for fila in resumen:
        clave = (fila["dataset"], fila["method"])
        grupos.setdefault(clave, []).append(fila)

    for clave in grupos:
        grupos[clave].sort(key=lambda fila: fila["s"])

    return grupos


def etiqueta(dataset: str, method: str) -> str:
    """
    Genera la etiqueta usada en los gráficos.

    Parámetros:
        dataset: Nombre del dataset.
        method: Nombre del método.

    Retorna:
        Texto de la etiqueta.
    """
    return f"{dataset} - {method}"


def graficar_tiempos_construccion(grupos, carpeta_plots: Path):
    """
    Genera el gráfico de tiempos de construcción.

    Parámetros:
        grupos: Resultados de construcción agrupados.
        carpeta_plots: Carpeta donde se guarda el gráfico.

    Retorna:
        None.
    """
    plt.figure()

    for (dataset, method), filas in grupos.items():
        x = [fila["N"] for fila in filas]
        y = [fila["build_time_ms"] for fila in filas]

        plt.plot(x, y, marker="o", label=etiqueta(dataset, method))

    plt.xscale("log", base=2)
    plt.xlabel("Cantidad de puntos N")
    plt.ylabel("Tiempo de construcción (ms)")
    plt.title("Tiempo de construcción de los R-trees")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(carpeta_plots / "build_times.png")
    plt.close()


def graficar_lecturas_consultas(grupos, carpeta_plots: Path):
    """
    Genera el gráfico de lecturas promedio por consulta.

    Parámetros:
        grupos: Resultados resumidos de consultas agrupados.
        carpeta_plots: Carpeta donde se guarda el gráfico.

    Retorna:
        None.
    """
    plt.figure()

    for (dataset, method), filas in grupos.items():
        x = [fila["s"] for fila in filas]
        y = [fila["avg_disk_reads"] for fila in filas]

        plt.plot(x, y, marker="o", label=etiqueta(dataset, method))

    plt.xlabel("Lado del cuadrado s")
    plt.ylabel("Lecturas a disco promedio")
    plt.title("Lecturas a disco promedio por consulta")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(carpeta_plots / "query_disk_reads.png")
    plt.close()


def graficar_puntos_consultas(grupos, carpeta_plots: Path):
    """
    Genera el gráfico de puntos promedio con barras de error.

    Parámetros:
        grupos: Resultados resumidos de consultas agrupados.
        carpeta_plots: Carpeta donde se guarda el gráfico.

    Retorna:
        None.
    """
    plt.figure()

    for (dataset, method), filas in grupos.items():
        x = [fila["s"] for fila in filas]
        y = [fila["avg_points_found"] for fila in filas]
        error = [fila["std_points_found"] for fila in filas]

        plt.errorbar(
            x,
            y,
            yerr=error,
            marker="o",
            capsize=4,
            label=etiqueta(dataset, method),
        )

    plt.xlabel("Lado del cuadrado s")
    plt.ylabel("Puntos encontrados promedio")
    plt.title("Puntos encontrados promedio por consulta")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(carpeta_plots / "query_points_found.png")
    plt.close()


def main():
    """
    Lee resultados, calcula resúmenes y genera los gráficos.

    Parámetros:
        Ninguno.

    Retorna:
        None.
    """
    raiz = Path(__file__).resolve().parent.parent

    archivo_build = raiz / "outputs" / "results" / "build_times.csv"
    archivo_query = raiz / "outputs" / "results" / "query_results.csv"

    if not archivo_build.exists():
        print(f"Error: no se encontró {archivo_build}")
        print("Primero ejecuta: make run-build-experiments")
        sys.exit(2)

    if not archivo_query.exists():
        print(f"Error: no se encontró {archivo_query}")
        print("Primero ejecuta: make run-query-experiments")
        sys.exit(3)

    carpeta_plots = crear_carpeta_plots(raiz)

    build_times = leer_build_times(archivo_build)
    query_results = leer_query_results(archivo_query)

    grupos_build = agrupar_build_times(build_times)

    resumen_consultas = resumir_consultas(query_results)
    archivo_resumen = raiz / "outputs" / "results" / "query_summary.csv"
    guardar_resumen_consultas(resumen_consultas, archivo_resumen)

    grupos_query = agrupar_resumen_consultas(resumen_consultas)

    graficar_tiempos_construccion(grupos_build, carpeta_plots)
    graficar_lecturas_consultas(grupos_query, carpeta_plots)
    graficar_puntos_consultas(grupos_query, carpeta_plots)

    print(f"Resumen de consultas guardado en: {archivo_resumen}")
    print(f"Gráficos guardados en: {carpeta_plots}")


if __name__ == "__main__":
    main()