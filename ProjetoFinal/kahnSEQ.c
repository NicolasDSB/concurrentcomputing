#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void read_graph_from_file(const char *filename, int ***out_adj_list, int **list_sizes, int *total_nodes)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo para leitura");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", total_nodes); // Ler o número total de nós

    /* Alocar memória para a lista de adjacência e os tamanhos */
    *out_adj_list = malloc(sizeof(int *) * (*total_nodes));
    *list_sizes = malloc(sizeof(int) * (*total_nodes));
    if (*out_adj_list == NULL || *list_sizes == NULL)
    {
        perror("Erro ao alocar memória");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    /* Ler as listas de adjacência */
    for (int i = 0; i < *total_nodes; i++)
    {
        int size;
        fscanf(file, "%d", &size); // Tamanho da lista de adjacência
        (*list_sizes)[i] = size;
        (*out_adj_list)[i] = malloc(sizeof(int) * size);
        if ((*out_adj_list)[i] == NULL)
        {
            perror("Erro ao alocar memória");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < size; j++)
        {
            fscanf(file, "%d", &(*out_adj_list)[i][j]);
        }
    }

    fclose(file);
}

/* Função para calcular a ordenação topológica com o algoritmo de Kahn usando a lista de adjacência de entrada */
void kahn_topological_sort(int **adj_list_out, int *list_sizes, int total_nodes, int *in_degree)
{
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

        /* Reduzir o grau de entrada dos nós vizinhos */
        for (int j = 0; j < list_sizes[current]; j++)
        {
            int target = adj_list_out[current][j] - 1; // Ajustar índice para 0-based
            in_degree[target]--;                       // Decrementar o grau de entrada do nó alvo

            if (in_degree[target] == 0) // Se o grau de entrada for 0, adicione à fila
            {
                queue[rear++] = target;
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
    // Adicionar timer
    clock_t start_time, end_time;
    double time_taken;
    int **out_adj_list = NULL;
    int *list_sizes = NULL;
    int total_nodes;

    read_graph_from_file("graph.txt", &out_adj_list, &list_sizes, &total_nodes);
    /* Vetor para armazenar o grau de entrada de cada nó (in_degree) */
    int *in_degree = calloc(total_nodes, sizeof(int)); // Inicializa com 0
    if (in_degree == NULL)
    {
        perror("Erro ao alocar memória para in_degree");
        return 0;
    }
    /* Calcular o grau de entrada de cada nó */
    for (int i = 0; i < total_nodes; i++)
    {
        for (int j = 0; j < list_sizes[i]; j++)
        {
            int target = out_adj_list[i][j] - 1; // Ajustar índice para 0-based
            in_degree[target]++;
        }
    }
    start_time = clock();
    kahn_topological_sort(out_adj_list, list_sizes, total_nodes, in_degree);
    end_time = clock();
    time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    int total_edges = 0;
    for (int i = 0; i < total_nodes; i++)
    {
        total_edges += list_sizes[i]; // Soma o número de arestas para o nó i
    }
    printf("Complexidade: O(%d)\n", total_edges + total_nodes);
    printf("%f sec", time_taken);

    /* Liberar memória */
    for (int i = 0; i < total_nodes; i++)
    {
        free(out_adj_list[i]);
    }
    free(out_adj_list);
    free(list_sizes);

    return 0;
}