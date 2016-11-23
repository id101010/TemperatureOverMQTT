#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

static void *Thread1(void *pdata){
    while(1){
        printf("Piiiiing\n");
        fflush(stdout);
        sleep(1);
    }
    pthread_exit(NULL);
}

static void *Thread2(void *pdata){
    while(1){
        printf("Poooong\n");
        fflush(stdout);
        sleep(1);
    }
    pthread_exit(NULL);
}

int32_t main(void){

    pthread_t Thread1ID, Thread2ID;

    /* Create all Threads */
    pthread_create(&Thread1ID, NULL, Thread1, NULL);
    pthread_create(&Thread2ID, NULL, Thread2, NULL);

    /* Wait 10s */
    sleep(10);

    pthread_cancel(Thread1ID);
    pthread_join(Thread1ID, NULL);
    pthread_cancel(Thread2ID);
    pthread_join(Thread2ID, NULL);

    exit(EXIT_SUCCESS);
}
