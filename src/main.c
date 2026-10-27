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
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "CDLL.h"           // Circular, Doubly Linked List (written years ago)

#define RED     "\033[0;31m" // Main thread
#define GREEN   "\033[0;32m" // Odd Producer
#define YELLOW  "\033[0;33m" // Even Producer
#define BLUE    "\033[0;34m" // Odd Consumer
#define MAGENTA "\033[0;35m" // Even Consumer
#define RESET   "\033[0m"

int begin_threads = 0; // Global thread launch

// Circular Doubly Linked List pointers
struct Node *buffer_head = NULL;
struct Node *buffer_tail = NULL;

unsigned int BUFFER_SIZE = 50;
unsigned int items_in_buffer = 0;
unsigned int odds_in_buffer = 0;
unsigned int evens_in_buffer = 0;

sem_t odd_sem;
sem_t even_sem;
pthread_mutex_t buffer_lock;

void produce_odd()
{
    // Wait for all threads to be created
    while(begin_threads == 0) {}

    // If a thread failed to start, exit
    if(begin_threads == -1)
    {
        return;
    }

    int produced = 0;
    while (produced < 100)
    {
        sem_wait(&odd_sem);                     // Allows even producer/consumer to work independently
        pthread_mutex_lock(&buffer_lock);       // Mutex to control buffer pointers

        // If there's buffer space, add to the end of it
        if (items_in_buffer < BUFFER_SIZE)
        {
            addToEnd(produced*2 + 1, &buffer_head, &buffer_tail);
            odds_in_buffer++;
            items_in_buffer++;
            produced++;
            printf(GREEN "Produced Odd: %d" RESET "\n", produced);
        }

        pthread_mutex_unlock(&buffer_lock);
        sem_post(&odd_sem);

    }

    return;
}


void produce_even()
{
    // Wait for all threads to be created
    while(begin_threads == 0) {}

    // If a thread failed to start, exit
    if(begin_threads == -1)
    {
        return;
    }

    int produced = 0;
    while (produced < 100)
    {
        sem_wait(&even_sem);                // Allows odd producer/consumer to work independently
        pthread_mutex_lock(&buffer_lock);   // Mutex to control buffer pointers

        // If there's buffer space, add to the end of it
        if (items_in_buffer < BUFFER_SIZE)
        {
            addToEnd(produced*2, &buffer_head, &buffer_tail);
            evens_in_buffer++;
            items_in_buffer++;
            produced++;
            printf(YELLOW "Produced Even: %d" RESET "\n", produced);
        }

        pthread_mutex_unlock(&buffer_lock);
        sem_post(&even_sem);
        
    }

    return;
}


void consume_odd()
{
    // Wait for all threads to be created
    while(begin_threads == 0) {}

    // If a thread failed to start, exit
    if(begin_threads == -1)
    {
        return;
    }

    int consumed = 0;
    while (consumed < 100)
    {
        sem_wait(&odd_sem);
        if (odds_in_buffer > 0)
        {
            pthread_mutex_lock(&buffer_lock);

            // Search buffer for odd value
            int list_size = getSize(&buffer_head, &buffer_tail);
            int buffer_val;
            for (int i = 0; i < list_size; i++)
            {
                // Check if index has an odd value. If so, remove it from list, break loop.
                if (at(i+1, &buffer_val, &buffer_head, &buffer_tail) == 1 && (buffer_val % 2 != 0))
                {
                    removeSlot(i+1, &buffer_head, &buffer_tail);
                    break;
                }
            }

            odds_in_buffer--;
            items_in_buffer--;
            consumed++;
            printf(BLUE "Consumed Odd: %d" RESET "\n", buffer_val);

            pthread_mutex_unlock(&buffer_lock);
        }
        
        sem_post(&odd_sem);

    }

    return;
}


void consume_even()
{
    // Wait for all threads to be created
    while(begin_threads == 0) {}

    // If a thread failed to start, exit
    if(begin_threads == -1)
    {
        return;
    }

    int consumed = 0;
    while (consumed < 100)
    {
        sem_wait(&even_sem);
        if (evens_in_buffer > 0)
        {
            pthread_mutex_lock(&buffer_lock);

            // Search buffer for even value
            int list_size = getSize(&buffer_head, &buffer_tail);
            int buffer_val;
            for (int i = 0; i < list_size; i++)
            {
                // Check if index has an even value. If so, remove it from list, break loop.
                if (at(i+1, &buffer_val, &buffer_head, &buffer_tail) == 1 && (buffer_val % 2 == 0))
                {
                    removeSlot(i+1, &buffer_head, &buffer_tail);
                    break;
                }
            }

            evens_in_buffer--;
            items_in_buffer--;
            consumed++;
            printf(MAGENTA "Consumed Even: %d" RESET "\n", buffer_val);

            pthread_mutex_unlock(&buffer_lock);
        }
        
        sem_post(&even_sem);

    }

    return;
}


int main()
{
    sem_init(&odd_sem, 0, 1);
    sem_init(&even_sem, 0, 1);
    pthread_mutex_init(&buffer_lock, NULL);

    pthread_t even_producer;
    pthread_t odd_producer;
    pthread_t even_consumer;
    pthread_t odd_consumer;

    // Create each thread. If one fails to start, skip others and
    // join any that have already been started.
    if ( pthread_create(&even_producer, NULL, (void(*))&produce_even, NULL) != 0 ) 
    {
        begin_threads = -1;
        goto exit;
    }
    if ( pthread_create(&odd_producer, NULL, (void(*))&produce_odd, NULL) != 0 )
    {
        begin_threads = -1;
        goto join_1;
    }
    if ( pthread_create(&even_consumer, NULL, (void(*))&consume_even, NULL) != 0 ) 
    {
        begin_threads = -1;
        goto join_2;
    }
    if ( pthread_create(&odd_consumer, NULL, (void(*))&consume_odd, NULL) != 0 )
    {
        begin_threads = -1;
        goto join_3;
    } 

    begin_threads = 1;

    // Join threads in the reverse order they were created for
    // easier thread creation error handling.
    pthread_join(odd_consumer, NULL);    
join_3:
    pthread_join(even_consumer, NULL);
join_2:
    pthread_join(odd_producer, NULL);
join_1:
    pthread_join(even_producer, NULL);

exit:
    pthread_mutex_destroy(&buffer_lock);
    sem_destroy(&even_sem);
    sem_destroy(&odd_sem);
    destroyList(&buffer_head, &buffer_tail);
    return 0;
}