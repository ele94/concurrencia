#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/shm.h>


#define MSG_EXCEPT 020000

struct permisoSC{
  long tipo;
};

int main(int argc, char *argv[]){

	int id_cola_aux[1];
	int *id_cola, *numLec;
	int idqueue, id_nodo, shmid;
	key_t key;
	sem_t *sem;
	char *path = "/tmp";
        struct permisoSC permisoSC;

	
        if(argc!=2){
		printf("La llamada correcta al proceso es: %s i\n",argv[0]);
        	exit(-1);
        }

	id_cola = id_cola_aux;

	id_nodo = atoi(argv[1]);

        key = ftok(path,15+id_nodo);
        id_cola[0] = msgget (key, 0600 | IPC_CREAT);
	idqueue=id_cola[0];


	//CLAVE 25 PARA numLec
        key = ftok(path,25+id_nodo);

        shmid=shmget(key,sizeof(int),IPC_CREAT | 0666);
	if(shmid<0){
        	  perror("shmget");
        	  exit(1);
  	}

	numLec=(int *)shmat(shmid, NULL, 0);
	if(*numLec == -1) {
        	perror("shmat");
        	exit(1);
  	}

	//SEMAFORO MUTEX PARA NUMLEC
        key = ftok(path,45+id_nodo);

        shmid=shmget(key,sizeof(sem_t),IPC_CREAT | 0666);

        if(shmid<0){
                perror("shmget");
        	exit(1);
        }

        sem=(sem_t *)shmat(shmid, NULL, 0);
        if(sem == (sem_t *)-1) {
                perror("shmat");
                exit(1);
        }

  do{
	printf("Lector en el nodo %i sin hacer nada\n",id_nodo);
        getchar();
        printf("Lector en la cola del nodo %i\n", id_nodo);
	msgrcv(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),(long)3,0 | MSG_EXCEPT);
	if(permisoSC.tipo == 1)
		printf("Algoritmo internodo\n");
	
	sem_wait(sem);
        numLec[0]++;
	sem_post(sem);
	
	permisoSC.tipo=2;
	msgsnd(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),0);
        printf("Hay %i lectores en la cola\n" ,numLec[0]);


	printf("Lector leyendo en el nodo %i\n", id_nodo);
	getchar();



    msgrcv(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),(long)2,0);
	
	sem_wait(sem);
        numLec[0]--;
	sem_post(sem);
        
	printf("Hay %i lectores en la cola\n", numLec[0]);
	
	sem_wait(sem);
	if(numLec[0]){
		permisoSC.tipo=2;
                msgsnd(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),0);
        }
        else{
		printf("Algoritmo internodo\n");
		permisoSC.tipo=1;
                msgsnd(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),0);
        }
	
	sem_post(sem);
	
  }while(1);

}
