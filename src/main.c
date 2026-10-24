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

#define RED     "\033[0;31m" // Main thread
#define GREEN   "\033[0;32m" // Odd Producer
#define YELLOW  "\033[0;33m" // Even Producer
#define BLUE    "\033[0;34m" // Odd Consumer
#define MAGENTA "\033[0;35m" // Even Consumer
#define RESET   "\033[0m"

int begin_threads = 0; // Global thread launch


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
        printf(GREEN "Produced Odd: %d" RESET "\n", produced);

        produced++;
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
        printf(YELLOW "Produced Even: %d" RESET "\n", produced);

        produced++;
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
        printf(BLUE "Consumed Odd: %d" RESET "\n", consumed);

        consumed++;
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
        printf(MAGENTA "Consumed Even: %d" RESET "\n", consumed);

        consumed++;
    }

    return;
}


int main()
{

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
join_all:
    pthread_join(odd_consumer, NULL);    
join_3:
    pthread_join(even_consumer, NULL);
join_2:
    pthread_join(odd_producer, NULL);
join_1:
    pthread_join(even_producer, NULL);

exit:
    return 0;
}