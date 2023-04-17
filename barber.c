#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_CUSTOMERS 27

// I defined inputs

int customer_wait = 0;
int customer_finish = 0;
int num_customers;
int chair_number;
int max_arrival_time;
int max_haircut_duration;
int haircut_repetition;

//pthreads
pthread_mutex_t lock;
pthread_cond_t barber_sleeping;
pthread_cond_t customer_waiting;



void* customer(void* arg) {
int* customer_id = (int*) arg;
int haircuts = 0;
while (haircuts < haircut_repetition) {
pthread_mutex_lock(&lock);
if (customer_wait < chair_number) {
customer_wait++;
time_t start_time = time(NULL);
printf("Customer %d is waiting for a haircut...\n", *customer_id);
pthread_cond_signal(&barber_sleeping);
pthread_cond_wait(&customer_waiting, &lock);
time_t end_time = time(NULL);
int waiting_time = (int) difftime(end_time, start_time);
printf("Customer %d got a haircut. Waiting time: %d seconds. Haircuts: %d/%d\n", *customer_id, waiting_time, haircuts, haircut_repetition);
haircuts++;
} else {
printf("Customer %d left because no chairs were available.\n", *customer_id);
}
pthread_mutex_unlock(&lock);
int arrival_time = rand() % max_arrival_time + 1;
sleep(arrival_time);
}
customer_finish++;
pthread_exit(NULL);
}

void* barber(void* arg) {
    while (customer_finish < num_customers) {
        pthread_mutex_lock(&lock);
        while (customer_wait == 0) {
            printf("Barber is sleeping...\n");
            pthread_cond_wait(&barber_sleeping, &lock);
        }
        customer_wait--;
        printf("Barber is cutting hair...\n");
        int haircut_time = rand() % max_haircut_duration + 1;

        sleep(haircut_time);
        printf("Barber finished cutting hair.\n");
        pthread_cond_signal(&customer_waiting);
pthread_mutex_unlock(&lock);
}
pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
if (argc != 6) {
printf("Usage: barber <chair_number> <num_customer> <max_arrival_time> <max_haircut_duration> <haircut_repetition>\n");
return 1;
}
chair_number = atoi(argv[1]);
num_customers = atoi(argv[2]);
max_arrival_time = atoi(argv[3]);
max_haircut_duration = atoi(argv[4]);
haircut_repetition = atoi(argv[5]);
srand(time(NULL));
if (num_customers > MAX_CUSTOMERS) {
printf("Number of customers cannot be greater than %d\n", MAX_CUSTOMERS);
return 1;
}pthread_t barber_thread;
pthread_t customer_threads[num_customers];

pthread_mutex_init(&lock, NULL);
pthread_cond_init(&barber_sleeping, NULL);
pthread_cond_init(&customer_waiting, NULL);

pthread_create(&barber_thread, NULL, barber, NULL);

int customer_ids[num_customers];
for (int i = 0; i < num_customers; i++) {
    customer_ids[i] = i+1;
    pthread_create(&customer_threads[i], NULL, customer, &customer_ids[i]);
}

pthread_join(barber_thread, NULL);
for (int i = 0; i < num_customers; i++) {
pthread_join(customer_threads[i], NULL);
}pthread_mutex_destroy(&lock);
pthread_cond_destroy(&barber_sleeping);
pthread_cond_destroy(&customer_waiting);

return 0;
}