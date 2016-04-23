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
  shmid = shmget(key, sizeof(int[5], shmflg);
  printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  peticionesLectores = returnPtr;

  //Para compartir peticionesEscritores:
  id = 20 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int[5], shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  peticionesEscritores = returnPtr;

  //Para compartir servidosLectores
  id = 30 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int[5], shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  servidosLectores = returnPtr;

  //Para compartir servidosEscritores
  id = 40 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int[5], shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  servidosEscritores = returnPtr;

  //Para numero de lectores en SC
  id = 50 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  numNodLec = returnPtr;

  //Para saber si tenemos el testigo
  id = 60 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);   
  hasToken = returnPtr;


  //Para saber el número de peticiones de nuestro nodo
  id = 70 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  myNum = returnPtr;

  //Para saber si nuestro nodo está en SC
  id = 80 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  inSC = returnPtr;

  //Para saber si el representante es un lector o un escritor
  id = 90 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  lectorOEscritor = returnPtr;

  //Para saber si hay peticiones de otros nodos despues de que el ultimo lector salga de la SC
  id = 100 + 1 * id_nodo;
  key = ftok(path,id);
  shmid = shmget(key, sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  esperandoAviso = returnPtr;

	
	id = 98;
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
}
	

	while(1){

		printf("Pulse la tecla ENTER para intentar entrar a la sección crítica.\n");
		int entrar = getchar();

		sem_wait(sem_hasToken);
		if(!(*hasToken)){
			printf("Oooh espera, no tienes el testigo. Pidiendo el testigo...\n");
			sem_post(sem_hasToken);
			(*myNum)++;
			sem_wait(sem_lectorOEscritor);
			(*lectorOEscritor) = 0;
			sem_post(sem_lectorOEscritor);
			peticion.myID = id_nodo;
			peticion.myNum = (*myNum);
			peticion.lectorOEscritor = (*lectorOEscritor);
			for(id_nodo_sig=0; id_nodo_sig<4; id_nodo_sig++){

				//VA AQUIIIII
				peticion.mtype = id_nodos[id_nodo_sig];
				printf("Mandando peticion a la cola con el msqid %d\n",msqid_colas[id_nodo_sig]);
				msgsnd(msqid_colas[id_nodo_sig], (struct msgbuf *) &peticion, sizeof(peticion), 0);
			}

			printf("Intentando entrar...\n");
			printf("Esperando por el testigo en la cola %d con mi id %d\n",cola_token,id_nodo);
			msgrcv(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), (long)id_nodo, 0);
			printf("Testigo recibido! Menos mal, tanto tiempo esperando...\n");

		} else {
			printf("Tienes el testigo! Que suerte!\n");
			sem_post(sem_hasToken);
		}

		sem_wait(sem_hasToken);
		*hasToken = 1;
		sem_post(sem_hasToken);
		sem_wait(sem_numNodLec);
		*numNodLec = testigo.numNodLec;
		(*numNodLec)++;
		sem_post(sem_numNodLec);
		sem_wait(sem_inSC);
		*inSC = 1;
		sem_post(sem_inSC);

		printf("Lector leyendo.... Pulse la tecla ENTER para salir de la sección crítica.\n");
		int salir = getchar();


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
			peticion.lectorOEscritor = (*lectorOEscritor);
			
			for(id_nodo_sig=0; id_nodo_sig<4; id_nodo_sig++){
			printf("Pidiendo el testigo en la cola %d con mi id %d\n",msqid_colas[id_nodo_sig],id_nodo);
				peticion.mtype = id_nodo;
				msgsnd(msqid_colas[id_nodo_sig], (struct msgbuf *) &peticion, sizeof(peticion), 0);
			}
			printf("Intentando salir...\n");
			msgrcv(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), (long)id_nodo, 0);
			printf("Testigo recibido! Ahora ya nadie podra interponerse en tu camino\n");

		} else {
			printf("Uf, sigues teniendo el testigo. Menos mal. Saliendo...\n");
			sem_post(sem_hasToken);
		}

		printf("Has salido de la sección crítica.\n");

		sem_wait(sem_hasToken);
		*hasToken = 1;
		sem_post(sem_hasToken);

		sem_wait(sem_numNodLec);
		*numNodLec = testigo.numNodLec;
		(*numNodLec)--;
		sem_post(sem_numNodLec);

		sem_wait(sem_inSC);
		*inSC = 0;
		sem_post(sem_inSC);

		//sendToken()
		//Hay que ver lo de reservar y compartir memoria para el array, igual da violación de segmento.
		printf("Actualizando peticiones servidas\n");
		sem_wait(sem_servidosLectores);
		testigo.servidosLectores[id_nodo-1] = servidosLectores[id_nodo-1];
		sem_post(sem_servidosLectores);

		sem_wait(sem_servidosEscritores);
		testigo.servidosEscritores[id_nodo-1] = servidosEscritores[id_nodo-1];
		sem_post(sem_servidosEscritores);
		
		printf("Actualizando numero de lectores\n");
		sem_wait(sem_numNodLec);
		testigo.numNodLec = (*numNodLec);
		sem_post(sem_numNodLec);
		
		for(id_nodo_sig=0; id_nodo_sig < 5; id_nodo_sig++){
			if( (id_nodo_sig + 1) == id_nodo ) continue;

			sem_wait(sem_servidosEscritores);
			sem_wait(sem_peticionesEscritores);
			if(peticionesEscritores[id_nodo_sig] > servidosEscritores[id_nodo_sig]){
				sem_post(sem_peticionesEscritores);
				sem_post(sem_servidosEscritores);


				sem_wait(sem_numNodLec);
				if((*numNodLec) == 0){
					sem_post(sem_numNodLec);
				
					testigo.mtype = id_nodo_sig + 1;
					printf("Mandando el testigo...\n");
					msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);

					sem_wait(sem_hasToken); //repasar este semáforo
					*hasToken = 0;
					sem_post(sem_hasToken); //repasar este semáforo
					break;
				} else {
					sem_post(sem_numNodLec);

					//bloquearse esperando hasta petición
					sem_wait(sem_esperandoAviso);
					printf("Esperando aviso...\n");
					*esperandoAviso = 1;
					sem_post(sem_esperandoAviso);

					msgrcv(cola_warning, (struct msgbuf *) &aviso, sizeof(aviso), (long)4, 0);
					printf("Aviso recibido! Liberando testigo...\n");

					sem_wait(sem_esperandoAviso);
					*esperandoAviso = 0;
					sem_post(sem_esperandoAviso);
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

					testigo.mtype = id_nodo_sig + 1;
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
		} else {
			sem_post(sem_hasToken);
		}

	}
}
