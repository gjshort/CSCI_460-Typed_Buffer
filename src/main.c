/*****************************************
*
* CSCI 460: Operating Systems
* Montana State University
* Assignment 2: Synchronized Typed Buffer
*
* Author: Griffin Short 
* 
*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>
#include "CDLL.h"           // Circular, Doubly Linked List (written years ago)

// Circular Doubly Linked List pointers
struct Node *buffer_head = NULL;
struct Node *buffer_tail = NULL;

unsigned int BUFFER_SIZE = 50;
unsigned int items_in_buffer = 0;
unsigned int odds_in_buffer = 0;
unsigned int evens_in_buffer = 0;

unsigned int seq = 1;

sem_t odd_sem;
sem_t even_sem;
sem_t space_in_buf;
pthread_mutex_t buffer_lock;
pthread_barrier_t begin_threads;


void produce_odd(void *thread_seed)
{
    pthread_barrier_wait(&begin_threads);

    FILE *log_file = fopen("producer_odd_output.txt", "w");
    if (log_file == NULL)
    {
        return;
    }

    int produced = 0;
    while (produced < 100)
    {
        unsigned int random_num = (rand_r(thread_seed) % 100) + 1; // [1:100]
        // Make sure it's odd
        if (random_num % 2 == 0)
        {
            if (random_num > 0) random_num--;
            else random_num++;
        }

        sem_wait(&space_in_buf);
        pthread_mutex_lock(&buffer_lock);       // Mutex to control buffer pointers

        fprintf(log_file, "[seq=%u] [thread=produce_odd] [action=before_insert] [value=%u] [size=%d] [odd=%u] [even=%u]\n",
                seq, random_num, items_in_buffer, odds_in_buffer, evens_in_buffer);

        addToEnd(random_num, &buffer_head, &buffer_tail);
        odds_in_buffer++;
        items_in_buffer++;
        produced++;
        seq++;

        fprintf(log_file, "[seq=%u] [thread=produce_odd] [action=after_insert]  [value=%u] [size=%d] [odd=%u] [even=%u]\n",
                seq, random_num, items_in_buffer, odds_in_buffer, evens_in_buffer);
        
        seq++;

        pthread_mutex_unlock(&buffer_lock);
        sem_post(&odd_sem);

    }

    fclose(log_file);
    return;
}


void produce_even(void *thread_seed)
{
    pthread_barrier_wait(&begin_threads);

    FILE *log_file = fopen("producer_even_output.txt", "w");
    if (log_file == NULL)
    {
        return;
    }

    int produced = 0;
    while (produced < 100)
    {
        unsigned int random_num = (rand_r(thread_seed) % 100) + 1; // [1:100]
        // Make sure it's even
        if (random_num % 2 == 1)
        {
            random_num--;
        }

        sem_wait(&space_in_buf);
        pthread_mutex_lock(&buffer_lock);   // Mutex to control buffer pointers

        
        fprintf(log_file, "[seq=%u] [thread=produce_even] [action=before_insert] [value=%u] [size=%d] [odd=%u] [even=%u]\n",
                seq, random_num, items_in_buffer, odds_in_buffer, evens_in_buffer);

        addToEnd(random_num, &buffer_head, &buffer_tail);
        evens_in_buffer++;
        items_in_buffer++;
        produced++;
        seq++;

        fprintf(log_file, "[seq=%u] [thread=produce_even] [action=after_insert]  [value=%u] [size=%d] [odd=%u] [even=%u]\n",
                seq, random_num, items_in_buffer, odds_in_buffer, evens_in_buffer);

        seq++;

        pthread_mutex_unlock(&buffer_lock);
        sem_post(&even_sem);
        
    }

    fclose(log_file);
    return;
}


void consume_odd()
{
    pthread_barrier_wait(&begin_threads);

    FILE *log_file = fopen("consumer_odd_output.txt", "w");
    if (log_file == NULL)
    {
        return;
    }

    int consumed = 0;
    while (consumed < 100)
    {
        sem_wait(&odd_sem);
        pthread_mutex_lock(&buffer_lock);

        // Search buffer for odd value
        int list_size = getSize(&buffer_head, &buffer_tail);
        int buffer_val;
        for (int i = 1; i <= list_size; i++)
        {
            // Check if index has an odd value. If so, remove it from list, break loop.
            if (at(i, &buffer_val, &buffer_head, &buffer_tail) == 1 && (buffer_val % 2 != 0))
            {
                fprintf(log_file, "[seq=%u] [thread=consume_odd] [action=before_remove] [value=%u] [size=%d] [odd=%u] [even=%u]\n",
                        seq, buffer_val, items_in_buffer, odds_in_buffer, evens_in_buffer);

                removeSlot(i, &buffer_head, &buffer_tail);
                break;
            }
        }

        odds_in_buffer--;
        items_in_buffer--;
        consumed++;
        seq++;

        fprintf(log_file, "[seq=%u] [thread=consume_odd] [action=after_remove]  [value=%u] [size=%d] [odd=%u] [even=%u]\n",
                seq, buffer_val, items_in_buffer, odds_in_buffer, evens_in_buffer);

        seq++;

        pthread_mutex_unlock(&buffer_lock);
        sem_post(&space_in_buf);

    }

    fclose(log_file);
    return;
}


void consume_even()
{
    pthread_barrier_wait(&begin_threads);

    FILE *log_file = fopen("consumer_even_output.txt", "w");
    if (log_file == NULL)
    {
        return;
    }

    int consumed = 0;
    while (consumed < 100)
    {
        sem_wait(&even_sem);
        pthread_mutex_lock(&buffer_lock);

        // Search buffer for even value
        int list_size = getSize(&buffer_head, &buffer_tail);
        int buffer_val;
        for (int i = 1; i <= list_size; i++)
        {
            // Check if index has an even value. If so, remove it from list, break loop.
            if (at(i, &buffer_val, &buffer_head, &buffer_tail) == 1 && (buffer_val % 2 == 0))
            {
                fprintf(log_file, "[seq=%u] [thread=consume_even] [action=before_remove] [value=%u] [size=%d] [odd=%u] [even=%u]\n",
                        seq, buffer_val, items_in_buffer, odds_in_buffer, evens_in_buffer);

                removeSlot(i, &buffer_head, &buffer_tail);
                break;
            }
        }

        evens_in_buffer--;
        items_in_buffer--;
        consumed++;
        seq++;
        
        fprintf(log_file, "[seq=%u] [thread=consume_even] [action=after_remove]  [value=%u] [size=%d] [odd=%u] [even=%u]\n",
                seq, buffer_val, items_in_buffer, odds_in_buffer, evens_in_buffer);

        seq++;

        pthread_mutex_unlock(&buffer_lock);
        sem_post(&space_in_buf);

    }

    fclose(log_file);
    return;
}


int main(int argc, char *argv[])
{
    // Pull in optional seed
    unsigned int odd_producer_seed = 42;
    if (argc == 3) 
    {   
        if (strncmp(argv[1], "-s", 2) == 0)
        {
            odd_producer_seed = atoi(argv[2]);
        }
        else
        {
            printf("Incorrect args. Use ' -s <seed> '\n");
            return 0;
        }
    }
    unsigned int even_producer_seed = odd_producer_seed + 1;

    // Inter-thread locks
    sem_init(&odd_sem, 0, 0);
    sem_init(&even_sem, 0, 0);
    sem_init(&space_in_buf, 0, 50);
    pthread_mutex_init(&buffer_lock, NULL);

    // 4 worker threads
    pthread_t even_producer;
    pthread_t odd_producer;
    pthread_t even_consumer;
    pthread_t odd_consumer;

    pthread_barrier_init(&begin_threads, NULL, 4); // All 4 threads must launch

    // No real error checking on thread creation. Just using the barrier for synchronization
    pthread_create(&even_producer, NULL, (void(*))&produce_even, (void *)&even_producer_seed);
    pthread_create(&odd_producer, NULL, (void(*))&produce_odd, (void *)&odd_producer_seed);
    pthread_create(&even_consumer, NULL, (void(*))&consume_even, NULL);
    pthread_create(&odd_consumer, NULL, (void(*))&consume_odd, NULL);

    pthread_join(odd_consumer, NULL);    
    pthread_join(even_consumer, NULL);
    pthread_join(odd_producer, NULL);
    pthread_join(even_producer, NULL);

    printf("Wasted wakeups: 0 (semaphores w/o condition checking)\n");
    pthread_barrier_destroy(&begin_threads);
    pthread_mutex_destroy(&buffer_lock);
    sem_destroy(&even_sem);
    sem_destroy(&odd_sem);
    destroyList(&buffer_head, &buffer_tail);
    return 0;
}