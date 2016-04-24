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
#include <sys/stat.h>
#include <fcntl.h>

#define SEM_PETLEC1 "/peticionesLectoresUno"
#define SEM_PETESC1 "/peticionesEscritoresUno"
#define SEM_SERVLEC1 "/servidosLectoresUno"
#define SEM_SERVESC1 "/servidosEscritoresUno"
#define SEM_NUMNODLEC1 "/numNodosLectoresUno"
#define SEM_HASTOKEN1 "/hasTokenUno"
#define SEM_INSC1 "/inSCUno"
#define SEM_LECESC1 "/lectorOEscritorUno"
#define SEM_AVISO1 "/esperandoAvisoUno"
#define SEM_RECEIVE1 "/receiveUno"

#define SEM_PETLEC2 "/peticionesLectoresDos"
#define SEM_PETESC2 "/peticionesEscritoresDos"
#define SEM_SERVLEC2 "/servidosLectoresDos"
#define SEM_SERVESC2 "/servidosEscritoresDos"
#define SEM_NUMNODLEC2 "/numNodosLectoresDos"
#define SEM_HASTOKEN2 "/hasTokenDos"
#define SEM_INSC2 "/inSCDos"
#define SEM_LECESC2 "/lectorOEscritorDos"
#define SEM_AVISO2 "/esperandoAvisoDos"
#define SEM_RECEIVE2 "/receiveDos"

#define SEM_PETLEC3 "/peticionesLectoresTres"
#define SEM_PETESC3 "/peticionesEscritoresTres"
#define SEM_SERVLEC3 "/servidosLectoresTres"
#define SEM_SERVESC3 "/servidosEscritoresTres"
#define SEM_NUMNODLEC3 "/numNodosLectoresTres"
#define SEM_HASTOKEN3 "/hasTokenTres"
#define SEM_INSC3 "/inSCTres"
#define SEM_LECESC3 "/lectorOEscritorTres"
#define SEM_AVISO3 "/esperandoAvisoTres"
#define SEM_RECEIVE3 "/receiveTres"

#define SEM_PETLEC4 "/peticionesLectoresCua"
#define SEM_PETESC4 "/peticionesEscritoresCua"
#define SEM_SERVLEC4 "/servidosLectoresCua"
#define SEM_SERVESC4 "/servidosEscritoresCua"
#define SEM_NUMNODLEC4 "/numNodosLectoresCua"
#define SEM_HASTOKEN4 "/hasTokenCua"
#define SEM_INSC4 "/inSCCua"
#define SEM_LECESC4 "/lectorOEscritorCua"
#define SEM_AVISO4 "/esperandoAvisoCua"
#define SEM_RECEIVE4 "/receiveCua"

#define SEM_PETLEC5 "/peticionesLectoresCin"
#define SEM_PETESC5 "/peticionesEscritoresCin"
#define SEM_SERVLEC5 "/servidosLectoresCin"
#define SEM_SERVESC5 "/servidosEscritoresCin"
#define SEM_NUMNODLEC5 "/numNodosLectoresCin"
#define SEM_HASTOKEN5 "/hasTokenCin"
#define SEM_INSC5 "/inSCCin"
#define SEM_LECESC5 "/lectorOEscritorCin"
#define SEM_AVISO5 "/esperandoAvisoCin"
#define SEM_RECEIVE5 "/receiveCin"

//INTRANODO
#define MSG_EXCEPT 020000

struct permisoSC{
  long tipo;
};
//INTRANODO

//semaforos
sem_t * sem_peticionesLectores;
sem_t * sem_peticionesEscritores;
sem_t * sem_servidosLectores;
sem_t * sem_servidosEscritores;
sem_t * sem_numNodLec;
sem_t * sem_hasToken;
sem_t * sem_inSC;
sem_t * sem_lectorOEscritor;

//memoria compartida a nivel de nodo: punteros a esa memoria
int * peticionesLectores;
int * peticionesEscritores;
int * servidosLectores;
int * servidosEscritores;
int * numNodLec;
int * hasToken; //0 no lo tiene, 1 lo tiene
int * myNum;
int * inSC; //0 no esta, 1 esta
int * lectorOEscritor;  //0 lector, 1 escritor

int id_nodo;


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

