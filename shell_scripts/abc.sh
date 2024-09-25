#!/bin/bash

# Path to the CSV file
csv_file="instances_with_k.csv"

g++ run_main_abc.cpp lib/graphs.cpp lib/solution.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o run_abc

tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
    for bees in 50 100 200 300 400; do
        for limit in 5 10 25 50; do
            for max_iter in 20 50 70 80; do
                for n_test in {1..30}; do
                    echo ./run_abc "inst: $instance_name.col" "colors: $k" "bees: $bees" "limit: $limit" "max_iter: $max_iter" "output: results_abc/random/$instance_name.txt" "test: $n_test" 
                    ./run_abc instances/$instance_name.col $k $bees $limit $max_iter results_abc/random/$instance_name.txt
                done
            done
        done
    done
done
