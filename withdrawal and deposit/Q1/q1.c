#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CUSTOMERS 5 // number of customers
#define NUM_TRANSACTION 6 // number of transactions/threads
#define INIT_STOCK 100 // the initial value for each person stock

// type transaction. you should pass an object of this type to the function arguments while creating thread
typedef struct
{
    double amount;
    int customer_id;
} transaction_t;

double safeBox = 1000.0; // shared variable for all customers

///////// Start your code here ///////////

/*1. define an array to store the stock of all customers
  2. define the necessary mutexes for each customer account and safe box*/
double arr [NUM_CUSTOMERS + 1];
pthread_mutex_t safebox_mutex [NUM_CUSTOMERS + 1];
pthread_mutex_t account_mutex [NUM_CUSTOMERS + 1];

void* withdraw(void* arg){
    transaction_t* transaction = (transaction_t*)arg;
    double amount = transaction->amount;
    int customer_id = transaction->customer_id;

    pthread_mutex_lock(&account_mutex[customer_id]);

    if (arr[customer_id] >= amount) {
        arr[customer_id] -= amount;
        printf("Customer %d withdrew %.2f from account. New balance: %.2f\n", customer_id, amount, arr[customer_id]);
    } else {
        pthread_mutex_lock(&safebox_mutex[customer_id]);
        double temp = amount - arr[customer_id];
        if (safeBox >= temp) {
            safeBox -= temp;
            arr[customer_id] -= amount;
            printf("Customer %d withdrew %.2f and %.2f from safe-box. New balance: %.2f\tSafe-box value: %.2f\n",
                   customer_id, amount, temp, arr[customer_id], safeBox);
        } else {
            printf("Customer %d has insufficient funds. Withdrawal of %.2f failed. Balance: %.2f\tSafe-box value: %.2f\n",
                   customer_id, amount, arr[customer_id], safeBox);
        }

        pthread_mutex_unlock(&safebox_mutex[customer_id]);
    }

    pthread_mutex_unlock(&account_mutex[customer_id]);

    return NULL;
}


void* deposit(void* arg){

    transaction_t* transaction = (transaction_t*)arg;
    double amount = transaction->amount;
    int customer_id = transaction->customer_id;

    pthread_mutex_lock(&account_mutex[customer_id]);

    if (arr[customer_id] >= 0) {
        arr[customer_id] += amount;
        pthread_mutex_lock(&safebox_mutex[customer_id]);
        printf("Customer %d stock was charged by %.2f. New balance: %.2f\tSafe-box value: %.2f\n",
            customer_id, amount, arr[customer_id], safeBox);
        pthread_mutex_unlock(&safebox_mutex[customer_id]);

    } else {
        pthread_mutex_lock(&safebox_mutex[customer_id]);

        double temp = -arr[customer_id];
        if (amount >= temp){
            safeBox += temp;
            arr[customer_id] += amount - temp;
            printf("Customer %d stock was charged by %.2f. New balance: %.2f\tSafe-box value: %.2f\n",
                customer_id, amount, arr[customer_id], safeBox);
        }else{
            safeBox += temp;
            arr[customer_id] += amount;
            printf("Customer %d stock was charged by %.2f. New balance: %.2f\tSafe-box value: %.2f\n",
                customer_id, amount, arr[customer_id], safeBox);
        }
        pthread_mutex_unlock(&safebox_mutex[customer_id]);
    }

    pthread_mutex_unlock(&account_mutex[customer_id]);

    return NULL;
}

int main(){
    srand(time(NULL));

    for (int i = 1; i <= NUM_CUSTOMERS; i++) {
        arr[i] = INIT_STOCK;
        pthread_mutex_init(&account_mutex[i], NULL);
        pthread_mutex_init(&safebox_mutex[i], NULL);
    }

    pthread_t threads[NUM_TRANSACTION];

    for (int i = 0; i < NUM_TRANSACTION; i++) {
        transaction_t* transaction = (transaction_t*)malloc(sizeof(transaction_t));
        transaction->amount = (rand() % 2000) + 1; 
        transaction->customer_id = (rand() % NUM_CUSTOMERS) + 1; 

        if (rand() % 2 == 0) {
            pthread_create(&threads[i], NULL, withdraw, (void*)transaction);
        } else {
            pthread_create(&threads[i], NULL, deposit, (void*)transaction);
        }
    }

    for (int i = 0; i < NUM_TRANSACTION; ++i) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 1; i <= NUM_CUSTOMERS; ++i) {
        pthread_mutex_destroy(&account_mutex[i]);
        pthread_mutex_destroy(&safebox_mutex[i]);
    }

    return 0;
}
