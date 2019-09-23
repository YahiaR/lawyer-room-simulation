
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <windows.h>

#define NUM_CLIENTES 6
#define CAPACIDAD_SALA 3
#define TIEMPO_SEGUNDOS 3

HANDLE sem_escritura;
HANDLE sem_salaEspera;
HANDLE sem_abogado;


/*typedef struct
{
    int id;
}TPARAMHILO;*/

DWORD funcionHilo(LPVOID ptr)
{
    int id2 = *((int*)ptr);
    WaitForSingleObject(sem_escritura, INFINITE);
    printf("\nEl cliente %d llega al bufete.\n",id2);
    ReleaseSemaphore(sem_escritura, 1, NULL );
       
    WaitForSingleObject(sem_salaEspera, INFINITE);
    
    WaitForSingleObject(sem_escritura, INFINITE);
    printf("\nEl cliente %d entra en la sala de espera.\n",id2);
    ReleaseSemaphore(sem_escritura, 1, NULL );
    
    WaitForSingleObject(sem_abogado, INFINITE);
    
    ReleaseSemaphore(sem_salaEspera, 1, NULL );
    
    WaitForSingleObject(sem_escritura, INFINITE);
    printf("\nEl cliente %d entra en despacho.\n",id2);
    ReleaseSemaphore(sem_escritura, 1, NULL );
    
    Sleep(TIEMPO_SEGUNDOS * 1000);
    
    ReleaseSemaphore(sem_abogado, 1, NULL );
    
    WaitForSingleObject(sem_escritura, INFINITE);
    printf("\nEl cliente %d sale del despacho.\n",id2);
    ReleaseSemaphore(sem_escritura, 1, NULL );  
   
    return (EXIT_SUCCESS);
}

int main(int argc, char** argv) 
{
    int i = 0;
    int id[NUM_CLIENTES];
    HANDLE thread[NUM_CLIENTES];
    DWORD wait, close;
    //TPARAMHILO paramHilo[NUM_CLIENTES];
    
    // Creacion de los Semaforos
    sem_escritura = CreateSemaphore(NULL, 1, 1, NULL);
    if(sem_escritura == NULL)
    {
        printf("ERROR %d EN LA CREACION DEL SEMAPHORE.\n",GetLastError());
        return(EXIT_FAILURE);
    }
       
    sem_abogado = CreateSemaphore(NULL, 1, 1, NULL);//(NULL,Valor inicial del semaforo,Valor maximo del semaforo, NULL)
    if(sem_abogado == NULL)
    {
        printf("ERROR %d EN LA CREACION DEL SEMAPHORE.\n",GetLastError());
        return(EXIT_FAILURE);
    }     
    
    sem_salaEspera = CreateSemaphore(NULL, CAPACIDAD_SALA, CAPACIDAD_SALA, NULL);//(NULL,Valor inicial del semaforo,Valor maximo del semaforo, NULL)
    if(sem_salaEspera == NULL)
    {
        printf("ERROR %d EN LA CREACION DEL SEMAPHORE.\n",GetLastError());
        return(EXIT_FAILURE);
    }     
       
    // Creacion de los Hilos
    for(i=0;i<NUM_CLIENTES;i++)
    {
        //paramHilo[i].id = i;
        id[i] = i;
        
        thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)funcionHilo, &id[i], 0, NULL);
        if(thread[i]==NULL)
        {
            printf("\nERROR %d EN LA CREACION DEL HILO %d.\n",GetLastError(),i);
            return(EXIT_FAILURE);
        }
    }   
    
    // Espera de los Hilos
    wait = WaitForMultipleObjects(NUM_CLIENTES, thread, TRUE, INFINITE);//(Numero de HANDLES del Vector, Vector de los HANDLES que se quiere esperar, TRUE=TERMINA CUANDO TERMINAN TODOS, Tiempo de espera) 
    if(wait == WAIT_FAILED)
    {
        printf("\nERROR %d EN LA ESPERA DEL HILO.\nLa funcion de espera a devuelto el valor WAIT_FAILED.\nERROR: %d\n",GetLastError());
        return(EXIT_FAILURE);
    }
    
    printf("\n\nTODOS LOS CLIENTES HAN SIDO ATENDIDOS.\n\n");
    
    // Cierre de los Hilos
    for(i=0;i<NUM_CLIENTES;i++)
    {
        CloseHandle(thread[i]);
        if(close == FALSE)
        {
            printf("ERROR %d EN EL CIERRE DEL HILO %d\n", GetLastError(),i);
            return EXIT_FAILURE;
        }
    }
    
    // Cierre de semaforos
    CloseHandle(sem_salaEspera);
    CloseHandle(sem_escritura);
    CloseHandle(sem_abogado);
    return (EXIT_SUCCESS);
}

