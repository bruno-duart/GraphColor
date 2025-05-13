#!/bin/bash

# Path to the CSV file
csv_file="instances_with_k.csv"

g++ run_main_sa.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/utils.cpp lib/SAGraphColoring.cpp -I include -o run_sa_v2

# tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
#     for T_max in 100 1000 5000 10000; do
#         for alpha_int in 6 7 8 9; do
#             for n_test in {1..20}; do
#                 echo ./run_sa_v2 "inst: $instance_name.col" "colors: $k" "T_max: $T_max" "alpha_int: $alpha_int" "output: results_sa_v2/analysis/$instance_name.txt" "method: greedy" "test: $n_test"
#                 ./run_sa_v2 instances/$instance_name.col $k $T_max $alpha_int results_sa_v2/analysis/$instance_name.txt
#             done
#         done
#     done
# done

tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
    for n_test in {1..10}; do
        echo ./run_sa_v2 "inst: $instance_name.col" "colors: $k" "T_max: 100" "alpha_int: 9" "output: results_sa_v2/results/$instance_name.txt" "method: greedy" "test: $n_test"
        ./run_sa_v2 instances/$instance_name.col $k 100 9 results_sa_v2/results/$instance_name.txt
    done
done

# ./run_sa_v2 instances/fpsol2.i.1.col 65 100 6 results_sa_v2/calibrar/fpsol2.i.1.txt
