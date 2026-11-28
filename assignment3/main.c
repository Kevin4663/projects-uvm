#include <stdio.h>
// #include <pthread.h>

// #define QUEUE_SIZE 10
// #define NUM_PRODUCER 2
// #define NUM_WORKER 2
// #define JOBS_PER_PRODUCER 10


// typedef struct {
//     int thread_num;
//     int queue;
// } ThreadArgs;

// void *producer(void* add);
// void *worker(void);

// int main(void){

//     pthread_t producers[NUM_PRODUCER];
//     pthread_t workers[NUM_WORKER];

//     int queue[QUEUE_SIZE];

//    // create producer thread
//    for(int i = 1; i <= NUM_PRODUCER; i++){
//         pthread_create(&producers[i], NULL, producer, (void*) &queue, int i);
//         thread
//    }
    
//     // creater worker thread
//     for(int i = 0; i < NUM_WORKER; i++){
//         pthread_create(&workers[i], NULL, producer, (void*) &queue);
//    }
//     return 0;
// }

// void *producer(void *queue){
//     for(int i = 1; i<= JOBS_PER_PRODUCER; i++){
//         printf("Producer");
//     }
    


//     return NULL;
// }

// void *worker(void){

//     return NULL;
// }

int main(void){
    printf("Hello World");
    return 0;
}