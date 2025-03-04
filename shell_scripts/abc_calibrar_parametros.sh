#!/bin/bash

# Path to the CSV file
csv_file="instance_calibrar.csv"

g++ run_main_abc_calibrar_parametros.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o run_abc_calib

tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
    for method in "p" ; do
        for bees in 50 100 200 300 400; do
            for limit in 5 10 25 50; do
                for max_iter in 20 50 70 80; do
                    for n_test in {1..10}; do
                        echo ./run_abc_calib "inst: $instance_name.col" "colors: $k" "bees: $bees" "limit: $limit" "max_iter: $max_iter" "output: results_abc/$output_folder/$instance_name.txt" "method: $method" "rcl_size: 1" "test: $n_test" "local_search:random"
                        ./run_abc_calib instances/$instance_name.col $k $bees $limit $max_iter resultado_calibrar_parametro/$instance_name.txt $method 1
                    done
                done
            done
        done
    done

done
