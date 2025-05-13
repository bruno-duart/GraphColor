#!/bin/bash

# Path to the CSV file
csv_file="instances_with_k.csv"

g++ run_main_tabu_v2.cpp lib/graphs.cpp lib/solution.cpp lib/utils.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/TabuGraphColoringV2.cpp -I include -o run_tabu_v2

# tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
#     for max_iter in 10 100 1000 10000; do
#         for T_iter in 5 10 25 50 75 100; do
#             for n_test in {1..20}; do
#                 echo ./run_tabu_v2 "inst: $instance_name.col" "colors: $k" "T_iter: $T_iter" "Max_iter: $max_iter" "output: results_tabu_v2/analysis/$instance_name.txt" "method: greedy" "test: $n_test"
#                 ./run_tabu_v2 instances/$instance_name.col $k $T_iter $max_iter results_tabu_v2/analysis/$instance_name.txt
#             done
#         done
#     done
# done

tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
    for n_test in {1..10}; do
        echo ./run_tabu_v2 "inst: $instance_name.col" "colors: $k" "T_iter: 25" "Max_iter: 10000" "output: results_tabu_v2/results/$instance_name.txt" "method: greedy" "test: $n_test"
        ./run_tabu_v2 instances/$instance_name.col $k 25 10000 results_tabu_v2/results/$instance_name.txt
    done
done

# ./run_sa instances/fpsol2.i.1.col 65 100 results_sa/fpsol2.i.1.txt
