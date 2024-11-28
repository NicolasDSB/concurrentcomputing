#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 12

// Estruturas compartilhadas
int **out_adj_list = NULL; // Lista de adjacência
int *list_sizes = NULL;    // Tamanho de cada lista
int *in_degree;            // Grau de entrada de cada nó
int total_nodes;           // Total de nós
pthread_mutex_t mutex;     // Mutex para sincronizar o acesso à ready_queue
int *ready_queue;          // Fila de nós com grau de entrada zero
int front = 0, rear = 0;   // Índices para a fila
pthread_barrier_t barrier; // Barreira para sincronização das threads
int *top_order;            // Vetor para armazenar a ordenação
int top_order_index = 0;   // Índice atual na ordenação

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

void *kahn_thread(void *arg)
{
    while (1)
    {
        int current = -1;

        // Pegar um nó da ready_queue
        pthread_mutex_lock(&mutex);
        if (front < rear)
        {
            current = ready_queue[front++];
        }
        pthread_mutex_unlock(&mutex);
        if (current == -1)
        {
            // Se não há mais nós, aguardar sincronização
            pthread_barrier_wait(&barrier);
            if (front >= rear)
                break;
            continue;
        }
        // Adicionar o nó à ordenação topológica
        pthread_mutex_lock(&mutex);
        top_order[top_order_index++] = current + 1; // Incrementar +1 para usar numeração humana
        pthread_mutex_unlock(&mutex);

        // Processar os vizinhos do nó `current`
        for (int i = 0; i < list_sizes[current]; i++) // Percorrer a lista de adjacência de saída do nó `current`
        {
            int neighbor = out_adj_list[current][i] - 1; // Ajustar para índice 0-based
            pthread_mutex_lock(&mutex);
            in_degree[neighbor]--;        // Decrementar o grau de entrada do nó vizinho
            if (in_degree[neighbor] == 0) // Se o grau de entrada for 0, adicione à fila
            {
                ready_queue[rear++] = neighbor;
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}
int main(void)
{
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    // Adicionar timer
    clock_t start_time, end_time;
    double time_taken;

    read_graph_from_file("graph.txt", &out_adj_list, &list_sizes, &total_nodes);

    // Inicializar estruturas compartilhadas
    ready_queue = malloc(sizeof(int) * total_nodes);
    in_degree = calloc(total_nodes, sizeof(int)); // Inicializa com 0
    top_order = malloc(sizeof(int) * total_nodes);
    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    // Calcular o grau de entrada baseado na lista de adjacência de saída e preparar a fila inicial
    for (int i = 0; i < total_nodes; i++)
    {
        // Percorrer a lista de adjacência de saída do nó `i`
        for (int j = 0; j < list_sizes[i]; j++)
        {
            int target = out_adj_list[i][j] - 1; // Ajustar para índice 0-based
            in_degree[target]++;                 // Incrementar o grau de entrada do nó de destino
        }
    }
    start_time = clock();
    // Adicionar à fila inicial os nós com grau de entrada 0
    for (int i = 0; i < total_nodes; i++)
    {
        if (in_degree[i] == 0)
        {
            ready_queue[rear++] = i;
        }
    }
    // Criar threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, kahn_thread, NULL);
    }
    // Aguardar todas as threads terminarem
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    end_time = clock();
    time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    int total_edges = 0;
    for (int i = 0; i < total_nodes; i++)
    {
        total_edges += list_sizes[i]; // Soma o número de arestas para o nó i
    }
    printf("Complexidade: O(%d)\n", total_edges + total_nodes);
    printf("%f sec", time_taken);

    // Liberar recursos
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    free(ready_queue);
    free(in_degree);

    return 0;
}