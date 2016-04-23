#include <stdio.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define SEM_PETLEC "/peticionesLectores"
#define SEM_PETESC "/peticionesEscritores"
#define SEM_SERVLEC "/servidosLectores"
#define SEM_SERVESC "/servidosEscritores"
#define SEM_NUMNODLEC "/numNodosLectores"
#define SEM_HASTOKEN "/hasToken"
#define SEM_INSC "/inSC"
#define SEM_LECESC "/lectorOEscritor"
#define SEM_AVISO "/esperandoAviso"
#define SEM_RECEIVE "/receive"

struct request{
	long mtype;
	int myID;
	int myNum;
	int lectorOEscritor;
};

/*
struct message{
	long mtype;
};
*/

struct warning{
	long mtype;
};

struct token{
	long mtype;
	int servidosEscritores[5];
	int servidosLectores[5];
	int numNodLec;
};

int main(int argc, char * argv[]){

	if(argc != 2){
		perror("Error");
		exit(1);
	}

	int id_nodo = atoi(argv[1]);
	
	int id_nodos1[4] = {2, 3, 4, 5};				
	int id_colas1[4] = {12 ,13 ,14, 15};

	int id_nodos2[4] = {1, 3, 4, 5};
	int id_colas2[4] = {12, 23, 24, 25};

	int id_nodos3[4] = {1, 2, 4, 5};
	int id_colas3[4] = {13, 23, 34, 35};

	int id_nodos4[4] = {1, 2, 3, 5};
	int id_colas4[4] = {14, 24, 34, 45};

	int id_nodos5[4] = {1, 2, 3, 4};
	int id_colas5[4] = {15, 25, 35, 45};

	int id_nodos[4];
	int id_colas[4];
	int id_nodo_sig;

	struct token testigo;
	struct request peticion;
	struct warning aviso;

	switch(id_nodo){

		case 1: memcpy(id_nodos, id_nodos1, 4*sizeof(int));
			    memcpy(id_colas, id_colas1, 4*sizeof(int));
				break;

		case 2: memcpy(id_nodos, id_nodos2, 4*sizeof(int));
			    memcpy(id_colas, id_colas2, 4*sizeof(int));
				break;

		case 3: memcpy(id_nodos, id_nodos3, 4*sizeof(int));
			    memcpy(id_colas, id_colas3, 4*sizeof(int));
				break;

		case 4: memcpy(id_nodos, id_nodos4, 4*sizeof(int));
			    memcpy(id_colas, id_colas4, 4*sizeof(int));
				break;

		case 5: memcpy(id_nodos, id_nodos5, 4*sizeof(int));
			    memcpy(id_colas, id_colas5, 4*sizeof(int));
				break;

		default: break;
	}

	// Variables compartidas

	//Para compartir peticionesLectores:
	key_t key = 10000 + 1000 * id_nodo;

	int shmid = shmget(key, 5*sizeof(int), IPC_CREAT | 0777);
	int * shm_petlec = (int *) shmat(shmid, NULL, 0);

	int * peticionesLectores = shm_petlec;

	//Semáforo peticionesLectores
	key = 100000 + 1000 * id_nodo;
	sem_t * sem_peticionesLectores = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);
	

	//Para compartir peticionesEscritores:
	key = 10000 + 1050 * id_nodo;

	shmid = shmget(key, 5*sizeof(int), IPC_CREAT | 0777 );
	int * shm_petesc = (int *) shmat(shmid, NULL, 0);

	int * peticionesEscritores = shm_petesc;

	//Semáforo peticionesEscritores
	key = 100000 + 1050 * id_nodo;
	sem_t * sem_peticionesEscritores = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);
	

	//Para compartir servidosLectores
	key = 10000 + 1100 * id_nodo;

	shmid = shmget(key, 5*sizeof(int), IPC_CREAT | 0777 );
	int * shm_servlec = (int *) shmat(shmid, NULL, 0);

	int * servidosLectores = shm_servlec;

	//Semáforo servidosLectores
	key = 100000 + 1100 * id_nodo;
	sem_t * sem_servidosLectores = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

	//Para compartir servidosEscritores
	key = 10000 + 1150 * id_nodo;

	shmid = shmget(key, 5*sizeof(int), IPC_CREAT | 0777 );
	int * shm_servesc = (int *) shmat(shmid, NULL, 0);

	int * servidosEscritores = shm_servesc;

	//Semáforo servidosEscritores
	key = 100000 + 1150 * id_nodo;
	sem_t * sem_servidosEscritores = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

	//Para numero de lectores en SC

	key = 1000 + id_nodo;

	shmid = shmget(key, sizeof(int), IPC_CREAT | 0777 );
	int * shm1 = (int *) shmat(shmid, NULL, 0);

	int * numNodLec = shm1;

	//Semáforo numNodLec
	key = 10000 + id_nodo;
	sem_t * sem_numNodLec = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

	//Para saber si tenemos el testigo

	key = 1100 + id_nodo;
		
	shmid = shmget(key, sizeof(int), IPC_CREAT | 0777 );
	int * shm2 = (int *) shmat(shmid, NULL, 0);
		
	int * hasToken = shm2;

	//Semáforo hasToken
	key = 11000 + id_nodo;
	sem_t * sem_hasToken = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

	//Para saber el número de peticiones de nuestro nodo

	key = 1200 + id_nodo;

	shmid = shmget(key, sizeof(int), IPC_CREAT | 0777 );
	int * shm3 = (int *) shmat(shmid, NULL, 0);

	int * myNum = shm3;

	/*Semáforo myNum(en principio innecesario)
	key = 12000 + id_nodo;
	sem_t * sem_myNum = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);
	*/

	//Para saber si nuestro nodo está en SC

	key = 1300 + id_nodo;

	shmid = shmget(key, sizeof(int), IPC_CREAT | 0777 );
	int * shm4 = (int *) shmat(shmid, NULL, 0);

	int * inSC = shm4;

	//Semáforo inSC
	key = 13000 + id_nodo;
	sem_t * sem_inSC = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

	//Para saber si el representante es un lector o un escritor

	key = 1400 + id_nodo;

	shmid = shmget(key, sizeof(int), IPC_CREAT | 0777 );
	int * shm5 = (int *) shmat(shmid, NULL, 0);

	int * lectorOEscritor = shm5;

	//Semáforo lectorOEscritor
	key = 14000 + id_nodo;
	sem_t * sem_lectorOEscritor = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

	//Para saber si hay peticiones de otros nodos despues de que el ultimo lector salga de la SC

	key = 1500 + id_nodo;

	shmid = shmget(key, sizeof(int), IPC_CREAT | 0777 );
	int * shm6 = (int *) shmat(shmid, NULL, 0);

	int * esperandoAviso = shm6;

	//Semáforo lectorOEscritor
	key = 15000 + id_nodo;
	sem_t * sem_esperandoAviso = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);	

	//Colas del testigo y del aviso de peticion de testigo

	key_t token_key = 100;
	int cola_token = msgget(token_key, 0777 | IPC_CREAT);

	key_t warning_key = 150 + id_nodo;
	int cola_warning = msgget(warning_key, 0777 | IPC_CREAT);


	key_t request_key;
	int cola_request;

	while(1){
	
		printf("Pulse la tecla ENTER para intentar entrar a la sección crítica.\n");
		int entrar = getchar();

		sem_wait(sem_hasToken);
		if(!(*hasToken)){
			sem_post(sem_hasToken);

			(*myNum)++;
			sem_wait(sem_lectorOEscritor);
			(*lectorOEscritor) = 1;
			sem_post(sem_lectorOEscritor);

			peticion.myID = id_nodo;
			peticion.myNum = (*myNum);
			peticion.lectorOEscritor = (*lectorOEscritor);

			for(id_nodo_sig=0; id_nodo_sig<4; id_nodo_sig++){
				request_key = id_colas[id_nodo_sig];
				cola_request = msgget(request_key, 0777 | IPC_CREAT);
				msgsnd(cola_request, (struct msgbuf *) &peticion, sizeof(peticion), 0);
			}

			printf("Intentando entrar...\n");
			msgrcv(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), (long)id_nodo, 0);

		}
		else{
			sem_post(sem_hasToken);
		}

		sem_wait(sem_hasToken);
		*hasToken = 1;
		sem_post(sem_hasToken);

		sem_wait(sem_inSC);
		*inSC = 1;
		sem_post(sem_inSC);

		printf("Escritor escribiendo.... Pulse la tecla ENTER para salir de la sección crítica.\n");
		int salir = getchar();

		sem_wait(sem_servidosEscritores);
		servidosEscritores[id_nodo-1] = (*myNum);
		sem_post(sem_servidosEscritores);

		sem_wait(sem_inSC);
		*inSC = 0;
		sem_post(sem_inSC);

		//sendToken()
		//Hay que ver lo de reservar y compartir memoria para el array, igual da violación de segmento.

		sem_wait(sem_servidosLectores);
		testigo.servidosLectores[id_nodo-1] = servidosLectores[id_nodo-1];
		sem_post(sem_servidosLectores);

		sem_wait(sem_servidosEscritores);
		testigo.servidosEscritores[id_nodo-1] = servidosEscritores[id_nodo-1];
		sem_post(sem_servidosEscritores);

		sem_wait(sem_numNodLec);
		testigo.numNodLec = (*numNodLec);
		sem_post(sem_numNodLec);

		
		for(id_nodo_sig=0; id_nodo_sig < 5; id_nodo_sig++){
			if( (id_nodo_sig + 1) == id_nodo ) continue;

			sem_wait(sem_peticionesEscritores);
			if(peticionesEscritores[id_nodo_sig] > servidosEscritores[id_nodo_sig]){
				sem_post(sem_peticionesEscritores);
				testigo.mtype = id_nodo_sig + 1;
				msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);
				sem_wait(sem_hasToken);
				*hasToken = 0;
				sem_post(sem_hasToken);
				break;
			}
			else{
				sem_post(sem_peticionesEscritores);
			}
		}
		sem_wait(sem_hasToken);
		if (*hasToken != 0) {
			sem_post(sem_hasToken);
			for(id_nodo_sig=0; id_nodo_sig < 5; id_nodo_sig++){
				if( (id_nodo_sig + 1) == id_nodo ) continue;
				sem_wait(sem_peticionesLectores);	
				if(peticionesLectores[id_nodo_sig] > servidosLectores[id_nodo_sig]){
					sem_post(sem_peticionesLectores);
					testigo.mtype = id_nodo_sig + 1;
					msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);
					sem_wait(sem_hasToken);
					*hasToken = 0;
					sem_post(sem_hasToken);
					break;
				}
				else{
					sem_post(sem_peticionesLectores);
				}
			}
		}
		else{
			sem_post(sem_hasToken);
		}
	}
}
