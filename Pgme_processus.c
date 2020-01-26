#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define N 6
typedef struct info
{
    int pid;
    int rang;
    int val;
}info;

 struct sembuf Pmutex={0,-1,0};
 struct sembuf Vmutex={0,1,0};

 struct sembuf Ptj={0,-1,0};
 struct sembuf Vtj={0,1,0};


int mutex,t,memoire2,memoire1;


int main(){
    
    int i,j;
    info element;
    key_t cle;
    /*recuperation des sémaphores et des segments partagés */
    cle=ftok("TP_main.c",1);

    mutex=semget(cle,1,0666);
    if(mutex==-1) {printf("\n Erreur de recuperation du sémaphore mutex"); exit(1);}

    cle=ftok("TP_main.c",2);
    t=semget(cle,N,0666);

    if(t==-1) {printf("\n Erreur de recuperation des sémaphores"); exit(1);}

    cle=ftok("TP_main.c",3);

    memoire1=shmget(cle,2*sizeof(int),0666);
    if(memoire1==-1) {printf("\n Erreur de recuperation de la mémoire partagée"); exit(2);}

    cle=ftok("TP_main.c",4);
    memoire2=shmget(cle,N*sizeof(info),0666);
    if(memmoire2==-1) {printf("\n Erreur de recuperation de la mémoire partagée"); exit(2);}
    /* Attachement des segments partagés */
    int *seg_var= shmat(memoire1,0,0);
    info *Acces_V1= shmat(memoire2,0,0);

    /*Debut du programme*/
    semop(mutex,&Pmutex,1);  ///P(mutex)
        j=seg_var[0];
        seg_var[0]=seg_var[0]+1;
        printf("\nProcessus N: %d de PID = %d\n",j,getpid());
    semop(mutex,&Vmutex,1); ///V(mutex)
    
    
    Ptj.sem_num=j;
    Vtj.sem_num=((j+N-1)%N);// v(hggg)

    semop(t,&Ptj,1);   /// P(t[j])
    
        seg_var[1]=seg_var[1]+j;
        element.pid=getpid();
        element.rang=j;
        element.val=seg_var[1];
        Acces_V1[j]=element;

    semop(t,&Vtj,1);  /// V(t[(j+N-1)%N])
    
    if(j==0){
        printf("Les resultats d'acces a la variable V1 :");
        for (int i = N-1; i >=0; i--){
            printf("\nAcces N : %d , par processus de PID : %d , de rang : %d, v1 = %d :\n",N-i,Acces_V1[i].pid,Acces_V1[i].rang,Acces_V1[i].val);
        }

    }

/* Détachement des segements */
shmdt(Acces_V1); 
shmdt(seg_var);



return 0;
}