int main(char argc, char * argv[]){

	if(argc != 2){
		perror("Error");
		exit(1);
	}

	id_nodo = atoi(argv[1]);
	printf("El id es %d\n",id_nodo);
	
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

	int msqid_colas[4]; //a inicializar despues

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

//Variables para la memoria compartida
  key_t key = 0;
  size_t size = sizeof(int);
  //int shmflg = SHM_R | SHM_W;
  int shmflg = IPC_CREAT | 0666;
  int shmid;
  int msqid;
  int * returnPtr;
  char *path = "/tmp";
  int id = 0;


  //Para compartir peticionesLectores:
  id = 10 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int[5]), shmflg);
  printf("El shmid es %d\n",shmid);
  peticionesLectores = (int*) shmat(shmid, NULL, 0);

  //Para compartir peticionesEscritores:
  id = 20 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int[5]), shmflg);
    printf("El shmid es %d\n",shmid);
  peticionesEscritores = (int*) shmat(shmid, NULL, 0);

  //Para compartir servidosLectores
  id = 30 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int[5]), shmflg);
    printf("El shmid es %d\n",shmid);
  servidosLectores = (int*) shmat(shmid, NULL, 0);

  //Para compartir servidosEscritores
  id = 40 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int[5]), shmflg);
    printf("El shmid es %d\n",shmid);
  servidosEscritores = (int*) shmat(shmid, NULL, 0);

  //Para numero de lectores en SC
  id = 50 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  numNodLec = (int*) shmat(shmid, NULL, 0);

  //Para saber si tenemos el testigo
  id = 60 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  hasToken = (int*) shmat(shmid, NULL, 0);


  //Para saber el número de peticiones de nuestro nodo
  id = 70 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  myNum = (int*) shmat(shmid, NULL, 0);

  //Para saber si nuestro nodo está en SC
  id = 80 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  printf("El shmid es %d\n",shmid);
  inSC = (int*) shmat(shmid, NULL, 0);

  //Para saber si el representante es un lector o un escritor
  id = 90 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
   printf("El shmid es %d\n",shmid);
  lectorOEscritor = (int*) shmat(shmid, NULL, 0);


	
	id = 120;
	key = ftok(path,id);
	int cola_token = msgget(key, 0666| IPC_CREAT);
	printf("cola_token %d\n",cola_token);
	if(cola_token == 0){
		printf("Error por culpa de la cola token! Saliendo del proceso...\n");
		return 0;
	}

	  //Creacion de las keys
  int f;
  for(f=0;f<4;f++){
    id = id_colas[f];
    key = ftok(path,id);
    msqid = msgget(key,shmflg);
    msqid_colas[f] = msqid;
  }


if(id_nodo==1){
	//SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
		//Semáforo peticionesLectores
	sem_peticionesLectores = sem_open(SEM_PETLEC1, 0);	
		//Semáforo peticionesEscritores
	sem_peticionesEscritores = sem_open(SEM_PETESC1, 0);	
	//Semáforo servidosLectores
	sem_servidosLectores = sem_open(SEM_SERVLEC1, 0);
	//Semáforo servidosEscritores
	sem_servidosEscritores = sem_open(SEM_SERVESC1, 0);
	//Semáforo numNodLec
	sem_numNodLec = sem_open(SEM_NUMNODLEC1, 0);
	//Semáforo hasToken
	sem_hasToken = sem_open(SEM_HASTOKEN1, 0);
	//Semáforo inSC
	sem_inSC = sem_open(SEM_INSC1, 0);
	//Semáforo lectorOEscritor
	sem_lectorOEscritor = sem_open(SEM_LECESC1, 0);		
}
if(id_nodo==2){
	//SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
		//Semáforo peticionesLectores
	sem_peticionesLectores = sem_open(SEM_PETLEC2, 0);	
		//Semáforo peticionesEscritores
	sem_peticionesEscritores = sem_open(SEM_PETESC2, 0);	
	//Semáforo servidosLectores
	sem_servidosLectores = sem_open(SEM_SERVLEC2, 0);
	//Semáforo servidosEscritores
	sem_servidosEscritores = sem_open(SEM_SERVESC2, 0);
	//Semáforo numNodLec
	sem_numNodLec = sem_open(SEM_NUMNODLEC2, 0);
	//Semáforo hasToken
	sem_hasToken = sem_open(SEM_HASTOKEN2, 0);
	//Semáforo inSC
	sem_inSC = sem_open(SEM_INSC2, 0);
	//Semáforo lectorOEscritor
	sem_lectorOEscritor = sem_open(SEM_LECESC2, 0);		
	}
