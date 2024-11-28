#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 12
#define MIN_PER_RANK 100 /* Nodes/Rank: How 'fat' the DAG should be. */
#define MAX_PER_RANK 100
#define MIN_RANKS 100 /* Ranks: How 'tall' the DAG should be. */
#define MAX_RANKS 100
#define PERCENT 40 /* Chance of having an Edge. */

// Estruturas compartilhadas
int **adj_list = NULL;     // Lista de adjacência de entrada
int *list_sizes = NULL;    // Tamanho de cada lista
int *in_degree;            // Grau de entrada de cada nó
int total_nodes;           // Total de nós
pthread_mutex_t mutex;     // Mutex para sincronizar o acesso à ready_queue
int *ready_queue;          // Fila de nós com grau de entrada zero
int front = 0, rear = 0;   // Índices para a fila
pthread_barrier_t barrier; // Barreira para sincronização das threads
int *top_order;            // Vetor para armazenar a ordenação
int top_order_index = 0;   // Índice atual na ordenação

void *kahn_thread(void *arg)
{
    int thread_id = *(int *)arg;
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
        /* Reduzir o grau de entrada dos nós vizinhos */
        for (int i = 0; i < total_nodes; i++) // Buscar em todos os nós
        {
            for (int j = 0; j < list_sizes[i]; j++) // Percorrer lista de adjacência de entrada
            {
                if (adj_list[i][j] - 1 == current) // Verificar se o nó `current` está na lista de adjacências do nó `i`
                {
                    pthread_mutex_lock(&mutex);
                    in_degree[i]--;        // Decrementar o grau de entrada do nó `i`
                    if (in_degree[i] == 0) // Se o grau de entrada de `i` for 0, adicione à fila
                    {
                        ready_queue[rear++] = i;
                    }
                    pthread_mutex_unlock(&mutex);
                }
            }
        }
    }

    return NULL;
}
int main(void)
{
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    int i, j, k;
    srand(time(NULL));
    // Adicionar timer
    clock_t start_time, end_time;
    double time_taken;

    int ranks = MIN_RANKS + (rand() % (MAX_RANKS - MIN_RANKS + 1));

    /* Gerar as camadas e adicionar nós dinamicamente */
    for (i = 0; i < ranks; i++)
    {
        int new_nodes = MIN_PER_RANK + (rand() % (MAX_PER_RANK - MIN_PER_RANK + 1));

        /* Expandir a lista de adjacência */
        int **temp_list = (int **)realloc(adj_list, sizeof(int *) * (total_nodes + new_nodes));
        int *temp_sizes = (int *)realloc(list_sizes, sizeof(int) * (total_nodes + new_nodes));

        if (temp_list == NULL || temp_sizes == NULL)
        {
            perror("Erro ao realocar memória");
            free(adj_list);
            free(list_sizes);
            return EXIT_FAILURE;
        }

        adj_list = temp_list;
        list_sizes = temp_sizes;

        /* Inicializar novas listas */
        for (j = 0; j < new_nodes; j++)
        {
            adj_list[total_nodes + j] = NULL;
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
                    int *temp_adj = (int *)realloc(adj_list[target], sizeof(int) * (current_size + 1));

                    if (temp_adj == NULL)
                    {
                        perror("Erro ao realocar memória");
                        for (int x = 0; x < total_nodes + new_nodes; x++)
                        {
                            free(adj_list[x]);
                        }
                        free(adj_list);
                        free(list_sizes);
                        return EXIT_FAILURE;
                    }

                    adj_list[target] = temp_adj;

                    /* Adicionar nó `j + 1` à lista de entrada (incrementar +1 para representar a numeração correta) */
                    adj_list[target][current_size] = j + 1;
                    list_sizes[target]++;
                }
            }
        }

        total_nodes += new_nodes; // Atualizar o número total de nós
    }
    // Inicialização do grafo (não mostrado aqui)

    // Inicializar estruturas compartilhadas
    ready_queue = (int *)malloc(sizeof(int) * total_nodes);
    in_degree = (int *)malloc(sizeof(int) * total_nodes);
    top_order = (int *)malloc(sizeof(int) * total_nodes);
    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    // Calcular o grau de entrada e preparar a fila inicial
    for (i = 0; i < total_nodes; i++)
    {
        in_degree[i] = list_sizes[i];
        if (in_degree[i] == 0)
        {
            ready_queue[rear++] = i;
        }
    }
    start_time = clock();
    // Criar threads
    for (i = 0; i < NUM_THREADS; i++)
    {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, kahn_thread, &thread_ids[i]);
    }

    // Aguardar todas as threads terminarem
    for (i = 0; i < NUM_THREADS; i++)
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
    printf("Complexidade: O(%d)\n", (total_edges / 2) + total_nodes);
    printf("%f sec", time_taken);

    // Liberar recursos
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    free(ready_queue);
    free(in_degree);

    return 0;
}