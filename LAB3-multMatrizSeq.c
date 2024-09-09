/* Programa que le um arquivo binario com dois valores inteiros (indicando as dimensoes de uma matriz)
 * e uma sequencia com os valores das matrizes (em float)
 * Entrada: nome do arquivo de entrada e de saída 
 * Saida: valores da matriz escritos no formato binario no arquivo e tempo de execução na saída padrão
 * */

#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

int main(int argc, char *argv[])
{
   float *matriz1, *matriz2, *matrizResult; // matriz que será carregada do arquivo
   int linhas, colunas, aux;                // dimensoes da matriz
   long long int tam;                       // qtde de elementos na matriz
   FILE *descritorArquivo;                  // descritor do arquivo de entrada
   size_t ret;                              // retorno da funcao de leitura no arquivo de entrada
   double start, finish, elapsed;           // cronometrar execução

   // recebe os argumentos de entrada
   if (argc < 3)
   {
      fprintf(stderr, "Digite: %s <arquivo entrada> <arquivo saida>\n", argv[0]);
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
   GET_TIME(start);
   for (int i = 0; i < linhas; i++)
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

