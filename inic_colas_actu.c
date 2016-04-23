#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define INTERNODOS 10
#define INTRANODOS 5
#define TOKEN 100

struct permisoSC{
  long tipo;
};

struct token{
  long tipo;
  int servidosEscritores[5];
  int servidosLectores[5];
  int numNodLec;
};

int main (int argc,char *argv[]){
  key_t key, key0, key1, key2, key3;
  int id_cola,i,j;
  int shmid, shmid0, shmid1, shmid2, shmid3;
  int *shm0 = NULL;
  int *shm1 = NULL;
  sem_t *sem2, *sem3;

  //semaforos internodo
  //semaforos mutex
  sem_t * sem_peticionesLectores;
  sem_t * sem_peticionesEscritores;
  sem_t * sem_servidosLectores;
  sem_t * sem_servidosEscritores;
  sem_t * sem_numNodLec;
  sem_t * sem_hasToken;
  sem_t * sem_inSC;
  sem_t * sem_lectorOEscritor;
  sem_t * sem_esperandoAviso;
  sem_t * sem_receive;

  struct token testigo;
  struct permisoSC permisoSC;

  key = TOKEN;
  id_cola = msgget (key, 0777 | IPC_CREAT);
  msgctl(id_cola, IPC_RMID, NULL);
  id_cola = msgget (key, 0777 | IPC_CREAT);
  if (id_cola == -1){
    printf("Error al generar la cola.\n");
    exit (-1);
  }

  testigo.tipo=1;
  for(i=0;i<5;i++){
    	testigo.servidosLectores[i] = 0;
    	testigo.servidosEscritores[i] = 0;
    }
  testigo.numNodLec = 0;

  msgsnd(id_cola,(struct msgbuf *)&testigo,sizeof(testigo),0);

  for(i=1;i<5;i++){
	for(j=i+1;j<6;j++){
		key = INTERNODOS*i + j;
  		id_cola = msgget (key, 0777 | IPC_CREAT);
  		msgctl(id_cola, IPC_RMID, NULL);
  		id_cola = msgget (key, 0777 | IPC_CREAT);
  		if (id_cola == -1){
  			printf("Error al generar la cola.\n");
  			exit (-1);
  		}
  	}
  }

  permisoSC.tipo = 1;

   int id_nodo;
  for(i=0;i<INTRANODOS;i++){
   id_nodo = i+1;
  	key = 201+i;
  	id_cola = msgget (key, 0777 | IPC_CREAT);
  	msgctl(id_cola, IPC_RMID, NULL);
  	id_cola = msgget (key, 0777 | IPC_CREAT);
  	if (id_cola == -1){
  		printf("Error al generar la cola.\n");
                exit (-1);
        }

	msgsnd(id_cola,(struct msgbuf *)&permisoSC,sizeof(permisoSC),0);

//	CLAVE 30[12345] PARA numLec

	key0=300+i;

	shmid0=shmget(key0,sizeof(int),IPC_CREAT | 0777);
	if(shmid0<0){
	        perror("shmget");
	        exit(1);
  	}

	shm0=(int *)shmat(shmid0, NULL, 0);
	if(shm0 == (int *)-1) {
	        perror("shmat");
 		exit(1);
  	}
  	
	shm0[0] = 0;

//	CLAVE 40[12345] PARA escritoresEnCola 

	key1=400 + i;

	shmid1=shmget(key1,sizeof(int),IPC_CREAT | 0777);

	if(shmid1<0){
	        perror("shmget");
		exit(1);
  	}

  	shm1=(int *)shmat(shmid1, NULL, 0);
  	if(shm1 == (int *)-1) {
  		perror("shmat");
  		exit(1);
  	}
  	
	shm1[0] = 0;

//	SEMAFORO MUTEX NUMLEC

        key2=500 + i;

        shmid2=shmget(key2,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid2<0){
                perror("shmget");
	        exit(1);
        }

         sem2=(sem_t *)shmat(shmid2, NULL, 0);
         if(sem2 == (sem_t *)-1) {
	        perror("shmat");
	        exit(1);
      	}

        sem_init(sem2,1,1);	

//	SEMAFORO MUTEX ESCRITORESENCOLA
  
        key3=600 + i;

        shmid3=shmget(key3,sizeof(sem_t),IPC_CREAT | 0777);

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

        //MEMORIA COMPARTIDA Y SEMS INTRANODO
        //PETICIONES LECTORES
        key = 100000 + 1000 * id_nodo;

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid<0){
                perror("shmget");
           exit(1);
        }

        sem_peticionesLectores =(sem_t *)shmat(shmid, NULL, 0);
        if(sem_peticionesLectores == (sem_t *)-1) {
           perror("shmat");
                exit(1);
        }

        sem_init(sem_peticionesLectores,1,1);

        //PETICIONES ESCRITORES
        key = 100000 + 1050 * id_nodo;

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid<0){
                perror("shmget");
           exit(1);
        }

        sem_peticionesEscritores =(sem_t *)shmat(shmid, NULL, 0);
        if(sem_peticionesEscritores == (sem_t *)-1) {
           perror("shmat");
                exit(1);
        }

        sem_init(sem_peticionesEscritores,1,1);


        //SERVIDOS LECTORES
        key = 100000 + 1100 * id_nodo;

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid<0){
                perror("shmget");
           exit(1);
        }

        sem_servidosLectores =(sem_t *)shmat(shmid, NULL, 0);
        if(sem_servidosLectores == (sem_t *)-1) {
           perror("shmat");
                exit(1);
        }

        sem_init(sem_servidosLectores,1,1);


        //SERVIDOS ESCRITORES
        key = 100000 + 1150 * id_nodo;

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid<0){
                perror("shmget");
           exit(1);
        }

        sem_servidosEscritores =(sem_t *)shmat(shmid, NULL, 0);
        if(sem_servidosEscritores == (sem_t *)-1) {
           perror("shmat");
                exit(1);
        }

        sem_init(sem_servidosEscritores,1,1);


        //NUM NOD LEC
        key = 10000 + id_nodo;

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid<0){
                perror("shmget");
           exit(1);
        }

        sem_numNodLec =(sem_t *)shmat(shmid, NULL, 0);
        if(sem_numNodLec == (sem_t *)-1) {
           perror("shmat");
                exit(1);
        }

        sem_init(sem_numNodLec,1,1);

        //HAS TOKEN

        key = 11000 + id_nodo;

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid<0){
                perror("shmget");
           exit(1);
        }

        sem_hasToken =(sem_t *)shmat(shmid, NULL, 0);
        if(sem_hasToken == (sem_t *)-1) {
           perror("shmat");
                exit(1);
        }

        sem_init(sem_hasToken,1,1);


        //en SC

        key = 13000 + id_nodo;

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid<0){
                perror("shmget");
           exit(1);
        }

        sem_inSC =(sem_t *)shmat(shmid, NULL, 0);
        if(sem_inSC == (sem_t *)-1) {
           perror("shmat");
                exit(1);
        }

        sem_init(sem_inSC,1,1);


        //lector o escritor

        key = 14000 + id_nodo;

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid<0){
                perror("shmget");
           exit(1);
        }

        sem_lectorOEscritor =(sem_t *)shmat(shmid, NULL, 0);
        if(sem_lectorOEscritor == (sem_t *)-1) {
           perror("shmat");
                exit(1);
        }

        sem_init(sem_lectorOEscritor,1,1);


        //Esperando Aviso

        key = 15000 + id_nodo;

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0777);

        if(shmid<0){
                perror("shmget");
           exit(1);
        }

        sem_peticionesLectores =(sem_t *)shmat(shmid, NULL, 0);
        if(sem_peticionesLectores == (sem_t *)-1) {
           perror("shmat");
                exit(1);
        }

        sem_init(sem_peticionesLectores,1,1);




  }	

  sem_destroy(sem2);
  sem_destroy(sem3);
  sem_destroy(sem_peticionesLectores);
  sem_destroy(sem_peticionesEscritores);
  sem_destroy(sem_servidosLectores);
  sem_destroy(sem_servidosEscritores);
  sem_destroy(sem_lectorOEscritor);
  sem_destroy(sem_inSC);
  sem_destroy(sem_hasToken);
  sem_destroy(sem_numNodLec);
 
  return 0;

}
