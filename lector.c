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
#define SEM_AV1 "/avisoUno"

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
#define SEM_AV2 "/avisoDos"


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
#define SEM_AV3 "/avisoTres"


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
#define SEM_AV4 "/avisoCuatro"


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
#define SEM_AV5 "/avisoCin"

#define MSG_EXCEPT 020000

struct permisoSC{
  long tipo;
};

//semaforos
sem_t * sem_peticionesLectores;
sem_t * sem_peticionesEscritores;
sem_t * sem_servidosLectores;
sem_t * sem_servidosEscritores;
sem_t * sem_numNodLec;
sem_t * sem_hasToken;
sem_t * sem_inSC;
sem_t * sem_lectorOEscritor;
sem_t * sem_esperandoAviso;
sem_t * sem_aviso;

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
int * esperandoAviso; //0 no esperando, 1 esperando

int id_nodo;


struct request{
	long mtype;
	int myID;
	int myNum;
	int lectorOEscritor;
};


struct warning{
	long mtype;
	int num;
};

struct token{
	long mtype;
	int servidosEscritores[5];
	int servidosLectores[5];
	int numNodLec;
};


int main(char argc, char * argv[]){

	if(argc < 2){
    printf("Error: falta el ID\n");
    return 0;
  }
  else id_nodo = atoi(argv[1]);

  printf("Tu id es %d",id_nodo);
  
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

  // Variables compartidas

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
  inSC = (int*) shmat(shmid, NULL, 0);

  //Para saber si el representante es un lector o un escritor
  id = 90 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  lectorOEscritor = (int*) shmat(shmid, NULL, 0);

  //Para saber si hay peticiones de otros nodos despues de que el ultimo lector salga de la SC
  id = 100 + 1 * id_nodo;
  key = ftok(path,id);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  esperandoAviso = (int*) shmat(shmid, NULL, 0);

	
	id = 120;
	key = ftok(path,id);
	int cola_token = msgget(key, 0666| IPC_CREAT);
	printf("cola_token %d\n",cola_token);
	if(cola_token == 0){
		printf("Error por culpa de la cola token! Saliendo del proceso...\n");
		return 0;
	}

	id = 150 + id_nodo;
	key = ftok(path,id);
	int cola_warning = msgget(key, 0666| IPC_CREAT);
	printf("cola_warning %d\n",cola_warning);



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
	//Semáforo Aviso
	sem_esperandoAviso = sem_open(SEM_AVISO1, 0);
	//sem aviso
	sem_aviso = sem_open(SEM_AV1, 0);
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
	//Semáforo Aviso
	sem_esperandoAviso = sem_open(SEM_AVISO2, 0);
		//sem aviso
	sem_aviso = sem_open(SEM_AV2, 0);
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
	sem_lectorOEscritor = sem_open(SEM_LECESC3, 0);		
	//Semáforo Aviso
	sem_esperandoAviso = sem_open(SEM_AVISO3, 0);
		//sem aviso
	sem_aviso = sem_open(SEM_AV3, 0);
}
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
	//Semáforo Aviso
	sem_esperandoAviso = sem_open(SEM_AVISO4, 0);
		//sem aviso
	sem_aviso = sem_open(SEM_AV4, 0);
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
	//Semáforo Aviso
	sem_esperandoAviso = sem_open(SEM_AVISO5, 0);
		//sem aviso
	sem_aviso = sem_open(SEM_AV5, 0);
}


