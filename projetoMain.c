#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>

#define N 5 
// Quantidade de filósofos
#define COMENDO 2
#define FOME 1
#define PENSANDO 0
#define ESQUERDA (filosofoID + 4) % N 
// Pegar garfo da esquerda
#define DIREITA (filosofoID + 1) % N  
// Pegar garfo da direita

sem_t semaforo1; // Evita que dois processos ou threads acessem o mesmo recurso compartilhado
// Quando solicitar acesso a uma informação, qualquer outra deverá aguardar. 
sem_t semaforo2[N]; 
int estado[N]; // Estado dos filosofos (comendo, pensando)
int filosofoID[N] = {0,1,2,3,4}; // Identificação dos filósofos

void *filosofo(void *num); // Representacao do filosofo
void pegaGarfo(int);  // Função de pegar o garfo
void devolveGarfo(int);  // Função de devolver o garfo
void verificacao(int);  // Função de verificacao se o filosofo pode comer

int main() {  // Main
  int i;
  pthread_t thread_id[N]; // Identificadores das threads, filosofo

  sem_init(&semaforo1, 0, 1);// Inicia o semaforo e com o parametro "shared" esta como 0, o semaforo sera usado somente em threads do processo atual
  for (i=0; i<N; i++)
    sem_init(&semaforo2[i], 0, 0); // Inicia o semaforo
  for (i=0; i<N; i++) {
    pthread_create(&thread_id[i], NULL, filosofo, &filosofoID[i]);
    //criar as threads
    printf("O %dº Filósofo esta pensando...\n", i + 1);
  } 
  for (i=0; i<N; i++)
    pthread_join(thread_id[i], NULL); // Faz a junção das threads
  return 0;
}

void *filosofo(void *num) { // Executando as ações de vida do filósfo
  while(1) {  // Roda ate o programa ser interrompido 
    int *i = num; // Identificação do filósofo
    sleep(1); // Espera 1 segundo para ser executado
    pegaGarfo(*i);
    sleep(1);  // Espera 1 segundo para ser executado
    devolveGarfo(*i);
  }
}

void devolveGarfo(int filosofoID) { // Função de colocar o garfo na mesa
  sem_wait(&semaforo1); // Entra na sessão crítica, os garfos. Bloqueia a thread
  estado[filosofoID] = PENSANDO; // O estado do filosofo passa a ser pensando
  printf("O %dº Filósofo comeu e devolveu os garfos que usou para comer...\n", filosofoID + 1);
  printf("O %dº Filósofo está pensando...\n", filosofoID + 1);
  verificacao(ESQUERDA); // Executa a funcao de verificacao com o filosofo da esquerda
  verificacao(DIREITA); // Executa a funcao de verificacao com o filosofo da direita
  sem_post(&semaforo1); // Sai da sessão crítica. Desbloqueia o semaforo
}

void pegaGarfo(int filosofoID) { // Função de pegar o garfo
  sem_wait(&semaforo1); // Entra na sessão crítica, os garfos. Bloqueia a thread
  estado[filosofoID] = FOME; // Altera o estado do filosofo
  printf("O %dº Filósofo está com fome...\n", filosofoID + 1);
  verificacao(filosofoID);
  sem_post(&semaforo1); // Sai da sessão crítica. Desbloqueia o semaforo
  sem_wait(&semaforo2[filosofoID]); // Bloqueia o semaforo, sendo desbloqueado somente na etapa de verificacao, após o filosofo comer
  sleep(1);  // Espera 1 segundo para ser executado
}

void verificacao(int filosofoID) {  // Verificacao 
  if (estado[filosofoID] == FOME     // Entra na condicao caso o filosofo sinta fome 
    && estado[ESQUERDA] != COMENDO  // e o filósofo a sua esquerda  
    && estado[DIREITA] != COMENDO)   // e o filosofo a sua direita nao estejam comendo 
  { 
    estado[filosofoID] = COMENDO; // Caso entre altera o estado do filosofo para Comendo
    sleep(2);  // Espera 2 segundos para ser executado
    printf("O %dº Filósofo pegou os garfos %d e %d \n", filosofoID + 1, ESQUERDA + 1, filosofoID + 1);
    printf("O %dº Filósofo estava com fome e agora está comendo...\n", filosofoID + 1); 
    sem_post(&semaforo2[filosofoID]); // Desbloqueia o semaforo iniciado na funcao pegaGarfo
  }
}

// É resolvido o problema da comunicação inter processos (IPC), com a implementação de semáforos, os quais garantem que cada garfo seja pego por um filosofo de cada vez, impedindo que ocorra deadlock e starvation. Por meio do sem_wait e do sem_post, os quais bloqueiam e desbloqueiam o semaforo (green light e red light)