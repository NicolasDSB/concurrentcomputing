/* Programa que le um arquivo binario com dois valores inteiros (indicando as dimensoes de uma matriz)
 * e uma sequencia com os valores das matrizes (em float)
 * Entrada: nome do arquivo de entrada, de saída e threads
 * Saida: valores da matriz escritos no formato binario no arquivo e tempo de execução na saída padrão
 * */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

float *matriz1, *matriz2, *matrizResult; // matriz que será carregada do arquivo
int linhas, colunas;                     // dimensoes da matriz
long long int tam;                       // qtde de elementos na matriz
int nthreads;                            // numero de threads

void *multMatriz(void *tid)
{
    long int id = (long int)tid; // identificador da thread
    int ini, fim, bloco, aux;         // auxiliares para divisao da matriz em blocos
    bloco = linhas / nthreads;   // tamanho do bloco de dados de cada thread
    ini = id * bloco;            // posicao inicial do vetor
    fim = ini + bloco;           // posicao final do vetor
    if (id == (nthreads - 1))
    {
        fim = linhas; // a ultima thread trata os elementos restantes no caso de divisao nao exata
    }
    for (int i = ini; i < fim; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            for (int k = 0; k < colunas; k++)
            {
                aux += matriz1[i * colunas + k] * matriz2[colunas * k + j];
            }
            matrizResult[i * colunas + j] = aux;
            aux = 0;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    FILE *descritorArquivo;        // descritor do arquivo de entrada
    size_t ret;                    // retorno da funcao de leitura no arquivo de entrada
    double start, finish, elapsed; // cronometrar execução
    pthread_t *tid_sistema;        // vetor de identificadores das threads no sistema
    // recebe os argumentos de entrada
    if (argc < 4)
    {
        fprintf(stderr, "Digite: %s <arquivo entrada> <arquivo saida> <numero de threads>\n", argv[0]);
        return 1;
    }
    // abre o arquivo para leitura binaria
    descritorArquivo = fopen(argv[1], "rb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    // le as dimensoes da matriz
    ret = fread(&linhas, sizeof(int), 1, descritorArquivo);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }
    ret = fread(&colunas, sizeof(int), 1, descritorArquivo);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }
    tam = linhas * colunas; // calcula a qtde de elementos da matriz
    // aloca memoria para a matriz
    matriz1 = (float *)malloc(sizeof(float) * tam);
    matriz2 = (float *)malloc(sizeof(float) * tam);
    matrizResult = (float *)malloc(sizeof(float) * tam);
    if (!matriz1 || !matriz2)
    {
        fprintf(stderr, "Erro de alocao da memoria da matriz\n");
        return 3;
    }

    // carrega a matriz de elementos do tipo float do arquivo
    ret = fread(matriz1, sizeof(float), tam, descritorArquivo);
    ret = fread(matriz2, sizeof(float), tam, descritorArquivo);
    if (ret < tam)
    {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return 4;
    }
    // pega o numero de threads
    nthreads = atoi(argv[3]);
    // limita o numero de threads ao tamanho das linhas
    if (nthreads > linhas)
        nthreads = linhas;
    // aloca espaco para o vetor de identificadores das threads no sistema
    tid_sistema = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid_sistema == NULL)
    {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
    GET_TIME(start);
    // cria as threads
    for (long int i = 0; i < nthreads; i++)
    {
        if (pthread_create(&tid_sistema[i], NULL, multMatriz, (void *)i))
        {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }
    // espera todas as threads terminarem
    // retorno = (float*) malloc(sizeof(float));
    for (int i = 0; i < nthreads; i++)
    {
        if (pthread_join(tid_sistema[i], NULL))
        {
            printf("--ERRO: pthread_join()\n");
            exit(-1);
        }
    }
    GET_TIME(finish);
    elapsed = finish - start;
    printf("Levou %e segundos de execucao para multiplicar sequencialmente\n", elapsed);
    descritorArquivo = fopen(argv[2], "wb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 5;
    }
    ret = fwrite(&linhas, sizeof(int), 1, descritorArquivo);
    ret = fwrite(&colunas, sizeof(int), 1, descritorArquivo);
    ret = fwrite(matrizResult, sizeof(float), tam, descritorArquivo);
    if (ret < tam)
    {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 6;
    }
    // finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(matriz1);
    free(matriz2);
    free(matrizResult);
    return 0;
}