if(id_nodo==3){
//SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
		//Semáforo peticionesLectores
	sem_peticionesLectores = sem_open(SEM_PETLEC3, 0);	
		//Semáforo peticionesEscritores
	sem_peticionesEscritores = sem_open(SEM_PETESC3, 0);	
	//Semáforo servidosLectores
	sem_servidosLectores = sem_open(SEM_SERVLEC3, 0);
	//Semáforo servidosEscritores
	sem_servidosEscritores = sem_open(SEM_SERVESC3, 0);
	//Semáforo numNodLec
	sem_numNodLec = sem_open(SEM_NUMNODLEC3, 0);
	//Semáforo hasToken
	sem_hasToken = sem_open(SEM_HASTOKEN3, 0);
	//Semáforo inSC
	sem_inSC = sem_open(SEM_INSC3, 0);
	//Semáforo lectorOEscritor
	sem_lectorOEscritor = sem_open(SEM_LECESC3, 0);		}
if(id_nodo==4){
//SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
		//Semáforo peticionesLectores
	sem_peticionesLectores = sem_open(SEM_PETLEC4, 0);	
		//Semáforo peticionesEscritores
	sem_peticionesEscritores = sem_open(SEM_PETESC4, 0);	
	//Semáforo servidosLectores
	sem_servidosLectores = sem_open(SEM_SERVLEC4, 0);
	//Semáforo servidosEscritores
	sem_servidosEscritores = sem_open(SEM_SERVESC4, 0);
	//Semáforo numNodLec
	sem_numNodLec = sem_open(SEM_NUMNODLEC4, 0);
	//Semáforo hasToken
	sem_hasToken = sem_open(SEM_HASTOKEN4, 0);
	//Semáforo inSC
	sem_inSC = sem_open(SEM_INSC4, 0);
	//Semáforo lectorOEscritor
	sem_lectorOEscritor = sem_open(SEM_LECESC4, 0);		
}
if(id_nodo==5){
//SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
		//Semáforo peticionesLectores
	sem_peticionesLectores = sem_open(SEM_PETLEC5, 0);	
		//Semáforo peticionesEscritores
	sem_peticionesEscritores = sem_open(SEM_PETESC5, 0);	
	//Semáforo servidosLectores
	sem_servidosLectores = sem_open(SEM_SERVLEC5, 0);
	//Semáforo servidosEscritores
	sem_servidosEscritores = sem_open(SEM_SERVESC5, 0);
	//Semáforo numNodLec
	sem_numNodLec = sem_open(SEM_NUMNODLEC5, 0);
	//Semáforo hasToken
	sem_hasToken = sem_open(SEM_HASTOKEN5, 0);
	//Semáforo inSC
	sem_inSC = sem_open(SEM_INSC5, 0);
	//Semáforo lectorOEscritor
	sem_lectorOEscritor = sem_open(SEM_LECESC5, 0);		
}


