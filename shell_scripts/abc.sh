#!/bin/bash

# Path to the CSV file
csv_file="instances_with_k.csv"

#g++ run_main_abc.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o run_abc

tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
    for method in "p" "g" "r"; do
        if [ "$method" = "p" ]; then
            output_folder="pseudo_greedy"
        elif [ "$method" = "g" ]; then
            output_folder="grasp_build"
        else
            output_folder="random"
        fi
        for bees in 50 100 200 300 400; do
            for limit in 5 10 25 50; do
                for max_iter in 20 50 70 80; do
                    for n_test in {1..30}; do
                        echo ./run_abc "inst: $instance_name.col" "colors: $k" "bees: $bees" "limit: $limit" "max_iter: $max_iter" "output: results_abc/$output_folder/$instance_name.txt" "method: $method" "test: $n_test"
                        #./run_abc instances/$instance_name.col $k $bees $limit $max_iter results_abc/$output_folder/$instance_name.txt $method
                    done
                done
            done
        done
    done

done
