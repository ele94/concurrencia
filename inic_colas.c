#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/shm.h>


#define INTERNODOS 10
#define INTRANODOS 5
#define TOKEN 100

#define SEM_PETLEC "/peticionesLectores"
#define SEM_PETESC "/peticionesEscritores"
#define SEM_SERVLEC "/servidosLectores"
#define SEM_SERVESC "/servidosEscritores"
#define SEM_NUMNODLEC "/numNodosLectores"

struct permisoSC{
  long tipo;
};

struct token{
  long tipo;
  int servidosEscritores[5];
  int servidosLectores[5];
  int numNodLec;
};

int main (int argc,int *argv[]){

  printf("Linea 25\n");

  key_t key, key0, key1, key2, key3;
  int id_cola,i,j;
  int shmid, shmid0, shmid1, shmid2, shmid3;
  int *shm0 = NULL;
  int *shm1 = NULL;
  sem_t *sem2, *sem3;
  char *path = "/tmp";
  int id = 0;


  struct permisoSC permisoSC;

    printf("Linea 50\n");


  permisoSC.tipo = 1;

   int id_nodo;
  for(i=0;i<INTRANODOS;i++){
   id_nodo = i+1;
  	id = 15+i;
    key = ftok(path,id);
    printf("key %d\n",key);
  	id_cola = msgget (key, 0666| IPC_CREAT);
  	msgctl(id_cola, IPC_RMID, NULL);
  	id_cola = msgget (key, 0666| IPC_CREAT);
  	if (id_cola == -1){
  		printf("Error al generar la cola.\n");
                exit (-1);
        }

	msgsnd(id_cola,(struct msgbuf *)&permisoSC,sizeof(permisoSC),0);

  printf("Linea 100\n");

//	CLAVE 30[12345] PARA numLec

	id=25+i;
  key0=ftok(path,id);
  printf("key0 %d\n",key0);
	shmid0=shmget(key0,sizeof(int),0666| IPC_CREAT);
  printf("shmid0 %d\n",shmid0);
	if(shmid0<0){
	        perror("shmget");
	        exit(1);
  	}
      printf("Linea 110\n");


	shm0=(int *)shmat(shmid0, NULL, 0);
	if(shm0 == (int *)-1) {
	        perror("shmat");
 		exit(1);
  	}
  printf("Linea 118\n");
	shm0[0] = 0;
  printf("Linea 120\n");

//	CLAVE 40[12345] PARA escritoresEnCola 

	id=35 + i;
  key1=ftok(path,id);
  printf("key1 %d\n",key1);
	shmid1=shmget(key1,sizeof(int),0666| IPC_CREAT);
  printf("shm1d1 %d\n",shmid1);


  printf("Linea 130\n");

	if(shmid1<0){
	        perror("shmget");
		exit(1);
  	}
  	shm1=(int *)shmat(shmid1, NULL, 0);
  	if(shm1 == (int *)-1) {
  		perror("shmat");
  		exit(1);
  	}
      printf("Linea 142\n");

  	
	shm1[0] = 0;

//	SEMAFORO MUTEX NUMLEC
        id=45 + i;
        key2=ftok(path,id);
        printf("key2 %d\n",key2);
        shmid2=shmget(key2,sizeof(sem_t),0666| IPC_CREAT);
        printf("shmid2 %d\n",shmid2);

  printf("Linea 153\n");

        if(shmid2<0){
                perror("shmget");
	        exit(1);
        }

  printf("Linea 160\n");

         sem2=(sem_t *)shmat(shmid2, NULL, 0);
         if(sem2 == (sem_t *)-1) {
	        perror("shmat");
	        exit(1);
      	}
  printf("Linea 167\n");

        sem_init(sem2,1,1);	
  printf("Linea 170\n");

//	SEMAFORO MUTEX ESCRITORESENCOLA
  
        id= 55 + i;
        key3=ftok(path,id);
        printf("key3 %d\n",key3);
        shmid3=shmget(key3,sizeof(sem_t), 0666| IPC_CREAT);
        printf("shmid3 %d \n",shmid3);

        if(shmid3<0){
                perror("shmget");
	        exit(1);
        }

        sem3=(sem_t *)shmat(shmid3, NULL, 0);
        if(sem3 == (sem_t *)-1) {
	        perror("shmat");
                exit(1);
        }

        sem_init(sem3,1,1);

      


  }	
  printf("Linea 364\n");

  sem_destroy(sem2);
  sem_destroy(sem3);
 
 
  return 0;

}
