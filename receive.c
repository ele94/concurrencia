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
  int num;
}aviso;

struct token{
  long mtype;
  int servidosEscritores[5];
  int servidosLectores[5];
  int numNodLec;
}testigo;

int fin = 0;



void sendToken(int id_nodoReq, int tipoReq, int numReq);



void thread_receive(void *ptr){
  thdata *data = ptr;

  request peticion;

  while(fin == 0){

   printf("Esperando por peticiones del nodo %d\n",data->numNodo);
    //buzon
    int msgflg;
    int msqid = data->msqid;
    printf("Esperando por peticiones de la cola con msqid %d\n",msqid);

    size_t msgsz = 20;
    long msgtyp = 0;
    
    //esperamos a recibir una peticion de algun nodo
    int p = msgrcv(msqid, (struct msgbuf *)&peticion,sizeof(peticion),id_nodo,0);
    if(p<0) {
      printf("Error al recibir el mensaje\n");
      printf("Saliendo del programa\n");
      return;
    }
    printf("Peiticion recibida en la cola %d! del nodo %d\n",msqid,peticion.myID);
    printf("Peticion del nodo %d de tipo %d\n",data->numNodo,peticion.lectorOEscritor);
    
    sem_wait(sem_receive);
    //si recibimos una peticion de un escritor
    if(peticion.lectorOEscritor == 1){
      sem_wait(sem_peticionesEscritores);
      if(peticion.myNum > peticionesEscritores[peticion.myID-1]){
        peticionesEscritores[peticion.myID-1]=peticion.myNum;
        printf("Actualizado el numero de peticion de escritores a %d \n", peticion.myNum);
        printf("Escritores pedidos: %d %d %d %d %d\n",peticionesEscritores[0],peticionesEscritores[1],peticionesEscritores[2],peticionesEscritores[3],peticionesEscritores[4]);
        printf("Escritores servidos: %d %d %d %d %d\n",servidosEscritores[0],servidosEscritores[1],servidosEscritores[2],servidosEscritores[3],servidosEscritores[4]);
      }
      sem_post(sem_peticionesEscritores);

      sem_wait(sem_hasToken);
      sem_wait(sem_inSC);
      if((*hasToken == 1)&&(*inSC == 0)) { 
         sem_post(sem_inSC);
         sem_post(sem_hasToken);
         printf("Entrando en el sendToken 1 para ver si se puede mandar el testigo a %d\n",peticion.myID);
         sendToken(peticion.myID,peticion.lectorOEscritor,peticion.myNum);
      }
      else{
         sem_post(sem_inSC);
         sem_post(sem_hasToken);
         printf("Hay un proceso en la SC asi que no se puede mandar el testigo! Lo siento!\n");
      }
    }
    
    //si recibimos una peticion de un lector
    else if(peticion.lectorOEscritor == 0){
      sem_wait(sem_peticionesLectores);
      if(peticion.myNum > peticionesLectores[peticion.myID-1]){
        peticionesLectores[peticion.myID-1]=peticion.myNum;
        printf("Actualizado el numero de peticion de lectores a %d \n", peticion.myNum);
      }
      sem_post(sem_peticionesLectores);

      sem_wait(sem_hasToken);
      if(*hasToken == 1){
        printf("Tengo el token\n");
         sem_post(sem_hasToken);
         if(*lectorOEscritor == 0){
            printf("Soy un proceso lector\n");
            sem_wait(sem_esperandoAviso);
            if(*esperandoAviso == 1) {
            sem_post(sem_esperandoAviso);
            printf("Mandando aviso para que el lector envie el token\n");
            aviso.mtype = id_nodo;
            aviso.num = 0;
             msgsnd(cola_warning, (struct msgbuf *) &aviso, sizeof(aviso), 0);
            }
            else{
              printf("No hay avisos esperando a ser atendidos\n");
              sem_post(sem_esperandoAviso);
              printf("Entrando en el sendToken 2 para ver si se puede mandar el testigo a %d\n",peticion.myID);
              sendToken(peticion.myID,peticion.lectorOEscritor,peticion.myNum);
            }  
	     }
	     else{ 
        printf("Soy un proces escritor\n");
         sem_wait(sem_inSC);
         if(*inSC == 0) {
            printf("No estoy en la SC\n");
            sem_post(sem_inSC);
            printf("Entrando en el sendToken 3 para ve rsi se puede mandar el testigo a %d\n",peticion.myID);
            sendToken(peticion.myID,peticion.lectorOEscritor,peticion.myNum);
         }
         else{
            sem_post(sem_inSC);
         }
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

  printf("Tu id es %d \n",id_nodo);
  
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

  //Colas del testigo y del aviso de peticion de testigo
  id = 17;
  key = ftok(path,id);
  cola_token = msgget(key, shmflg);
  if(cola_token == 0){
    printf("Error! maldita cola token! Saliendo del proceso...\n");
    return 0;
  }

  id = 150 * id_nodo;
  key = ftok(path,id);
  cola_warning = msgget(key, shmflg);
  printf("Cola warning: %d\n",cola_warning);

  
  //Sección internodo
  // ??? key_t peticion_key;


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
  sem_peticionesLectores = sem_open(SEM_PETLEC1, O_CREAT, 0644, 1);
  //Semáforo peticionesEscritores
  sem_peticionesEscritores = sem_open(SEM_PETESC1, O_CREAT, 0644, 1);
  //Semáforo servidosLectores
  sem_servidosLectores = sem_open(SEM_SERVLEC1, O_CREAT, 0644, 1);
  //Semáforo servidosEscritores
  sem_servidosEscritores = sem_open(SEM_SERVESC1, O_CREAT, 0644, 1);
  //Semáforo numNodLec
  sem_numNodLec = sem_open(SEM_NUMNODLEC1, O_CREAT, 0644, 1);
  //Semáforo hasToken
  sem_hasToken = sem_open(SEM_HASTOKEN1, O_CREAT, 0644, 1);
  //Semáforo inSC
  sem_inSC= sem_open(SEM_INSC1, O_CREAT, 0644, 1);
  //Semáforo lectorOEscritor
  sem_lectorOEscritor = sem_open(SEM_LECESC1, O_CREAT, 0644, 1);
  //Semáforo espernadoAviso
  sem_esperandoAviso = sem_open(SEM_AVISO1, O_CREAT, 0644, 1);
  //Semaforo receive
  sem_receive = sem_open(SEM_RECEIVE1, O_CREAT, 0644, 1);
}
  if(id_nodo==2){
//SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
  //Semáforo peticionesLectores
  sem_peticionesLectores = sem_open(SEM_PETLEC2, O_CREAT, 0644, 1);
  //Semáforo peticionesEscritores
  sem_peticionesEscritores = sem_open(SEM_PETESC2, O_CREAT, 0644, 1);
  //Semáforo servidosLectores
  sem_servidosLectores = sem_open(SEM_SERVLEC2, O_CREAT, 0644, 1);
  //Semáforo servidosEscritores
  sem_servidosEscritores = sem_open(SEM_SERVESC2, O_CREAT, 0644, 1);
  //Semáforo numNodLec
  sem_numNodLec = sem_open(SEM_NUMNODLEC2, O_CREAT, 0644, 1);
  //Semáforo hasToken
  sem_hasToken = sem_open(SEM_HASTOKEN2, O_CREAT, 0644, 1);
  //Semáforo inSC
  sem_inSC= sem_open(SEM_INSC2, O_CREAT, 0644, 1);
  //Semáforo lectorOEscritor
  sem_lectorOEscritor = sem_open(SEM_LECESC2, O_CREAT, 0644, 1);
  //Semáforo espernadoAviso
  sem_esperandoAviso = sem_open(SEM_AVISO2, O_CREAT, 0644, 1);
  //Semaforo receive
  sem_receive = sem_open(SEM_RECEIVE2, O_CREAT, 0644, 1);
  }
  if(id_nodo==3){
//SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
  //Semáforo peticionesLectores
  sem_peticionesLectores = sem_open(SEM_PETLEC3, O_CREAT, 0644, 1);
  //Semáforo peticionesEscritores
  sem_peticionesEscritores = sem_open(SEM_PETESC3, O_CREAT, 0644, 1);
  //Semáforo servidosLectores
  sem_servidosLectores = sem_open(SEM_SERVLEC3, O_CREAT, 0644, 1);
  //Semáforo servidosEscritores
  sem_servidosEscritores = sem_open(SEM_SERVESC3, O_CREAT, 0644, 1);
  //Semáforo numNodLec
  sem_numNodLec = sem_open(SEM_NUMNODLEC3, O_CREAT, 0644, 1);
  //Semáforo hasToken
  sem_hasToken = sem_open(SEM_HASTOKEN3, O_CREAT, 0644, 1);
  //Semáforo inSC
  sem_inSC= sem_open(SEM_INSC3, O_CREAT, 0644, 1);
  //Semáforo lectorOEscritor
  sem_lectorOEscritor = sem_open(SEM_LECESC3, O_CREAT, 0644, 1);
  //Semáforo espernadoAviso
  sem_esperandoAviso = sem_open(SEM_AVISO3, O_CREAT, 0644, 1);
  //Semaforo receive
  sem_receive = sem_open(SEM_RECEIVE3, O_CREAT, 0644, 1);
  }
  if(id_nodo==4){
//SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
  //Semáforo peticionesLectores
  sem_peticionesLectores = sem_open(SEM_PETLEC4, O_CREAT, 0644, 1);
  //Semáforo peticionesEscritores
  sem_peticionesEscritores = sem_open(SEM_PETESC4, O_CREAT, 0644, 1);
  //Semáforo servidosLectores
  sem_servidosLectores = sem_open(SEM_SERVLEC4, O_CREAT, 0644, 1);
  //Semáforo servidosEscritores
  sem_servidosEscritores = sem_open(SEM_SERVESC4, O_CREAT, 0644, 1);
  //Semáforo numNodLec
  sem_numNodLec = sem_open(SEM_NUMNODLEC4, O_CREAT, 0644, 1);
  //Semáforo hasToken
  sem_hasToken = sem_open(SEM_HASTOKEN4, O_CREAT, 0644, 1);
  //Semáforo inSC
  sem_inSC= sem_open(SEM_INSC4, O_CREAT, 0644, 1);
  //Semáforo lectorOEscritor
  sem_lectorOEscritor = sem_open(SEM_LECESC4, O_CREAT, 0644, 1);
  //Semáforo espernadoAviso
  sem_esperandoAviso = sem_open(SEM_AVISO4, O_CREAT, 0644, 1);
  //Semaforo receive
  sem_receive = sem_open(SEM_RECEIVE4, O_CREAT, 0644, 1);
  }
  if(id_nodo==5){
 //SEMAFOROS (los he movido todos aqui para que sean faciles de mover)
  //Semáforo peticionesLectores
  sem_peticionesLectores = sem_open(SEM_PETLEC5, O_CREAT, 0644, 1);
  //Semáforo peticionesEscritores
  sem_peticionesEscritores = sem_open(SEM_PETESC5, O_CREAT, 0644, 1);
  //Semáforo servidosLectores
  sem_servidosLectores = sem_open(SEM_SERVLEC5, O_CREAT, 0644, 1);
  //Semáforo servidosEscritores
  sem_servidosEscritores = sem_open(SEM_SERVESC5, O_CREAT, 0644, 1);
  //Semáforo numNodLec
  sem_numNodLec = sem_open(SEM_NUMNODLEC5, O_CREAT, 0644, 1);
  //Semáforo hasToken
  sem_hasToken = sem_open(SEM_HASTOKEN5, O_CREAT, 0644, 1);
  //Semáforo inSC
  sem_inSC= sem_open(SEM_INSC5, O_CREAT, 0644, 1);
  //Semáforo lectorOEscritor
  sem_lectorOEscritor = sem_open(SEM_LECESC5, O_CREAT, 0644, 1);
  //Semáforo espernadoAviso
  sem_esperandoAviso = sem_open(SEM_AVISO5, O_CREAT, 0644, 1);
  //Semaforo receive
  sem_receive = sem_open(SEM_RECEIVE5, O_CREAT, 0644, 1);   
  }


  //Inicializacion de variables compartidas
  //aqui habria que mete rsemaforos...
  printf("A punto de inicializar token\n");
  if(id_nodo == 1){
    *hasToken = 1;
    printf("Felicidades! Tienes el token %d \n",*hasToken);
  }
  else {
    *hasToken = 0;
    printf("Ooooh lo siento, no te ha tocado el token\n");
  }



  *lectorOEscritor = 0;  //0 lector, 1 escritor
  *esperandoAviso = 0;
  *inSC = 0;
  *numNodLec = 0;
  *myNum = 0;
  int petLec[] = {0,0,0,0,0};
  int petEsc[] = {0,0,0,0,0};
  int servLec[] = {0,0,0,0,0};
  int servEsc[] = {0,0,0,0,0};
  //peticionesLectores = malloc( sizeof(int[5]) );
  //peticionesEscritores = malloc( sizeof(int[5]) );
  //servidosLectores = malloc( sizeof(int[5]) );
  //servidosEscritores = malloc( sizeof(int[5]) );
  memcpy(petLec, peticionesLectores, sizeof(int[5]));
  memcpy(petEsc, peticionesEscritores, sizeof(int[5]));
  memcpy(servLec, servidosLectores, sizeof(int[5]));
  memcpy(servEsc, servidosEscritores, sizeof(int[5]));




  pthread_t thread[4];
  thdata *data;

  data = (thdata *) calloc(4, sizeof(thdata));

  int i; 

  for(i = 0; i<4;i++){

	data[i].numNodo = id_nodos[i];
  data[i].msqid = msqid_colas[i];

	pthread_create(&(data[i].id_hilo), NULL,(void *)&thread_receive,(void *)&data[i]);
  printf("Creado el thread %i\n", i);
  }
  
  for (i=0; i<4; i++)
    pthread_join(data[i].id_hilo,NULL);


  sem_destroy(sem_peticionesLectores);
  sem_destroy(sem_peticionesEscritores);
  sem_destroy(sem_servidosLectores);
  sem_destroy(sem_servidosEscritores);
  sem_destroy(sem_lectorOEscritor);
  sem_destroy(sem_inSC);
  sem_destroy(sem_hasToken);
  sem_destroy(sem_numNodLec);
  
}






void sendToken(int id_nodoReq, int tipoReq, int numReq){
  printf("Hola! Soy el sendToken! Yo le atendere en el proceso de enviar el token a otro nodo\n");

sem_wait(sem_lectorOEscritor);
if(*lectorOEscritor==0){
    sem_post(sem_lectorOEscritor);
 printf("Soy un nodo lector y he recibido una peticion\n");
printf("Actualizando peticiones servidas\n");
    sem_wait(sem_servidosLectores);
     memcpy(testigo.servidosLectores, servidosLectores, sizeof(int[5]));
    //testigo.servidosLectores[id_nodo-1] = servidosLectores[id_nodo-1];
    sem_post(sem_servidosLectores);

    sem_wait(sem_servidosEscritores);
     memcpy(testigo.servidosEscritores, servidosEscritores, sizeof(int[5]));
    //testigo.servidosEscritores[id_nodo-1] = servidosEscritores[id_nodo-1];
    sem_post(sem_servidosEscritores);

        sem_wait(sem_numNodLec);
        testigo.numNodLec = (*numNodLec);
        sem_post(sem_numNodLec);
    
      
      if(tipoReq==1){

        sem_wait(sem_numNodLec);
        if((*numNodLec) == 0){
          sem_post(sem_numNodLec);
          printf("Actualizando datos de peticiones y servidos al num peticion %d\n",numReq);


         sem_wait(sem_servidosEscritores);
          servidosEscritores[id_nodoReq-1] = numReq;
         testigo.servidosEscritores[id_nodoReq-1] = servidosEscritores[id_nodoReq-1];
        sem_post(sem_servidosEscritores);
    
        sem_wait(sem_numNodLec);
        testigo.numNodLec = (*numNodLec);
        sem_post(sem_numNodLec);
        
          testigo.mtype = id_nodoReq;
          printf("Proceso sendtoken mandando el testigo en la cola %d al ID %d\n ! Buen viaje!",cola_token,id_nodoReq);
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

          sem_wait(sem_servidosLectores);
          servidosLectores[id_nodoReq-1] = numReq;
          testigo.servidosLectores[id_nodoReq-1] = servidosLectores[id_nodoReq-1];
          sem_post(sem_servidosLectores);

          testigo.mtype = id_nodoReq;
                    printf("Proceso sendtoken mandando el testigo en la cola %d al ID %d\n ! Buen viaje!",cola_token,id_nodoReq);
          msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);

          sem_wait(sem_hasToken); //repasar este semáforo
          *hasToken = 0;
          sem_post(sem_hasToken); //repasar este semáforo
        } else {
          sem_post(sem_hasToken);
        }
      }
    }

   else{
      sem_post(sem_lectorOEscritor);
      sem_wait(sem_lectorOEscritor);
      if(*lectorOEscritor==1){
        sem_post(sem_lectorOEscritor);
        printf("Soy un nodo escritor y he recibido una peticion!\n");
        printf("Actualizando peticiones servidas\n");
    sem_wait(sem_servidosLectores);
     memcpy(testigo.servidosLectores, servidosLectores, sizeof(int[5]));
    //testigo.servidosLectores[id_nodo-1] = servidosLectores[id_nodo-1];
    sem_post(sem_servidosLectores);

    sem_wait(sem_servidosEscritores);
     memcpy(testigo.servidosEscritores, servidosEscritores, sizeof(int[5]));
    //testigo.servidosEscritores[id_nodo-1] = servidosEscritores[id_nodo-1];
    sem_post(sem_servidosEscritores);

         sem_wait(sem_numNodLec);
         testigo.numNodLec = (*numNodLec);
         sem_post(sem_numNodLec);


         if(tipoReq==1){

          sem_wait(sem_servidosEscritores);
          servidosEscritores[id_nodoReq-1] = numReq;
         testigo.servidosEscritores[id_nodoReq-1] = servidosEscritores[id_nodoReq-1];
         sem_post(sem_servidosEscritores);
      
            testigo.mtype = id_nodoReq;
                      printf("Proceso sendtoken mandando el testigo en la cola %d al ID %d\n ! Buen viaje!",cola_token,id_nodoReq);

            msgsnd(cola_token, (struct msgbuf *) &testigo, sizeof(testigo), 0);
            sem_wait(sem_hasToken);
            *hasToken = 0;
            sem_post(sem_hasToken);
         }

         if(tipoReq==0){
      
            sem_wait(sem_hasToken);
            if (*hasToken != 0) {
               sem_post(sem_hasToken);

              sem_wait(sem_servidosLectores);
              servidosLectores[id_nodoReq-1] = numReq;
             testigo.servidosLectores[id_nodoReq-1] = servidosLectores[id_nodoReq-1];
             sem_post(sem_servidosLectores);
                  testigo.mtype = id_nodoReq;
                            printf("Proceso sendtoken mandando el testigo en la cola %d al ID %d\n ! Buen viaje!",cola_token,id_nodoReq);

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
