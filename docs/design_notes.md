# Design Notes

I used semaphores for each thread to wake up on. I used a mutex to restrict buffer access to a single thread at a time.

## Shared Variables
* `space_in_buf` semaphore: consumers post this after removing an item from the buffer
* `odd_sem` semaphore: odd producer posts this after adding to the buffer
* `even_sem` semaphore: even producer posts this after adding to the buffer
* `buffer_lock` mutex: single entry point for all threads to access the shared buffer after "acquiring" their respective semaphore

* Wakeups
    * Odd Producer: waits on `space_in_buf` (posted by consumers)
    * Even Producer: waits on `space_in_buf` (posted by consumers)
    * Odd Consumer: waits on `odd_sem` (posted by odd producer)
    * Even Consumer: waits on `even_sem` (posted by even producer)
* Buffer
    * All threads wait to gain access to the `buffer_lock` mutex after being woken up

## Eliminating Blocking

### Producers
After being woken up by a consumer posting to `space_in_buf`, the producer waits to acquire `buffer_lock`. If a consumer has the buffer now, another item will just be taken. If the other producer has the buffer, then the count of `space_in_buf` was already greater than 1- it has a starting count of 50 (buffer size).

### Consumers
After being woken up by a producer posting to `odd_sem` or `even_sem`, the consumer thread waits to acquire the buffer. When it does, the producer will have put something in there for the consumer to take (that's who posted to the semaphore).

## Targeting
Each thread is only woken up when its own semaphore gets posted to (or is already > 0). So there are 0 wasted wakeups.

## Termination
The consumers and producers sit in while loops that only end after 100 items have been produced/consumed. Producers immediately terminate after their 100th item and consumers terminate after taking their 100th item. So unless a thread crashes, all threads complete.

## Reproducibility
I use `rand_r()`, so any given seed will create the same string of numbers in order. However, the exact order that the numbers are placed in and taken from the buffer across all 4 threads is random and based on how the OS schedules the threads for every program execution.