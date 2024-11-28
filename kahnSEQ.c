#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_PER_RANK 100 /* Nodes/Rank: How 'fat' the DAG should be. */
#define MAX_PER_RANK 100
#define MIN_RANKS 100 /* Ranks: How 'tall' the DAG should be. */
#define MAX_RANKS 100
#define PERCENT 40 /* Chance of having an Edge. */

/* Função para calcular a ordenação topológica com o algoritmo de Kahn usando a lista de adjacência de entrada */
void kahn_topological_sort(int **adj_list_in, int *list_sizes, int total_nodes)
{
    /* Vetor para armazenar o grau de entrada de cada nó (in_degree) */
    int *in_degree = malloc(sizeof(int) * total_nodes);
    if (in_degree == NULL)
    {
        perror("Erro ao alocar memória para in_degree");
        return;
    }

    /* Inicializar o grau de entrada (in_degree) com os valores de list_sizes */
    for (int i = 0; i < total_nodes; i++)
    {
        in_degree[i] = list_sizes[i]; // O grau de entrada é dado pelo tamanho da lista de adjacência
    }

    /* Fila para armazenar os nós com grau de entrada 0 */
    int *queue = malloc(sizeof(int) * total_nodes);
    if (queue == NULL)
    {
        perror("Erro ao alocar memória para a fila");
        free(in_degree);
        return;
    }
    int front = 0, rear = 0;

    /* Adicionar nós com grau de entrada 0 à fila */
    for (int i = 0; i < total_nodes; i++)
    {
        if (in_degree[i] == 0) // Se o grau de entrada do nó é zero, adicione à fila
        {
            queue[rear++] = i;
        }
    }

    /* Vetor para armazenar a ordenação topológica */
    int *top_order = malloc(sizeof(int) * total_nodes);
    if (top_order == NULL)
    {
        perror("Erro ao alocar memória para top_order");
        free(queue);
        free(in_degree);
        return;
    }
    int index = 0;

    /* Processar nós da fila */
    while (front < rear)
    {
        int current = queue[front++];
        top_order[index++] = current + 1; // Ajustar índice para começar em 1

        /* Reduzir o grau de entrada dos nós vizinhos (nós que apontam para o current) */
        for (int i = 0; i < total_nodes; i++) // Buscar em todos os nós
        {
            for (int j = 0; j < list_sizes[i]; j++) // Percorrer lista de adjacência de entrada
            {
                if (adj_list_in[i][j] - 1 == current) // Verificar se o nó `current` está na lista de adjacências do nó `i`
                {
                    in_degree[i]--;        // Decrementar o grau de entrada do nó `i`
                    if (in_degree[i] == 0) // Se o grau de entrada de `i` for 0, adicione à fila
                    {
                        queue[rear++] = i;
                    }
                }
            }
        }
    }

    /* Verificar se houve um ciclo */
    if (index != total_nodes)
    {
        printf("Erro: O grafo possui um ciclo e não pode ser ordenado topologicamente.\n");
    }
    /* Liberar memória */
    free(queue);
    free(top_order);
    free(in_degree);
}
int main(void)
{
    int i, j, k;
    srand(time(NULL));
    // Adicionar timer
    clock_t start_time, end_time;
    double time_taken;

    int ranks = MIN_RANKS + (rand() % (MAX_RANKS - MIN_RANKS + 1));

    /* Inicializar tamanho acumulado de nós */
    int total_nodes = 0;

    /* Alocar uma matriz para armazenar as listas de adjacência de entrada */
    int **in_adj_list = NULL; // Ponteiro inicial nulo
    int *list_sizes = NULL;   // Para rastrear o tamanho de cada lista

    /* Gerar as camadas e adicionar nós dinamicamente */
    for (i = 0; i < ranks; i++)
    {
        int new_nodes = MIN_PER_RANK + (rand() % (MAX_PER_RANK - MIN_PER_RANK + 1));

        /* Expandir a lista de adjacência */
        int **temp_list = realloc(in_adj_list, sizeof(int *) * (total_nodes + new_nodes));
        int *temp_sizes = realloc(list_sizes, sizeof(int) * (total_nodes + new_nodes));

        if (temp_list == NULL || temp_sizes == NULL)
        {
            perror("Erro ao realocar memória");
            free(in_adj_list);
            free(list_sizes);
            return EXIT_FAILURE;
        }

        in_adj_list = temp_list;
        list_sizes = temp_sizes;

        /* Inicializar novas listas */
        for (j = 0; j < new_nodes; j++)
        {
            in_adj_list[total_nodes + j] = NULL;
            list_sizes[total_nodes + j] = 0;
        }

        /* Conectar nós existentes aos novos nós */
        for (j = 0; j < total_nodes; j++)
        {
            for (k = 0; k < new_nodes; k++)
            {
                if ((rand() % 100) < PERCENT)
                {
                    int target = total_nodes + k;

                    /* Adicionar `j` à lista de entrada do nó `target` */
                    int current_size = list_sizes[target];
                    int *temp_adj = realloc(in_adj_list[target], sizeof(int) * (current_size + 1));

                    if (temp_adj == NULL)
                    {
                        perror("Erro ao realocar memória");
                        for (int x = 0; x < total_nodes + new_nodes; x++)
                        {
                            free(in_adj_list[x]);
                        }
                        free(in_adj_list);
                        free(list_sizes);
                        return EXIT_FAILURE;
                    }

                    in_adj_list[target] = temp_adj;

                    /* Adicionar nó `j + 1` à lista de entrada (incrementar +1 para representar a numeração correta) */
                    in_adj_list[target][current_size] = j + 1; // Aqui está a mudança importante
                    list_sizes[target]++;
                }
            }
        }

        total_nodes += new_nodes; // Atualizar o número total de nós
    }
    start_time = clock();
    kahn_topological_sort(in_adj_list, list_sizes, total_nodes);
    end_time = clock();
    time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    int total_edges = 0;
    for (int i = 0; i < total_nodes; i++)
    {
        total_edges += list_sizes[i]; // Soma o número de arestas para o nó i
    }
    printf("Complexidade: O(%d)\n", (total_edges/2) + total_nodes);
    printf("%f", time_taken);
    /* Liberar memória */
    for (i = 0; i < total_nodes; i++)
    {
        free(in_adj_list[i]);
    }
    free(in_adj_list);
    free(list_sizes);

    return 0;
}