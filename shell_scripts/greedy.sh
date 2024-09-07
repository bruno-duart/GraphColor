#!/bin/bash

# Path to the CSV file
csv_file="instances_with_k.csv"

g++ run_main_greedy.cpp graphs.cpp solution.cpp heuristics.cpp utils.cpp -I include -o run_greedy

tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
    echo ./run_greedy "inst: $instance_name.col" "colors: $k" "output: results/$instance_name.txt"
    ./run_greedy instances/$instance_name.col $k results_greedy/$instance_name.txt
done