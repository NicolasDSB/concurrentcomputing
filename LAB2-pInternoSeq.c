#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MAX 1000 // valor maximo de um elemento do vetor
// descomentar o define abaixo caso deseje imprimir uma versao do vetor gerado no formato texto
#define TEXTO
int main(int argc, char *argv[])
{
    float *vetorA; // vetorque será gerada
    float *vetorB;
    long int n;
    float elem;             // valor gerado para incluir no vetor
    double pInterno = 0;    // soma total dos elementos gerados
    int fator = 1;          // fator multiplicador para gerar números negativos
    FILE *descritorArquivo; // descritor do arquivo de saida
    size_t ret;             // retorno da funcao de escrita no arquivo de saida
    if (argc < 3)
    {
        fprintf(stderr, "Digite: %s <dimensao> <arquivo saida>\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);

    // aloca memoria para o vetor
    vetorA = (float *)malloc(sizeof(float) * n);
    vetorB = (float *)malloc(sizeof(float) * n);
    if (!vetorA || !vetorB)
    {
        fprintf(stderr, "Erro de alocao da memoria do vetor\n");
        return 2;
    }

    // preenche o vetor com valores float aleatorios
    srand(time(NULL));
    for (long int i = 0; i < n; i++)
    {
        elem = (rand() % MAX) / 3.0 * fator;
        vetorA[i] = elem;
        elem = (rand() % MAX) / 3.0 * fator;
        vetorB[i] = elem;
        fator *= -1;
        pInterno += vetorA[i] * vetorB[i];
    }
// imprimir na saida padrao o vetor gerado
#ifdef TEXTO
    fprintf(stdout, "%ld\n", n);
    for (long int i = 0; i < n; i++)
    {
        fprintf(stdout, "%f ", vetorA[i]);
    }
    fprintf(stdout, "\n");
    for (long int i = 0; i < n; i++)
    {
        fprintf(stdout, "%f ", vetorB[i]);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "produto interno=%lf\n", pInterno);
#endif

    // escreve o vetor no arquivo
    // abre o arquivo para escrita binaria
    descritorArquivo = fopen(argv[2], "wb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }
    // escreve a dimensao
    ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);
    // escreve os elementos do vetor
    ret = fwrite(vetorA, sizeof(float), n, descritorArquivo);
    if (ret < n)
    {
        fprintf(stderr, "Erro de escrita no arquivo\n");
        return 4;
    }
    ret = fwrite(vetorB, sizeof(float), n, descritorArquivo);
    if (ret < n)
    {
        fprintf(stderr, "Erro de escrita no arquivo\n");
        return 4;
    }
    // escreve o somatorio
    ret = fwrite(&pInterno, sizeof(double), 1, descritorArquivo);

    // finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(vetorA);
    free(vetorB);
    return 0;
}