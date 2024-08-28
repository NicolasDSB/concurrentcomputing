#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// variaveis globais
// tamanho do vetor
long int n;
// vetor de elementos
float *vetorA;
float *vetorB;
// numero de threads
int nthreads;

// funcao executada pelas threads
// estrategia de divisao de tarefas: blocos de n/nthreads elementos
void *PInternoFunc(void *tid)
{
    long int id = (long int)tid; // identificador da thread
    int ini, fim, bloco;         // auxiliares para divisao do vetor em blocos
    float pInterno_local, *ret;
    bloco = n / nthreads; // tamanho do bloco de dados de cada thread
    ini = id * bloco;     // posicao inicial do vetor
    fim = ini + bloco;    // posicao final do vetor
    if (id == (nthreads - 1))
        fim = n; // a ultima thread trata os elementos restantes no caso de divisao nao exata
    for (int i = ini; i < fim; i++)
    {
        pInterno_local += vetorA[i] * vetorB[i];
    }

    // retorna o resultado
    ret = malloc(sizeof(float));
    if (ret != NULL)
        *ret = pInterno_local;
    else
        printf("--ERRO: malloc() thread\n");
    pthread_exit((void *)ret);
}

int main(int argc, char *argv[])
{
    FILE *arq;                       // arquivo de entrada
    size_t ret;                      // retorno da funcao de leitura no arquivo de entrada
    double pInterno_ori;             // produto interno registrado no arquivo
    float pInterno_par_global;       // resultado
    float pInterno1, pInterno2;      // auxiliares para o produto interno sequencial alternada
    float *pInterno_retorno_threads; // auxiliar para retorno das threads

    pthread_t *tid_sistema; // vetor de identificadores das threads no sistema

    if (argc < 3)
    {
        printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]);
        exit(-1);
    }

    // abre o arquivo de entrada com os valores para serem usados
    arq = fopen(argv[1], "rb");
    if (arq == NULL)
    {
        printf("--ERRO: fopen()\n");
        exit(-1);
    }
    // le o tamanho do vetor (primeira linha do arquivo)
    ret = fread(&n, sizeof(long int), 1, arq);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 2;
    }
    // aloca espaco de memoria e carrega o vetor de entrada
    vetorA = malloc(sizeof(float) * n);
    vetorB = malloc(sizeof(float) * n);
    if (!vetorA || !vetorB)
    {
        fprintf(stderr, "Erro de alocaçao da memoria do vetor\n");
        return 3;
    }
    ret = fread(vetorA, sizeof(float), n, arq);
    if (ret < n)
    {
        fprintf(stderr, "Erro de leitura dos elementos do vetor\n");
        return 4;
    }
    ret = fread(vetorB, sizeof(float), n, arq);
    if (ret < n)
    {
        fprintf(stderr, "Erro de leitura dos elementos do vetor\n");
        return 5;
    }
    // le o numero de threads da entrada do usuario
    nthreads = atoi(argv[2]);
    // limita o numero de threads ao tamanho do vetor
    if (nthreads > n)
        nthreads = n;

    // aloca espaco para o vetor de identificadores das threads no sistema
    tid_sistema = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid_sistema == NULL)
    {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }

    // cria as threads
    for (long int i = 0; i < nthreads; i++)
    {
        if (pthread_create(&tid_sistema[i], NULL, PInternoFunc, (void *)i))
        {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }
    // espera todas as threads terminarem
    // retorno = (float*) malloc(sizeof(float));
    for (int i = 0; i < nthreads; i++)
    {
        if (pthread_join(tid_sistema[i], (void *)&pInterno_retorno_threads))
        {
            printf("--ERRO: pthread_join()\n");
            exit(-1);
        }
        pInterno_par_global += *pInterno_retorno_threads;
        free(pInterno_retorno_threads);
    }

    // imprime o resultado
    printf("\n");
    printf("pInterno_concorrente = %.26f\n", pInterno_par_global);

    // le o somatorio registrado no arquivo
    ret = fread(&pInterno_ori, sizeof(double), 1, arq);
    printf("pInterno_ori = %.26lf\n", pInterno_ori);
    printf("erro: %f", (pInterno_ori/pInterno_par_global)/pInterno_ori);
    // desaloca os espacos de memoria
    free(vetorA);
    free(vetorB);
    free(tid_sistema);
    // fecha o arquivo
    fclose(arq);
    return 0;
}
