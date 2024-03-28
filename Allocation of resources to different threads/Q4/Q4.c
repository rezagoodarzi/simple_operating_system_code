#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUMBER_OF_RESOURCES 5
#define NUMBER_OF_THREADS 10

typedef struct {
    int resources[NUMBER_OF_RESOURCES];
    sem_t availableResources;
    pthread_mutex_t poolMutex;
} ResourceManager;

ResourceManager resourceManager;

void* threadWork(void* arg) {
    int threadId = *(int*)arg;
    int assignedResource = -1;
    while (1) {
        sem_wait(&resourceManager.availableResources);
        pthread_mutex_lock(&resourceManager.poolMutex);

        for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
            if (resourceManager.resources[i] < 0) {
                resourceManager.resources[i] = threadId;
                assignedResource = i;
                break;
            }
        }

        pthread_mutex_unlock(&resourceManager.poolMutex);

        if (assignedResource >= 0) {
            
            printf("Thread %d is performing work with resource %d\n", threadId, assignedResource);
            usleep(3000000); // Simulate work with a sleep of 3 seconds

            pthread_mutex_lock(&resourceManager.poolMutex);
            resourceManager.resources[assignedResource] = -1;
            pthread_mutex_unlock(&resourceManager.poolMutex);

            sem_post(&resourceManager.availableResources);

            assignedResource = -1; // Reset assignedResource for the next iteration
        }
    }
    
    return NULL;
}
int main() {
    sem_init(&resourceManager.availableResources, 0, NUMBER_OF_RESOURCES);
    pthread_mutex_init(&resourceManager.poolMutex, NULL);

    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        resourceManager.resources[i] = -1;
    }

    pthread_t threads[NUMBER_OF_THREADS];
    int threadIds[NUMBER_OF_THREADS];

    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        threadIds[i] = i + 1;
        pthread_create(&threads[i], NULL, threadWork, (void*)&threadIds[i]);
    }

    for (size_t i = 0; i < NUMBER_OF_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&resourceManager.availableResources);
    pthread_mutex_destroy(&resourceManager.poolMutex);

    return 0;
}
