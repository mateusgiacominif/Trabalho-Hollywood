# Trabalho-Hollywood

Este repositório contém o código-fonte em C para um trabalho focado na representação e análise de dados sobre "Hollywood", utilizando estruturas de dados avançadas como Grafos (TG) e Árvores B (TARVBM).

## 📂 Estrutura do Repositório

O projeto é organizado com os seguintes arquivos principais:

*   **`main.c`**: Arquivo principal de execução do projeto.
*   **`TARVBM_trab.c` / `TARVBM_trab.h`**: Implementação e cabeçalho de estruturas de Árvores B (B-Trees) para armazenamento e busca eficiente de dados no disco.
*   **`TG.c` / `TG.h`**: Implementação e cabeçalho de um Grafo Direcionado ou Não Direcionado para representar as conexões (ex: Atores e Filmes).
*   **`func_questoes.c` / `func_questoes.h`**: Funções específicas para responder às questões/consultas propostas no trabalho, utilizando as estruturas acima.
*   **`data/`**: Diretório contendo as bases de dados.
    *   **`Nodes.txt`**: Dados dos vértices (atores, filmes, etc.).
    *   **`Relationships.txt`**: Dados das arestas (relações entre os vértices).
*   **`.gitignore`**: Arquivo padrão para ignorar binários e arquivos temporários.

## ⚙️ Compilação e Execução

Para compilar e executar o projeto, você precisará de um compilador C (como `gcc`).

1.  Abra o terminal na raiz do projeto.
2.  Compile todos os arquivos `.c` juntos:
    ```bash
    gcc -o hollywood main.c TARVBM_trab.c TG.c func_questoes.c
    ```
3.  Execute o programa gerado:
    ```bash
    ./hollywood
    ```
    *(No Windows, execute `hollywood.exe`)*

## 🛠️ Tecnologias Utilizadas

*   **Linguagem C**: Toda a lógica, manipulação de arquivos e estruturação de dados.
*   **Grafos**: Para modelar a rede de relacionamentos de Hollywood.
*   **Árvore B (B-Tree)**: Para indexação e recuperação otimizada das informações nos arquivos de dados.

## 👨‍💻 Autor
Desenvolvido por **Mateus Giacomini**.
* GitHub: [mateusgiacominif](https://github.com/mateusgiacominif)
