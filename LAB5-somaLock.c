/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long int soma = 0;             // variavel compartilhada entre as threads
pthread_mutex_t mutex, mutex2; // variavel de lock para exclusao mutua
int nthreads, printar;         // qtde de threads (passada linha de comando)
pthread_cond_t cond;           // variavel para wait signal broadcast

// funcao executada pelas threads
void *ExecutaTarefa(void *arg)
{
  long int id = (long int)arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i = 0; i < 100000; i++)
  {
    //--entrada na SC
    pthread_mutex_lock(&mutex);
    //--SC (seção critica)
    if (!(soma % 10))
    {
      pthread_mutex_lock(&mutex2);
      printar = 1;
      pthread_cond_wait(&cond, &mutex2);
      pthread_mutex_unlock(&mutex2);
    }
    soma++; // incrementa a variavel compartilhada
    //--saida da SC
    pthread_mutex_unlock(&mutex);
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

// funcao executada pela thread de log
void *extra(void *args)
{
  printf("Extra : esta executando...\n");
  while (1)
  {
    pthread_mutex_lock(&mutex2);
    if (printar == 1)
    {
      printf("soma = %ld \n", soma);
      printar = 0;
      pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex2);
    if (soma == 100000 * nthreads)
    {
      break;
    }
  }
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

// fluxo principal
int main(int argc, char *argv[])
{
  pthread_t *tid; // identificadores das threads no sistema

  //--le e avalia os parametros de entrada
  if (argc < 2)
  {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);
  //--aloca as estruturas
  tid = (pthread_t *)malloc(sizeof(pthread_t) * (nthreads + 1));
  if (tid == NULL)
  {
    puts("ERRO--malloc");
    return 2;
  }

  //--inicilaiza o mutex (lock de exclusao mutua) e cond
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&mutex2, NULL);
  pthread_cond_init(&cond, NULL);

  //--cria as threads
  for (long int t = 0; t < nthreads; t++)
  {
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t))
    {
      printf("--ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  //--cria thread de log
  if (pthread_create(&tid[nthreads], NULL, extra, NULL))
  {
    printf("--ERRO: pthread_create()\n");
    exit(-1);
  }

  //--espera todas as threads terminarem
  for (int t = 0; t < nthreads + 1; t++)
  {
    if (pthread_join(tid[t], NULL))
    {
      printf("--ERRO: pthread_join() \n");
      exit(-1);
    }
  }

  //--finaliza o mutex

  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&mutex2);
  pthread_cond_destroy(&cond);
  printf("Valor de 'soma' = %ld\n", soma);

  return 0;
}
