#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t semF, semC;

void* p1(void* arg){
    sem_wait(&semF); 
    printf("F\n");
    sem_post(&semC); 
    printf("E\n");
    printf("G\n");

    return NULL;
}

void* p2(void* arg){
    printf("A\n");
    sem_post(&semF); 
    sem_wait(&semC);
    printf("C\n");
    printf("B\n");

    return NULL;
}

int main(){
    sem_init(&semF, 0, 0);
    sem_init(&semC, 0, 0);

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, p1, NULL);
    pthread_create(&thread2, NULL, p2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    sem_destroy(&semF);
    sem_destroy(&semC);

    return 0;
}
