# GraphColor **Repositório em desenvolvimento**

Este projeto implementa algoritmos de Coloração de Grafos, com o objetivo de resolver o problema de coloração utilizando diferentes métodos. A coloração de grafos é uma técnica útil em diversas áreas como alocação de frequências, escalonamento de tarefas, e problemas de otimização combinatória.

## Funcionalidades

- **Métodos implementados:**
  - Algoritmos gulosos com introdução de aleatoriedade para gerar variedade de soluções iniciais
  - GRASP (Greedy Randomized Adaptive Search Procedure)
  - Colônia de Abelhas Artificiais (Artificial Bee Colony)
- **Suporte para leitura e escrita de arquivos DIMACS**:
  - Grafos representados como lista de adjacência para otimizar o consumo de memória.
- **Análise estatística de combinações de parâmetros**:
  - Realização de grid search com análise dos resultados para encontrar a melhor combinação de parâmetros.
  
<!-- ## Estrutura do Projeto

- `src/`: Contém o código-fonte principal do projeto.
- `data/`: Contém os arquivos de grafos em formato DIMACS para testes e simulações.
- `results/`: Contém os resultados gerados pelas execuções dos algoritmos.
- `scripts/`: Scripts auxiliares para execução de testes e análise de resultados.
- `tests/`: Contém os testes unitários para validação do funcionamento correto dos algoritmos. -->

## Como executar

### Pré-requisitos

- **C++** (Compilador compatível com o padrão C++11 ou superior)
<!-- - **CMake** (para a construção do projeto) -->
- **Bibliotecas padrão**: STL (Standard Template Library)
- **Editor de código**: Ferramenta da sua preferência

#### Configurações utilizadas neste projeto

- **Sistema Operacional**: Windows Subsystem for Linux (WSL) - Ubuntu 22.04 LTS
- **C++**: Compilador g++ versão
- **Editor de código**: Visual Studio Code

### Compilação e Execução -- EM CONSTRUÇÃO ---

1. Clone o repositório:

   ```bash
   git clone https://github.com/bruno-duart/GraphColor.git
   ```

2. Navegue até o diretório do projeto:

   ```bash
   cd GraphColor
   ```

3. Para compilar:
    1. Greedy Method:

        ```bash
        g++ run_main_greedy.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/utils.cpp -I include -o run_greedy
        ```

    2. GRASP:

    3. Artificial Bee Colony:

        ```bash
        g++ run_main_abc.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o run_abc
        ```

4. Para executar um único caso de teste:
    1. Greedy Method:

        ```bash
        ./run_greedy instances/$instance_name.col $k results_greedy/$instance_name.txt
        ```

    2. GRASP:

    3. Artificial Bee Colony:

        ```bash
        ./run_abc instances/$instance_name.col $k $bees $limit $max_iter results_abc/random/$instance_name.txt
        ```

    Considere:
    - **$instance_name**: nome do caso de teste (disponíveis na pasta instances)
    - **$k**: número de cores disponível para colorir o grafo
    - **$bees**: número de abelhas da colméia (50% abelhas operárias, e 50% abelhas observadoras)
    - **$limit**: número limite de tentativas de melhora da abelha antes de ser resetada
    - **$max_iter**: número máximo de iterações sem melhora

    Os resultados da execução serão armazenados em "results_greedy/$instance_name.txt" e "results_abc/random/$instance_name.txt"

5. Para executar todos os casos de teste, considere utilizar os scripts da pasta **shell_scripts**. Estes scripts consideram o arquivo **instances_with_k** como guia para escolha de $k e outras informações
    1. Greedy Method:

        ```bash
        bash shell_scripts/greedy.sh
        ```

    2. GRASP:

    3. Artificial Bee Colony:

        ```bash
        bash shell_scripts/abc.sh
        ```

## Contribuição

Contribuições são bem-vindas! Siga os passos abaixo para contribuir com este projeto:

1. Faça um fork do repositório

2. Crie uma nova branch:

      ```bash
      git checkout -b minha-nova-funcionalidade
      ```

3. Faça suas alteraçõs e adicione os commits:

      ```bash
      git commit -m "Adicionei nova funcionalidade"
      ```

4. Envie as alterações para seu fork:

      ```bash
      git push origin minha-nova-funcionalidade
      ```

5. Abra um Pull Request no repositório original.
