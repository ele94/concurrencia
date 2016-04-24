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
	int *id_cola, *escritoresEnCola;
	int idqueue, id_nodo, shmid;
	key_t key;
	sem_t *sem;
	char *path = "/tmp";
      	struct permisoSC permisoSC;
	
	
	if(argc!=2){
	    printf("La llamada correcta al proceso es: %s i\n",argv[0]);
	    exit(-1);
	}

	id_cola=id_cola_aux;
	id_nodo = atoi(argv[1]);

        key = ftok(path,15+id_nodo);
	id_cola[0] = msgget (key, 0600 | IPC_CREAT);
	idqueue=id_cola[0];

//	CLAVE 301 PARA escritoresEnCola
        key = ftok(path,35+id_nodo);

        shmid=shmget(key,sizeof(int),IPC_CREAT | 0666);
  	if(shmid<0){
  	      perror("shmget");
 	       exit(1);
  	}

  	escritoresEnCola=(int *)shmat(shmid, NULL, 0);
  	if(*escritoresEnCola == -1) {
  	      perror("shmat");
  	      exit(1);
  	}

//SEMAFORO PARA ESCRITORES EN COLA
        key = ftok(path,55+id_nodo);

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
	printf("Escritor en el nodo %i sin hacer nada\n",id_nodo);
	getchar();
	printf("Escritor en la cola del nodo %i\n", id_nodo);

	sem_wait(sem);
	escritoresEnCola[0]++;
	sem_post(sem);

	printf("Hay %i escritores en la cola\n" ,escritoresEnCola[0]);
	msgrcv(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),(long)2,0 | MSG_EXCEPT);

	sem_wait(sem);
	escritoresEnCola[0]--;
	sem_post(sem);

	printf("Hay %i escritores en la cola\n", escritoresEnCola[0]);
	printf("Algoritmo internodo\n");
	printf("Escritor escribiendo en el nodo %i\n", id_nodo);
	getchar();
	sem_wait(sem);
	if(escritoresEnCola[0]){
		printf("Hay escritores en cola\n");
		permisoSC.tipo=3;
		msgsnd(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),0);  		
	}
	else{
		printf("No hay escritores en cola\n");
		permisoSC.tipo=1;
		msgsnd(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),0);
	}
	sem_post(sem);
	printf("Algoritmo internodo\n");

  }while(1);

}
