#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_PER_RANK 200 /* Nodes/Rank: How 'fat' the DAG should be. */
#define MAX_PER_RANK 200
#define MIN_RANKS 200 /* Ranks: How 'tall' the DAG should be. */
#define MAX_RANKS 200
#define PERCENT 40 /* Chance of having an Edge. */

void generate_and_write_graph(const char *filename)
{
    int i, j, k;
    int total_nodes = 0;
    int ranks = MIN_RANKS + (rand() % (MAX_RANKS - MIN_RANKS + 1));
    int **out_adj_list = NULL;
    int *list_sizes = NULL;

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo para escrita");
        exit(EXIT_FAILURE);
    }

    /* Gerar as camadas e adicionar nós dinamicamente */
    for (i = 0; i < ranks; i++)
    {
        int new_nodes = MIN_PER_RANK + (rand() % (MAX_PER_RANK - MIN_PER_RANK + 1));

        /* Expandir a lista de adjacência */
        int **temp_list = realloc(out_adj_list, sizeof(int *) * (total_nodes + new_nodes));
        int *temp_sizes = realloc(list_sizes, sizeof(int) * (total_nodes + new_nodes));

        if (temp_list == NULL || temp_sizes == NULL)
        {
            perror("Erro ao realocar memória");
            free(out_adj_list);
            free(list_sizes);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        out_adj_list = temp_list;
        list_sizes = temp_sizes;

        /* Inicializar novas listas */
        for (j = 0; j < new_nodes; j++)
        {
            out_adj_list[total_nodes + j] = NULL;
            list_sizes[total_nodes + j] = 0;
        }

        /* Conectar nós existentes aos novos nós */
        for (j = 0; j < total_nodes; j++)
        {
            for (k = 0; k < new_nodes; k++)
            {
                if ((rand() % 100) < PERCENT)
                {
                    int source = j;
                    int target = total_nodes + k;

                    /* Adicionar `target` à lista de saída do nó `source` */
                    int current_size = list_sizes[source];
                    int *temp_adj = realloc(out_adj_list[source], sizeof(int) * (current_size + 1));

                    if (temp_adj == NULL)
                    {
                        perror("Erro ao realocar memória");
                        for (int x = 0; x < total_nodes + new_nodes; x++)
                        {
                            free(out_adj_list[x]);
                        }
                        free(out_adj_list);
                        free(list_sizes);
                        fclose(file);
                        exit(EXIT_FAILURE);
                    }

                    out_adj_list[source] = temp_adj;

                    /* Adicionar nó `target + 1` à lista de saída */
                    out_adj_list[source][current_size] = target + 1; // Incrementar para manter índice baseado em 1
                    list_sizes[source]++;
                }
            }
        }

        total_nodes += new_nodes; // Atualizar o número total de nós
    }

    /* Escrever o grafo no arquivo */
    fprintf(file, "%d\n", total_nodes); // Número total de nós
    for (i = 0; i < total_nodes; i++)
    {
        fprintf(file, "%d", list_sizes[i]); // Tamanho da lista de adjacência
        for (j = 0; j < list_sizes[i]; j++)
        {
            fprintf(file, " %d", out_adj_list[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);

    /* Liberar memória */
    for (i = 0; i < total_nodes; i++)
    {
        free(out_adj_list[i]);
    }
    free(out_adj_list);
    free(list_sizes);
}

int main()
{
    srand(time(NULL));
    generate_and_write_graph("graph.txt");
    printf("Grafo gerado e salvo em 'graph.txt'\n");
    return 0;
}
