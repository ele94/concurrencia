/************ EJERCICIO 5 *********************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>


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
sem_t  sem_receive;

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
    
    sem_wait(&sem_receive);
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
         if(peticion.lectorOEscritor == 0){
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
    
   sem_post(&sem_receive);
  }
  
}




//Lanzar receive: nº nodo

int main(int argc, char* argv[]){
  
 
  if(argc != 2){
    printf("Error: falta el ID\n");
    return 0;
  }

id_nodo = atoi(argv[1]);
  
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

  //Para compartir peticionesLectores:
  key_t key = 10000 + 1000 * id_nodo;

  int shmid = shmget(key, 5*sizeof(int), IPC_CREAT | 0777);
  int * shm_petlec = (int *) shmat(shmid, NULL, 0);

  peticionesLectores = shm_petlec;

  //Semáforo peticionesLectores
  key = 100000 + 1000 * id_nodo;
  sem_peticionesLectores = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);
  

  //Para compartir peticionesEscritores:
  key = 10000 + 1050 * id_nodo;

  shmid = shmget(key, 5*sizeof(int), IPC_CREAT | 0777);
  int * shm_petesc = (int *) shmat(shmid, NULL, 0);

  peticionesEscritores = shm_petesc;

  //Semáforo peticionesEscritores
  key = 100000 + 1050 * id_nodo;
  sem_peticionesEscritores = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);
  

  //Para compartir servidosLectores
  key = 10000 + 1100 * id_nodo;

  shmid = shmget(key, 5*sizeof(int), IPC_CREAT | 0777);
  int * shm_servlec = (int *) shmat(shmid, NULL, 0);

  servidosLectores = shm_servlec;

  //Semáforo servidosLectores
  key = 100000 + 1100 * id_nodo;
  sem_servidosLectores = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

  //Para compartir servidosEscritores
  key = 10000 + 1150 * id_nodo;

  shmid = shmget(key, 5*sizeof(int), IPC_CREAT | 0777);
  int * shm_servesc = (int *) shmat(shmid, NULL, 0);

  servidosEscritores = shm_servesc;

  //Semáforo servidosEscritores
  key = 100000 + 1150 * id_nodo;
  sem_servidosEscritores = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

  //Para numero de lectores en SC

  key = 1000 + id_nodo;

  shmid = shmget(key, sizeof(int), IPC_CREAT | 0777);
  int * shm1 = (int *) shmat(shmid, NULL, 0);

  numNodLec = shm1;

  //Semáforo numNodLec
  key = 10000 + id_nodo;
  sem_numNodLec = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

  //Para saber si tenemos el testigo

  key = 1100 + id_nodo;
    
  shmid = shmget(key, sizeof(int), IPC_CREAT | 0777);
  int * shm2 = (int *) shmat(shmid, NULL, 0);
    
  hasToken = shm2;

  //Semáforo hasToken
  key = 11000 + id_nodo;
  sem_hasToken = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

  //Para saber el número de peticiones de nuestro nodo

  key = 1200 + id_nodo;

  shmid = shmget(key, sizeof(int), IPC_CREAT | 0777);
  int * shm3 = (int *) shmat(shmid, NULL, 0);

  myNum = shm3;

  /*Semáforo myNum(en principio innecesario)
  key = 12000 + id_nodo;
  sem_t * sem_myNum = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);
  */

  //Para saber si nuestro nodo está en SC

  key = 1300 + id_nodo;

  shmid = shmget(key, sizeof(int), IPC_CREAT | 0777);
  int * shm4 = (int *) shmat(shmid, NULL, 0);

  inSC = shm4;

  //Semáforo inSC
  key = 13000 + id_nodo;
  sem_inSC = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

  //Para saber si el representante es un lector o un escritor

  key = 1400 + id_nodo;

  shmid = shmget(key, sizeof(int), IPC_CREAT | 0777);
  int * shm5 = (int *) shmat(shmid, NULL, 0);

  lectorOEscritor = shm5;

  //Semáforo lectorOEscritor
  key = 14000 + id_nodo;
  sem_lectorOEscritor = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);

  //Para saber si hay peticiones de otros nodos despues de que el ultimo lector salga de la SC

  key = 1500 + id_nodo;

  shmid = shmget(key, sizeof(int), IPC_CREAT | 0777);
  int * shm6 = (int *) shmat(shmid, NULL, 0);

  esperandoAviso = shm6;

  //Semáforo lectorOEscritor
  key = 15000 + id_nodo;
  sem_esperandoAviso = (sem_t *) shmat(shmget(key, sizeof(sem_t), 0777 | IPC_CREAT), NULL, 0);  

  //Colas del testigo y del aviso de peticion de testigo

  key_t token_key = 100;
  cola_token = msgget(token_key, 0777 | IPC_CREAT);

  key_t warning_key = 150 + id_nodo;
  cola_warning = msgget(warning_key, 0777 | IPC_CREAT);


  //Sección internodo

  key_t peticion_key;

  sem_init(&sem_receive,0,1);


  //THREADS

  pthread_t thread[4];
  thdata *data;

  data = (thdata *) calloc(4, sizeof(thdata));

  int i; 

  for(i = 0; i<4;i++){
   peticion_key = id_colas[i];   
   cola_peticion = msgget(peticion_key, 0777 | IPC_CREAT);
	data[i].numNodo = i+1;
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
