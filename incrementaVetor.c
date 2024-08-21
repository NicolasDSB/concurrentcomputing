/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 1 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
   int idThread, mThreads, comeco, fim, *vetor, salto;
} t_Args;
//funcao executada pelas threads
void *PrintHello (void *arg) {
  t_Args *args = (t_Args *) arg;
  for(int i=args->comeco; i<args->fim; i++){
    args->vetor[i]++;
  }
  free(arg); //libera a alocacao feita na main
  pthread_exit(NULL);
}
//funcao principal do programa
int main(int argc, char* argv[]) {
  t_Args *args; //receberá os argumentos para a thread
  int d = 0;
  int mthreads; //qtde de threads que serao criadas (recebida na linha de comando)
  //verifica se o argumento 'qtde de threads' foi passado e armazena seu valor
  if(argc<3) {
      printf("--ERRO: informe a qtde de threads <%s> <nthreads> e o tamanho do vetor\n", argv[0]);
      return 1;
  }
  mthreads = atoi(argv[1]);
  int n = atoi(argv[2]);
  int vetor[n];
  int salto = n/mthreads;
  // fiz com um vetor zerado apenas para ser mais fácil testar a saída
  for(int i=0; i<n; i++){
    vetor[i]=0;
  }
  //identificadores das threads no sistema
  pthread_t tid_sistema[mthreads];

  //cria as threads
  for(int i=1; i<=mthreads; i++) {
    args = malloc(sizeof(t_Args));
    if (args == NULL) {
      printf("--ERRO: malloc()\n"); 
      return 1;
    }
    args->idThread = i; 
    args->mThreads = mthreads;
    args->vetor = vetor;
    args->comeco = (i-1) * salto;
    //a última thread fica com o resto se n/mthreads tiver resto
    args->fim = ((i)==(mthreads)) ? n : i*salto;
    if (pthread_create(&tid_sistema[i-1], NULL, PrintHello, (void*) args)) {
      printf("--ERRO: pthread_create()\n"); 
      return 2;
    }
  }

  //espera todas as threads terminarem
  for (int i=0; i<mthreads; i++) {
    if (pthread_join(tid_sistema[i], NULL)) {
         printf("--ERRO: pthread_join() da thread %d\n", i); 
    } 
  }
  for (int i=0; i<n; i++) {
    if(!vetor[i]==1){
        d = 1;
        break;
    }
  }
  (d == 1) ? printf("deu ruim") : printf("funcionou");
  //pthread_exit(NULL); //nao é necessario nesse caso, por que?
}
