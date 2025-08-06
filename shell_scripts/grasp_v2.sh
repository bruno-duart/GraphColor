#!/bin/bash

g++ run_main_grasp_v2.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/utils.cpp lib/GRASPGraphColoringV2.cpp -I include -o run_main_grasp_v2

# Path to the CSV file 
#################### Calibrar
# csv_file="instances_with_k_analysis.csv"

# tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
#     for method in "o"; do
#         if [ "$method" = "s" ]; then
#             output_folder="swap"
#         elif [ "$method" = "k" ]; then
#             output_folder="kempe"
#         else
#             output_folder="one"
#         fi
#         for max_iter in 10 100 1000; do
#             for alpha_int in {1..10}; do
#                 for n_test in {1..20}; do
#                     echo ./run_main_grasp_v2 "inst: $instance_name.col" "colors: $k" "Max_iter: $max_iter" "alpha_int: $alpha_int" "output: results_grasp_v2/analysis/$output_folder/$instance_name.txt" "method: $method" "test: $n_test"
#                     ./run_main_grasp_v2 instances/$instance_name.col $k $max_iter $alpha_int $method results_grasp_v2/analysis/$output_folder/$instance_name.txt
#                 done
#             done
#         done
#     done

# done


###################### executar testes com parâmetros fixos

csv_file="instances_with_k.csv"

# tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
#     echo "Running Tests with Swap Local Search"
#     for n_test in {1..25}; do
#         echo ./run_main_grasp_v2 "inst: $instance_name.col" "colors: $k" "Max_iter: 100" "alpha_int: 2" "output: results_grasp_v2/results/swap/$instance_name.txt" "method: swap" "test: $n_test"
#         ./run_main_grasp_v2 instances/$instance_name.col $k 100 2 s results_grasp_v2/results/swap/$instance_name.txt
#     done
#     echo "Running Tests with Kempe Local Search"
#     for n_test in {1..25}; do
#         echo ./run_main_grasp_v2 "inst: $instance_name.col" "colors: $k" "Max_iter: 100" "alpha_int: 1" "output: results_grasp_v2/results/kempe/$instance_name.txt" "method: kempe" "test: $n_test"
#         ./run_main_grasp_v2 instances/$instance_name.col $k 100 1 s results_grasp_v2/results/kempe/$instance_name.txt
#     done
#     echo "Running Tests with One Local Search"
#     for n_test in {1..25}; do
#         echo ./run_main_grasp_v2 "inst: $instance_name.col" "colors: $k" "Max_iter: 1000" "alpha_int: 10" "output: results_grasp_v2/results/one/$instance_name.txt" "method: one" "test: $n_test"
#         ./run_main_grasp_v2 instances/$instance_name.col $k 1000 10 s results_grasp_v2/results/one/$instance_name.txt
#     done
# done


tail -n +2 "$csv_file" | while IFS=';' read -r instance_name V E D xk k group; do
    echo "Running Tests with Swap Local Search"
    for n_test in {1..25}; do
        echo ./run_main_grasp_v2 "inst: $instance_name.col" "colors: $k" "Max_iter: 100" "alpha_int: 2" "output: results_grasp_v2/results/swap/$instance_name.txt" "method: swap" "test: $n_test"
        ./run_main_grasp_v2 instances/$instance_name.col $k 100 2 s results_grasp_v2/results/swap/$instance_name.txt
    done
    echo "Running Tests with Kempe Local Search"
    for n_test in {1..25}; do
        echo ./run_main_grasp_v2 "inst: $instance_name.col" "colors: $k" "Max_iter: 10" "alpha_int: 1" "output: results_grasp_v2/results/kempe/$instance_name.txt" "method: kempe" "test: $n_test"
        ./run_main_grasp_v2 instances/$instance_name.col $k 10 1 k results_grasp_v2/results/kempe/$instance_name.txt
    done
    echo "Running Tests with One Local Search"
    for n_test in {1..25}; do
        echo ./run_main_grasp_v2 "inst: $instance_name.col" "colors: $k" "Max_iter: 100" "alpha_int: 8" "output: results_grasp_v2/results/one/$instance_name.txt" "method: one" "test: $n_test"
        ./run_main_grasp_v2 instances/$instance_name.col $k 100 8 o results_grasp_v2/results/one/$instance_name.txt
    done
done

