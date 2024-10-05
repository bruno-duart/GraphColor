#!/bin/bash

# Path to the CSV file
csv_file="instances_with_k.csv"

g++ run_main_grasp.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o run_grasp

tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
    for method in "b" "g"; do
        if [ "$method" = "b" ]; then
            output_folder="build_phase"
        elif [ "$method" = "g" ]; then
            output_folder="grasp_full"
        fi
        for n_test in {1..30}; do
            echo ./run_grasp "inst: $instance_name.col" "colors: $k" "method: $method" "output: results_grasp/$output_folder/$instance_name.txt""test: $n_test"
            ./run_grasp instances/$instance_name.col $k $method results_grasp/$output_folder/$instance_name.txt
        done
    done

done
