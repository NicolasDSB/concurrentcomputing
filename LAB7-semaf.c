/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: As tres threads executam de acordo com as transições de estado (t1-t2-t3) */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 3

// Variaveis globais
sem_t estado1, estado2; // semaforos para coordenar a ordem de execucao das threads
FILE *descritorArquivo; // descritor do arquivo de entrada
size_t ret;             // retorno da funcao de leitura no arquivo de entrada
int N, j;               // tamanho do buffer
char *buffer, *buffer2;
// funcao executada pela thread 1
void *t1(void *arg)
{
    int i = 0;
    printf("Thread : 1 esta executando...\n");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 0;
    }
    // le as dimensoes da matriz
    ret = fscanf(descritorArquivo, "%d", &N);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 0;
    }
    buffer = (char *)malloc(sizeof(char) * N);
    while (fscanf(descritorArquivo, "%c", &buffer[i]) != EOF)
    {
        i++;
    }
    printf("Thread : 1 mudou estado!\n");
    sem_post(&estado1);

    printf("Thread : 1 terminou!\n");
    pthread_exit(NULL);
}

// funcao executada pela thread 2
void *t2(void *arg)
{
    buffer2 = (char *)malloc(sizeof(char) * N * N);
    int i = 0, j = 1;
    int quebra = 1;
    printf("Thread : 2 esta executando...\n");

    // espera a transicao para o estado 1
    sem_wait(&estado1);
    printf("Thread : 2 mudou estado!\n");
    while (1)
    {
        if (quebra == 0 && j < 10)
        {
            quebra = (2 * j) + 1;
            j++;
            buffer2[i] = '\n';
            i++;
        }
        else if (j >= 10 && quebra == 0)
        {
            quebra = 10;
            buffer2[i] = '\n';
            i++;
        }

        else
        {
            buffer2[i + j] = buffer[i];
            i++;
            quebra--;
        }
        if (buffer2[i]== EOF)
        {
            printf("oi");
            break;
        }
    }
    sem_post(&estado2);

    printf("Thread : 2 terminou!\n");
    pthread_exit(NULL);
}

// funcao executada pela thread 3
void *t3(void *arg)
{
    printf("Thread : 3 esta executando...\n");
    int i = 0;
    // espera a transicao para o estado 2
    sem_wait(&estado2);
    printf("Thread : 3 mudou estado!\n");
    while (1)
    {
        if (i == N - 1 + j)
        {
            break;
        }
        printf("%c", buffer2[i]);
        i++;
    }

    printf("Thread : 3 terminou!\n");
    pthread_exit(NULL);
}

// funcao principal
int main(int argc, char *argv[])
{
    pthread_t tid[NTHREADS];
    if (argc < 2)
    {
        printf("qual arquivo de entrada?");
        return 1;
    }
    descritorArquivo = fopen(argv[1], "r");
    // inicia os semaforos
    sem_init(&estado1, 0, 0);
    sem_init(&estado2, 0, 0);

    // cria as tres threads
    if (pthread_create(&tid[2], NULL, t3, NULL))
    {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }
    if (pthread_create(&tid[1], NULL, t2, NULL))
    {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }
    if (pthread_create(&tid[0], NULL, t1, NULL))
    {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }

    //--espera todas as threads terminarem
    for (int t = 0; t < NTHREADS; t++)
    {
        if (pthread_join(tid[t], NULL))
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    return 0;
}

