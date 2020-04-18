#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "logging.h"
#include "types.h"

void* calls(void* ptr) 
{ 
    logOP(IWANT,1,2,3);
    // sleep(2);
    logOP(RECVD,1,2,3);
    // sleep(2);
    logOP(ENTER,1,2,3);
    // sleep(2);
    logOP(IAMIN,1,2,3);
    // sleep(2);
    logOP(TIMUP,1,2,3);
    // sleep(2);
    logOP(TLATE,1,2,3);
    // sleep(2);
    logOP(CLOSD,1,2,3);
    // sleep(2);
    logOP(FAILD,1,2,3);
    // sleep(2);
    logOP(GAVUP,1,2,3);

    pthread_exit(NULL); 
    return NULL; 
} 

int main(void){
    int i; 
    pthread_t tid; 
  
    for (i = 0; i < 10; i++) 
        pthread_create(&tid, NULL, calls, NULL); 
  
    pthread_exit(NULL); 
    
    return OK;
}