//INIC DEL INTRANODO
int id_cola_aux[1];
	int *id_cola, *numLec;
	int idqueue;
	sem_t *sem;
        struct permisoSC permisoSC;

	

	id_cola = id_cola_aux;


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
	

	while(1){


		//****************** INICIO DEL PREPROTOCOLO DEL INTRANODO ********************************//

			printf("Lector en el nodo %i sin hacer nada\n",id_nodo);
        getchar();
        printf("Lector en la cola del nodo %i\n", id_nodo);
		msgrcv(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),(long)3,0 | MSG_EXCEPT);
		if(permisoSC.tipo == 1){
		printf("Algoritmo internodo\n");
		printf("Eres el primer lector! Accediendo a la seccion critica internodo...\n");
	


		/********************** FIN DEL PREPROTOCOLO DEL INTRANODO ****************************/

		printf("Pulse la tecla ENTER para intentar entrar a la sección crítica.\n");
		int entrar = getchar();

		/************************************* INICIO DEL PREPROTOCOLO ********************************************/

		sem_wait(sem_hasToken);
		if(!(*hasToken)){
			printf("Oooh espera, no tienes el testigo. Pidiendo el testigo...\n");
			sem_post(sem_hasToken);
			(*myNum)++;
			printf("Creando una peticion para el testigo con numero %d\n",*myNum);
			sem_wait(sem_lectorOEscritor);
			(*lectorOEscritor) = 0;
			sem_post(sem_lectorOEscritor);
			peticion.myID = id_nodo;
			peticion.myNum = (*myNum);
			sem_wait(sem_peticionesLectores);
			peticion.lectorOEscritor = (*lectorOEscritor);
			sem_post(sem_peticionesLectores);
			for(id_nodo_sig=0; id_nodo_sig<4; id_nodo_sig++){

				peticion.mtype = id_nodos[id_nodo_sig];
				printf("Mandando peticion a la cola con el msqid %d\n",msqid_colas[id_nodo_sig]);
				msgsnd(msqid_colas[id_nodo_sig], (struct msgbuf *) &peticion, sizeof(peticion), 0);
			}

			printf("Intentando entrar...\n");
			printf("Esperando por el testigo en la cola %d con mi id %d\n",cola_token,id_nodo);
			msgrcv(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), (long)id_nodo, 0);
			printf("Testigo recibido! Menos mal, tanto tiempo esperando...\n");
			printf("Actualizando lectores y escritores servidos en el proceso...\n");

			sem_wait(sem_servidosEscritores);
		 	memcpy(servidosEscritores, testigo.servidosEscritores, sizeof(int[5]));
		 	sem_post(sem_servidosEscritores);

		 	sem_wait(sem_servidosLectores);
		 	memcpy(servidosLectores, testigo.servidosLectores, sizeof(int[5]));
		 	sem_post(sem_servidosLectores);

		 	(*numNodLec) = testigo.numNodLec;
		 	printf("Actualizando servidosLectores ahora que hemos recibido el testigo...\n");

		 	sem_wait(sem_servidosLectores);
		 	servidosLectores[id_nodo-1] = (*myNum);
		 	sem_post(sem_servidosLectores);

		 	*numNodLec = testigo.numNodLec;
		 	printf("Numero de lectores leyendo: %d\n",*numNodLec);

		} else {
			printf("Tienes el testigo! Que suerte!\n");
			sem_post(sem_hasToken);
		}

		sem_wait(sem_hasToken);
		*hasToken = 1;
		sem_post(sem_hasToken);
		sem_wait(sem_numNodLec);
		(*numNodLec)++;
		printf("Numero de lectores leyendo conmigo: %d\n",*numNodLec);
		sem_post(sem_numNodLec);
		sem_wait(sem_inSC);
		*inSC = 1;
		sem_post(sem_inSC);

		
		//SENDTOKEN
				//sendToken()
		printf("Comprobando si hay peticiones de lectores pendientes...\n");
		sem_wait(sem_servidosLectores);
		 memcpy(testigo.servidosLectores, servidosLectores, sizeof(int[5]));
		sem_post(sem_servidosLectores);

		sem_wait(sem_servidosEscritores);
		 memcpy(testigo.servidosEscritores, servidosEscritores, sizeof(int[5]));
		sem_post(sem_servidosEscritores);
		
		printf("Actualizando numero de lectores en el token...\n");
		sem_wait(sem_numNodLec);
		testigo.numNodLec = (*numNodLec);
		sem_post(sem_numNodLec);
		
		sem_wait(sem_hasToken);
		if (*hasToken != 0) {
			sem_post(sem_hasToken);

			for(id_nodo_sig=0; id_nodo_sig < 5; id_nodo_sig++){
				if( (id_nodo_sig + 1) == id_nodo ) continue;

				sem_wait(sem_peticionesLectores);
				sem_wait(sem_servidosLectores);
				if(peticionesLectores[id_nodo_sig] > servidosLectores[id_nodo_sig]){
					sem_post(sem_peticionesLectores);
					sem_post(sem_servidosLectores);

					testigo.mtype = id_nodo_sig + 1;
					sem_wait(sem_servidosLectores);
					servidosLectores[id_nodo_sig] = peticionesLectores[id_nodo_sig];
					testigo.servidosLectores[id_nodo_sig] = servidosLectores[id_nodo_sig];
					sem_post(sem_servidosLectores);
					printf("Mandando el testigo al lector pendiente...\n");
					msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);

					sem_wait(sem_hasToken); //repasar este semáforo
					*hasToken = 0;
					sem_post(sem_hasToken); //repasar este semáforo
					break;
				} else {
					sem_post(sem_peticionesLectores);
					sem_post(sem_servidosLectores);
				}
			}
		} else {
			sem_post(sem_hasToken);
		}

		}
		//FIN SENDTOKEN

		/******************************************* PREPROTOCOLO INTRANODO SEGUNDA PARTE *****************************/
		sem_wait(sem);
        numLec[0]++;
		sem_post(sem);
	
		permisoSC.tipo=2;
		msgsnd(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),0);
        printf("Hay %i lectores en la cola\n" ,numLec[0]);

        printf("Lector leyendo en el nodo %i\n", id_nodo);
		getchar();

		/****************************************** FIN DEL PREPROTOCOLO INTRANODO SEGUNDA PARTE *******************/

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
        	printf("Algoritmo postprotocolo\n");
		
		/*******************************************  FIN DEL PREPROTOCOLO ****************************************************/


		printf("Lector leyendo.... Pulse la tecla ENTER para salir de la sección crítica.\n");
		int salir = getchar();

		/********************************** INICIO DEL POSTPROTOCOLO INTRANODO PRMIERA PARTE *************************++/

		/******************************************* INICIO DEL POSTPROTOCOLO ***************************************************/

		sem_wait(sem_servidosLectores);
		servidosLectores[id_nodo-1] = (*myNum);
		sem_post(sem_servidosLectores);


		sem_wait(sem_hasToken);
		if(!(*hasToken)){
			printf("Tsk te han vuelto a quitar el testigo? Espera que esto lo arreglo yo. Pidiendo el testigo...\n");
			sem_post(sem_hasToken);
			
			(*myNum)++;

			sem_wait(sem_lectorOEscritor);
			(*lectorOEscritor) = 0;
			sem_post(sem_lectorOEscritor);

			peticion.myID = id_nodo;
			peticion.myNum = (*myNum);

			sem_wait(sem_lectorOEscritor);
			peticion.lectorOEscritor = (*lectorOEscritor);
			sem_post(sem_lectorOEscritor);
			
			for(id_nodo_sig=0; id_nodo_sig<4; id_nodo_sig++){
			printf("Pidiendo el testigo en la cola %d con mi id %d\n",msqid_colas[id_nodo_sig],id_nodo);
				peticion.mtype = id_nodos[id_nodo_sig];
				msgsnd(msqid_colas[id_nodo_sig], (struct msgbuf *) &peticion, sizeof(peticion), 0);
			}
			printf("Intentando salir...\n");
			printf("Espernado por el testigo en la cola %d con tipo %d\n",cola_token,id_nodo);
			msgrcv(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), (long)id_nodo, 0);
			printf("Testigo recibido! Ahora ya nadie podra interponerse en tu camino\n");
			printf("Actualizando lectores y escritores servidos en el proceso...\n");

			sem_wait(sem_servidosEscritores);
		 	memcpy(servidosEscritores, testigo.servidosEscritores, sizeof(int[5]));
		 	sem_post(sem_servidosEscritores);

		 	sem_wait(sem_servidosLectores);
		 	memcpy(servidosLectores, testigo.servidosLectores, sizeof(int[5]));
		 	sem_post(sem_servidosLectores);

		 	sem_wait(sem_numNodLec);
		 	(*numNodLec) = testigo.numNodLec;
		 	sem_post(sem_numNodLec);

		 	printf("Actualizando servidosLectores ahora que hemos recibido el testigo...\n");
		 	sem_wait(sem_servidosLectores);
		 	servidosLectores[id_nodo-1] = (*myNum);
		 	sem_post(sem_servidosLectores);

		} else {
			printf("Uf, sigues teniendo el testigo. Menos mal. Saliendo...\n");
			sem_post(sem_hasToken);
		}

		printf("Has salido de la sección crítica.\n");

		sem_wait(sem_hasToken);
		*hasToken = 1;
		sem_post(sem_hasToken);
	
		sem_wait(sem_numNodLec);
		(*numNodLec)--;
		sem_post(sem_numNodLec);
		 
		sem_wait(sem_inSC);
		*inSC = 0;
		sem_post(sem_inSC);

		//sendToken()
		printf("Actualizando peticiones servidas...\n");
		sem_wait(sem_servidosLectores);
		memcpy(testigo.servidosLectores, servidosLectores, sizeof(int[5]));
		sem_post(sem_servidosLectores);

		sem_wait(sem_servidosEscritores);
		memcpy(testigo.servidosEscritores, servidosEscritores, sizeof(int[5]));
		sem_post(sem_servidosEscritores);
		
		printf("Actualizando numero de lectores en el token...\n");
		sem_wait(sem_numNodLec);
		testigo.numNodLec = (*numNodLec);
		sem_post(sem_numNodLec);

		int hayPet = 0;
		
		for(id_nodo_sig=0; id_nodo_sig < 5; id_nodo_sig++){
			if( (id_nodo_sig + 1) == id_nodo ) continue;

			sem_wait(sem_servidosEscritores);
			sem_wait(sem_peticionesEscritores);
			if(peticionesEscritores[id_nodo_sig] > servidosEscritores[id_nodo_sig]){
				sem_post(sem_peticionesEscritores);
				sem_post(sem_servidosEscritores);
				hayPet = 1;

				sem_wait(sem_numNodLec);
				if((*numNodLec) == 0){
					printf("El numero de lectores es %d\n",*numNodLec);
					sem_post(sem_numNodLec);
				
					testigo.mtype = id_nodo_sig + 1;
					sem_wait(sem_servidosEscritores);
					servidosEscritores[id_nodo_sig] = peticionesEscritores[id_nodo_sig];
					testigo.servidosEscritores[id_nodo_sig] = servidosEscritores[id_nodo_sig];
					sem_post(sem_servidosEscritores);
					printf("Mandando el testigo al proceso %d en la cola %d...\n",id_nodo_sig+1,cola_token);
					msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);

					sem_wait(sem_hasToken); //repasar este semáforo
					*hasToken = 0;
					sem_post(sem_hasToken); //repasar este semáforo
				} else {
					printf("No se puede mandar el testigo porque l numero de lectores leyendo es %d\n",*numNodLec);
					sem_post(sem_numNodLec);

					break;
				}
			} else {
				sem_post(sem_peticionesEscritores);
				sem_post(sem_servidosEscritores);
			}
		}

		sem_wait(sem_hasToken);
		if (*hasToken != 0) {
			sem_post(sem_hasToken);

			for(id_nodo_sig=0; id_nodo_sig < 5; id_nodo_sig++){
				if( (id_nodo_sig + 1) == id_nodo ) continue;

				sem_wait(sem_peticionesLectores);
				sem_wait(sem_servidosLectores);
				if(peticionesLectores[id_nodo_sig] > servidosLectores[id_nodo_sig]){
					sem_post(sem_peticionesLectores);
					sem_post(sem_servidosLectores);
					hayPet = 1;

					testigo.mtype = id_nodo_sig + 1;
					sem_wait(sem_servidosLectores);
					servidosLectores[id_nodo_sig] = peticionesLectores[id_nodo_sig];
					testigo.servidosLectores[id_nodo_sig] = servidosLectores[id_nodo_sig];
					sem_post(sem_servidosLectores);
					printf("Mandando el testigo...\n");
					msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);

					sem_wait(sem_hasToken); //repasar este semáforo
					*hasToken = 0;
					sem_post(sem_hasToken); //repasar este semáforo
					break;
				} else {
					sem_post(sem_peticionesLectores);
					sem_post(sem_servidosLectores);
				}
			}
			if(hayPet == 0){
				sem_wait(sem_numNodLec);
				if(*numNodLec!=0){
					sem_post(sem_numNodLec);
			//bloquearse esperando hasta petición
					sem_wait(sem_esperandoAviso);
					printf("Esperando aviso...\n");
					*esperandoAviso = 1;
					sem_post(sem_esperandoAviso);
					printf("No se para por culpa del semaforo palabrita del nino jesus\n");

					//msgrcv(cola_warning, (struct msgbuf *) &aviso, sizeof(aviso), 0, 0);
					sem_wait(sem_aviso);
					printf("Aviso recibido! Continuando ejecucion...\n");

					sem_wait(sem_esperandoAviso);
					*esperandoAviso = 0;
					sem_post(sem_esperandoAviso);
				}
				else{
					sem_post(sem_numNodLec);
				}
				}

		} else {
			sem_post(sem_hasToken);
		}

		/********************************************FIN DEL POSTPROTOCOLO ************************************************************/

		/*************************** INICIO DEL POSTPROTOCOLO INTRANODO PARTE 2 ****************************************/
		permisoSC.tipo=1;
        msgsnd(idqueue,(struct msgbuf *)&permisoSC,sizeof(permisoSC),0);
		}
			sem_post(sem);

	}
}
