#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h> 
#include "utils.h"
#include "types.h"
#include "logging.h"

int server_fd;

void * handle_request(void *arg){

    message * msg = (message *) arg;
    char fifoName[FIFONAME_SIZE + 50];
    sprintf(fifoName,"/tmp/%d.%lu",msg->pid,msg->tid);

    int fd;
    if((fd = open(fifoName,O_WRONLY)) == -1){ // Opening Client FIFO
        fprintf(stderr,"Error opening '%s' in WRITEONLY mode.\n",fifoName);
        return NULL;
    }

    msg->pid = getppid();
    msg->tid = pthread_self();

    if(write(fd, msg, sizeof(message)) == -1){ // Writing to Client FIFO
        fprintf(stderr,"Error writing response to Client.\n");
        return NULL;
    }

    close(fd);

    free(arg);

    unsigned int remainingTime = msg->dur;
    usleep(remainingTime); // Just consuming the time

    return NULL;
}

void * refuse_request(void *arg){

    message msg = *(message *) arg;
    
    char fifoName[FIFONAME_SIZE + 50];
    sprintf(fifoName,"/tmp/%d.%lu",msg.pid,msg.tid);
    
    int fd;
    if((fd = open(fifoName,O_WRONLY)) == -1){
        fprintf(stderr,"Error opening '%s' in WRITEONLY mode.\n",fifoName);
        free(arg);
        return NULL;
    }

    msg.pid = getppid();
    msg.tid = pthread_self();
    msg.pl = -1;
    msg.dur = -1;

    if(write(fd, &msg, sizeof(message)) == -1){
        fprintf(stderr,"Error writing response to Client.\n");
        free(arg);
        return NULL;
    }

    close(fd);

    free(arg);

    return NULL;
}

void * server_closing(void * arg){

    int threadNum = *(int *) arg;
    int limit = MAX_THREADS - threadNum; // Calculating limit available
    pthread_t threads[limit]; 

    threadNum = 0; // Restart Counting

    while(1){
       
        if(threadNum == limit){
            fprintf(stderr,"MAX THREADS exceeded...\n");
            break;
        }

        message * msg = (message *) malloc(sizeof(message));
        
        int r;
        if((r = read(server_fd,msg, sizeof(message))) < 0){
            if(isNotNonBlockingError() == OK){
                free(msg);
                break;
            }
            else{
                free(msg);
                continue;
            }
        }
        else if(r == 0){
            free(msg);
            break;
        }

        printMsg(msg);

        pthread_create(&threads[threadNum], NULL, refuse_request, msg);
        threadNum++;
    }

    // Wait for the threads that will inform clients that made requests when server was closing
    for(int i = 0; i < threadNum; i++){
        pthread_join(threads[i],NULL);
    }

    return NULL;
}

int main(int argc, char * argv[]){

    args a;

    if (checkArgs(argc, argv, &a, Q) != OK ){
        fprintf(stderr,"Usage: %s <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n",argv[0]);
        exit(ERROR);
    }
       
    char fifoName[FIFONAME_SIZE + 50];
    sprintf(fifoName,"/tmp/%s",a.fifoName);

    int placeNum = 0, threadNum = 0;
    pthread_t threads[MAX_THREADS], timechecker;

    // Create FIFO to receive client's requests
    if(mkfifo(fifoName,0660) < 0){
        if (errno == EEXIST){
            printf("FIFO '%s' already exists.\n",fifoName);
        }
        else {
            fprintf(stderr, "Could not create FIFO '%s'.\n",fifoName); 
            exit(ERROR);
        }
    }

    // Open FIFO in READ ONLY mode to read client's requests
    if((server_fd = open(fifoName,O_RDONLY|O_NONBLOCK)) == -1){
        fprintf(stderr,"Error opening '%s' in READONLY mode.\n",fifoName);
        if(unlink(fifoName) < 0){
            fprintf(stderr, "Error when destroying '%s'.\n",fifoName);
            exit(ERROR);
        }
        exit(ERROR);
    }

    // Don't block FIFO if no requests are ready to read
    setNonBlockingFifo(server_fd);


    int terminated[] = {a.nsecs, 0}; // Second element will be 1 if time is over, creating no more threads on main thread
    
    // Creating the synchronous thread that just checks if the time ended
    // Notifying the main thread with the @p terminated variable
    pthread_create(&timechecker, NULL, timeChecker, (void *) terminated);
    pthread_detach(timechecker);

    // Receive client's requests during defined time interval
    while(!terminated[1]){
        
        if(threadNum == MAX_THREADS){
            fprintf(stderr,"MAX THREADS exceeded...\n");
            break;
        }

        message * msg = (message *) malloc(sizeof(message));
        
        int r;
        // Read message from client if it exists (without blocking)
        if((r = read(server_fd, msg, sizeof(message))) < 0){
            if(isNotNonBlockingError() == OK){
                free(msg);
                break;
            }
            else{
                free(msg);
                continue;
            }
        }
        else if(r == 0){ // EOF
            free(msg);
            continue;
        }
        
        // Set client's place number
        msg->pl = placeNum;
        placeNum++;

        printMsg(msg);
        
        // Create thread to handle the request of the client
        if(pthread_create(&threads[threadNum], NULL, handle_request, msg) != OK){
            free(msg);
            fprintf(stderr,"Error creating thread.\n");
            break;
        }

        threadNum++;        
    }

    printf("Time is over... Threads will be joined.\n");

    // Create thread to handle requests while server is closing
    pthread_create(&threads[threadNum], NULL, server_closing, &threadNum);

    // Wait for all threads to finish except the ones thrown when server was already closing
    for(int i = 0; i < threadNum; i++){
        pthread_join(threads[i],NULL);
    }
    
    // Wait for the thread that is handling the requests sent when the server was closing
    pthread_join(threads[threadNum],NULL);

    // Close the server and stop receiving requests
    if(unlink(fifoName) < 0){
        fprintf(stderr, "Error when destroying '%s'.\n",fifoName);
        exit(ERROR);
    }

    printf("FIFO '%s' destroyed.\n",fifoName);
    
    close(server_fd);

    pthread_exit(OK);
}