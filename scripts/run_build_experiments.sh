#!/usr/bin/env bash

set -e

EXEC="./rtree"

DATA_RANDOM="data/random.bin"
DATA_EUROPA="data/europa.bin"

OUT_TREES="outputs/trees"
OUT_RESULTS="outputs/results"

CSV="${OUT_RESULTS}/build_times.csv"

mkdir -p "$OUT_TREES"
mkdir -p "$OUT_RESULTS"

echo "dataset,method,N,nodes,build_time_ms,writes,tree_file" > "$CSV"

datasets=("random" "europa")


methods=("nearest" "str")

for exp in {15..24}; do
    N=$((2 ** exp))

    for dataset in "${datasets[@]}"; do
        if [ "$dataset" = "random" ]; then
            data_file="$DATA_RANDOM"
        else
            data_file="$DATA_EUROPA"
        fi

        for method in "${methods[@]}"; do
            tree_file="${OUT_TREES}/${dataset}_${method}_${N}.bin"

            echo "Construyendo dataset=${dataset}, method=${method}, N=${N}"

            output=$($EXEC build "$method" "$data_file" "$tree_file" "$N")

            puntos=$(echo "$output" | grep "^puntos=" | cut -d "=" -f 2)
            nodes=$(echo "$output" | grep "^nodos=" | cut -d "=" -f 2)
            time_ms=$(echo "$output" | grep "^tiempo_creacion_ms=" | cut -d "=" -f 2)
            writes=$(echo "$output" | grep "^escrituras_disco=" | cut -d "=" -f 2)

            echo "${dataset},${method},${puntos},${nodes},${time_ms},${writes},${tree_file}" >> "$CSV"
        done
    done
done

echo "Resultados guardados en ${CSV}"