//INIT INTRANODO
	int id_cola_aux[1];
	int *id_cola, *escritoresEnCola;
	int idqueue;
	sem_t *sem;
    struct permisoSC permisoSC;
	

	id_cola=id_cola_aux;

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

	while(1){

		/************************************ INICIO DEL PREPROTOCOLO DE INTRANODO ******************************/

	printf("Escritor en el nodo %i sin hacer nada. Pulsa ENTER para ponerte a la cola\n",id_nodo);
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


		/************************************* FIN DEL PREPROTOCOLO DE INTRANODO ******************************/

		printf("Pulse la tecla ENTER para intentar entrar a la sección crítica.\n");
		int entrar = getchar();


		/********************************* INICIO DEL PREPROTOCOLO ****************************************************/

		sem_wait(sem_hasToken);
		if(!(*hasToken)){
			sem_post(sem_hasToken);
			(*myNum)++;
			//printf("Creada una peticion con numero %d\n",*myNum);
			sem_wait(sem_lectorOEscritor);
			*lectorOEscritor = 1;
			sem_post(sem_lectorOEscritor);

			peticion.myID = id_nodo;
			peticion.myNum = (*myNum);

			sem_wait(sem_lectorOEscritor);
			peticion.lectorOEscritor = (*lectorOEscritor);
			sem_post(sem_lectorOEscritor);

			for(id_nodo_sig=0; id_nodo_sig<4; id_nodo_sig++){
				peticion.mtype = id_nodos[id_nodo_sig];
				printf("Mandando peticion a la cola con el msqid %d\n",msqid_colas[id_nodo_sig]);
				msgsnd(msqid_colas[id_nodo_sig], (struct msgbuf *) &peticion, sizeof(peticion), 0);
			}

			printf("Esperando por el testigo en la cola %d con mi id %d\n",cola_token,id_nodo);
			msgrcv(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), (long)id_nodo, 0);
			printf("Testigo recibido! Menos mal, ya estaba empezando a cansarme...\n");
			//printf("Actualizando lectores y escritores servidos en el proceso...\n");

			sem_wait(sem_servidosEscritores);
		 	memcpy(servidosEscritores, testigo.servidosEscritores, sizeof(int[5]));
		 	sem_post(sem_servidosEscritores);

		 	sem_wait(sem_servidosLectores);
		 	memcpy(servidosLectores, testigo.servidosLectores, sizeof(int[5]));
		 	sem_post(sem_servidosLectores);

		 	sem_wait(sem_numNodLec);
		 	(*numNodLec) = testigo.numNodLec;
		 	sem_post(sem_numNodLec);


		}
		else{
			sem_post(sem_hasToken);
			printf("Que bien! Tienes el testigo!\n");
		}

		sem_wait(sem_lectorOEscritor);
		*lectorOEscritor = 1;
		sem_post(sem_lectorOEscritor);

		sem_wait(sem_hasToken);
		*hasToken = 1;
		sem_post(sem_hasToken);

		sem_wait(sem_inSC);
		*inSC = 1;
		sem_post(sem_inSC);

		/********************************************** FIN DEL PREPROTOCOLO *****************************************************/

		printf("Escritor escribiendo.... Pulse la tecla ENTER para salir de la sección crítica.\n");
		int salir = getchar();

		/*********************************************** INICIO DEL POSTPROTOCOLO *************************************************/

		//printf("Actualizando variables de servidos...\n");

		sem_wait(sem_servidosEscritores);
		servidosEscritores[id_nodo-1] = (*myNum);
		sem_post(sem_servidosEscritores);

		sem_wait(sem_inSC);
		*inSC = 0;
		sem_post(sem_inSC);

		//sendToken()
		//printf("Actualizando peticiones servidas en el token...\n");
		sem_wait(sem_servidosLectores);
		memcpy(testigo.servidosLectores, servidosLectores, sizeof(int[5]));
		sem_post(sem_servidosLectores);

		sem_wait(sem_servidosEscritores);
		memcpy(testigo.servidosEscritores, servidosEscritores, sizeof(int[5]));
		sem_post(sem_servidosEscritores);

		sem_wait(sem_numNodLec);
		testigo.numNodLec = (*numNodLec);
		sem_post(sem_numNodLec);

		printf("Comprobando si hay peticiones que atender...\n");
		for(id_nodo_sig=0; id_nodo_sig < 5; id_nodo_sig++){
			if( (id_nodo_sig + 1) == id_nodo ) continue;
			//printf("El id del nodo a analizar ahora es %d\n",id_nodo_sig);

			sem_wait(sem_peticionesEscritores);
			if(peticionesEscritores[id_nodo_sig] > servidosEscritores[id_nodo_sig]){
				sem_post(sem_peticionesEscritores);
				testigo.mtype = id_nodo_sig + 1;
				sem_wait(sem_servidosEscritores);
				servidosEscritores[id_nodo_sig] = peticionesEscritores[id_nodo_sig];
				testigo.servidosEscritores[id_nodo_sig] = servidosEscritores[id_nodo_sig];
				sem_post(sem_servidosEscritores);
				printf("Peticion encontrada! Mandando el testigo al proceso %d en la cola %d\n",id_nodo_sig+1,cola_token);
				msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);
				printf("Adios, testigo, volveremos a vernos!\n");
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
					sem_wait(sem_servidosLectores);
					servidosLectores[id_nodo_sig] = peticionesLectores[id_nodo_sig];
					testigo.servidosLectores[id_nodo_sig] = servidosLectores[id_nodo_sig];
					sem_post(sem_servidosLectores);
					printf("Peticion encontrada! Mandando el testigo al proceso %d en la cola %d\n",id_nodo_sig+1,cola_token);
					msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);
					printf("Adios, testigo, volveremos a vernos!\n");
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

		/**************************************** FIN DEL POSTPROTOCOLO **********************************************/

		printf("pulsa ENTER para dar paso a otros procesos de tu nodo %i\n", id_nodo);
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
	}
}
