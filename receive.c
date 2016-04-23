/************ EJERCICIO 5 *********************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/shm.h>

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

int cola_token;
int cola_warning;
int cola_peticion;

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
sem_t * sem_receive;

//struct que se le pasa a los threads
typedef struct thdata_th{
  pthread_t id_hilo;
  int numNodo;
  int msqid;
} thdata;

typedef struct request_th{
  long mtype;
  int myID;
  int myNum;
  int lectorOEscritor;
}request;


struct warning{
  long mtype;
}aviso;

struct token{
  long mtype;
  int servidosEscritores[5];
  int servidosLectores[5];
  int numNodLec;
}testigo;

int fin = 0;



void sendToken(int id_nodoReq, int tipoReq);



void thread_receive(void *ptr){
  thdata *data = ptr;

  request peticion;

  while(fin == 0){

   printf("Esperando por peticiones del nodo %d\n",data->numNodo);
    //buzon
    int msgflg;
    int msqid = data->msqid;

    size_t msgsz = 20;
    long msgtyp = 0;
    
    //esperamos a recibir una peticion de algun nodo
    msgrcv(msqid, (struct msgbuf *)&peticion,sizeof(peticion),(long)id_nodo,0);
    printf("Peticion del nodo %d de tipo %d",data->numNodo,peticion.lectorOEscritor);
    
    sem_wait(sem_receive);
    //si recibimos una peticion de un escritor
    if(peticion.lectorOEscritor == 1){
      sem_wait(sem_peticionesEscritores);
      peticionesEscritores[peticion.myID] = peticionesEscritores[peticion.myID]>peticion.myNum ? peticionesEscritores[peticion.myID] : peticion.myNum;
      sem_post(sem_peticionesEscritores);

      sem_wait(sem_hasToken);
      sem_wait(sem_inSC);
      if((*hasToken == 1)&&(*inSC == 0)) { 
         sem_post(sem_inSC);
         sem_post(sem_hasToken);
         sendToken(peticion.myID,peticion.lectorOEscritor);
      }
      else{
         sem_post(sem_inSC);
         sem_post(sem_hasToken);
      }
    }
    
    //si recibimos una peticion de un lector
    else if(peticion.lectorOEscritor == 0){
      sem_wait(sem_peticionesLectores);
      peticionesLectores[peticion.myID] =  peticionesLectores[peticion.myID]>peticion.myNum ? peticionesLectores[peticion.myID] : peticion.myNum;
      sem_post(sem_peticionesLectores);

      sem_wait(sem_hasToken);
      if(*hasToken == 1){
         sem_post(sem_hasToken);
         if(lectorOEscritor == 0){
            sem_wait(sem_esperandoAviso);
            if(*esperandoAviso == 1) {
            sem_post(sem_esperandoAviso);
             msgsnd(cola_warning, (struct msgbuf *) &aviso, sizeof(aviso), 0);
            }
            else{
               sem_post(sem_esperandoAviso);
            }  
	        sendToken(peticion.myID,peticion.lectorOEscritor);
	     }
	     else 
         sem_wait(sem_inSC);
         if(*inSC == 0) {
            sem_post(sem_inSC);
            sendToken(peticion.myID,peticion.lectorOEscritor);
         }
         else{
            sem_post(sem_inSC);
         }
      }
      else{
         sem_post(sem_hasToken);
      }
    }
    
   sem_post(sem_receive);
  }
  
}




//Lanzar receive: nº nodo

int main (char argc, char *argv[]){

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

  struct token testigo;
  struct request_th peticion;
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
  int * returnPtr;
  char *path = "/tmp";
  int id = 0;


  //Para compartir peticionesLectores:
  id = 10 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, 5*sizeof(int), shmflg);
  printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  peticionesLectores = returnPtr;

  //Para compartir peticionesEscritores:
  id = 20 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, 5*sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  peticionesEscritores = returnPtr;

  //Para compartir servidosLectores
  id = 30 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, 5*sizeof(int), shmflg);
    printf("El shmid es %d\n",shmid);
  returnPtr = (int*) shmat(shmid, NULL, 0);
  servidosLectores = returnPtr;

  //Para compartir servidosEscritores
  id = 40 + 1 * id_nodo;
  key = ftok(path,id);
  printf("La key es %d\n",key);
  shmid = shmget(key, 5*sizeof(int), shmflg);
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

  //Colas del testigo y del aviso de peticion de testigo
  key_t token_key = 100;
  cola_token = msgget(token_key, shmflg);

  key_t warning_key = 150 + id_nodo;
  cola_warning = msgget(warning_key, shmflg);


  //Sección internodo
  key_t peticion_key;



  //SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
  //Semáforo peticionesLectores
  sem_peticionesLectores = sem_open(SEM_PETLEC, O_CREAT, 0644, 1);
  //Semáforo peticionesEscritores
  sem_peticionesEscritores = sem_open(SEM_PETESC, O_CREAT, 0644, 1);
  //Semáforo servidosLectores
  sem_servidosLectores = sem_open(SEM_SERVLEC, O_CREAT, 0644, 1);
  //Semáforo servidosEscritores
  sem_servidosEscritores = sem_open(SEM_SERVESC, O_CREAT, 0644, 1);
  //Semáforo numNodLec
  sem_numNodLec = sem_open(SEM_NUMNODLEC, O_CREAT, 0644, 1);
  //Semáforo hasToken
  sem_hasToken = sem_open(SEM_HASTOKEN, O_CREAT, 0644, 1);
  //Semáforo inSC
  sem_inSC= sem_open(SEM_INSC, O_CREAT, 0644, 1);
  //Semáforo lectorOEscritor
  sem_lectorOEscritor = sem_open(SEM_LECESC, O_CREAT, 0644, 1);
  //Semáforo espernadoAviso
  sem_esperandoAviso = sem_open(SEM_AVISO, O_CREAT, 0644, 1);
  //Semaforo receive
  sem_receive = sem_open(SEM_RECEIVE, O_CREAT, 0644, 1);



  //THREADS
  printf("A punto de inicializar token\n");
  if(id_nodo == 1){
    *hasToken = 1;
    printf("Felicidades! Tienes el token %d \n",*hasToken);
  }
  else {
    *hasToken = 0;
    printf("Ooooh lo siento, no tienes el token\n");
  }

   printf("Llega aqui\n");

  *esperandoAviso = 0;
  *inSC = 0;
  *numNodLec = 0;
  int petLec[] = {0,0,0,0,0};
  int petEsc[] = {0,0,0,0,0};
  int servLec[] = {0,0,0,0,0};
  int servEsc[] = {0,0,0,0,0};
  //*peticionesLectores = petLec;
  //printf("Las peticiones de lectores en el nodo 1 son %d \n",peticionesLectores[0]);



  pthread_t thread[4];
  thdata *data;

  data = (thdata *) calloc(4, sizeof(thdata));

  int i; 

  for(i = 0; i<4;i++){
   peticion_key = id_colas[i];   
   cola_peticion = msgget(peticion_key, 0777 | IPC_CREAT);
	data[i].numNodo = id_nodos[i];
   data[i].msqid = cola_peticion;
	pthread_create(&(data[i].id_hilo), NULL,(void *)&thread_receive,(void *)&data[i]);
  printf("Creado el thread %i\n", i);
  }
  
  for (i=0; i<4; i++)
    pthread_join(data[i].id_hilo,NULL);
  
}






void sendToken(int id_nodoReq, int tipoReq){

sem_wait(sem_lectorOEscritor);
if(*lectorOEscritor==0){
    sem_post(sem_lectorOEscritor);
    sem_wait(sem_servidosLectores);
    testigo.servidosLectores[id_nodo-1] = servidosLectores[id_nodo-1];
    sem_post(sem_servidosLectores);

    sem_wait(sem_servidosEscritores);
    testigo.servidosEscritores[id_nodo-1] = servidosEscritores[id_nodo-1];
    sem_post(sem_servidosEscritores);
    
    sem_wait(sem_numNodLec);
    testigo.numNodLec = (*numNodLec);
    sem_post(sem_numNodLec);
    
    
      
      if(tipoReq==1){

        sem_wait(sem_numNodLec);
        if((*numNodLec) == 0){
          sem_post(sem_numNodLec);
        
          testigo.mtype = id_nodoReq;
          msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);

          sem_wait(sem_hasToken); //repasar este semáforo
          *hasToken = 0;
          sem_post(sem_hasToken); //repasar este semáfor
        } else {
          sem_post(sem_numNodLec);
        }
      } 
    
      if(tipoReq==0){
        sem_wait(sem_hasToken);
        if (*hasToken != 0) {
         sem_post(sem_hasToken);


          testigo.mtype = id_nodoReq;
          msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);

          sem_wait(sem_hasToken); //repasar este semáforo
          *hasToken = 0;
          sem_post(sem_hasToken); //repasar este semáforo
        } else {
        }
      }
    }

   else{
      sem_post(sem_lectorOEscritor);
      sem_wait(sem_lectorOEscritor);
      if(*lectorOEscritor==1){
         sem_post(sem_lectorOEscritor);
         sem_wait(sem_servidosLectores);
         testigo.servidosLectores[id_nodo-1] = servidosLectores[id_nodo-1];
         sem_post(sem_servidosLectores);

         sem_wait(sem_servidosEscritores);
         testigo.servidosEscritores[id_nodo-1] = servidosEscritores[id_nodo-1];
         sem_post(sem_servidosEscritores);

         sem_wait(sem_numNodLec);
         testigo.numNodLec = (*numNodLec);
         sem_post(sem_numNodLec);


         if(tipoReq==1){
      
            testigo.mtype = id_nodoReq;
            msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);
            sem_wait(sem_hasToken);
            *hasToken = 0;
            sem_post(sem_hasToken);
         }

         if(tipoReq==0){
      
            sem_wait(sem_hasToken);
            if (*hasToken != 0) {
               sem_post(sem_hasToken);
                  testigo.mtype = id_nodoReq;
                  msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);
                  sem_wait(sem_hasToken);
                  *hasToken = 0;
                  sem_post(sem_hasToken);
            }
            else{
               sem_post(sem_hasToken);
            }
         }
      }
      else{
         sem_post(sem_lectorOEscritor);
      }
    
    }
  }
