# Assignment 2: Synchronized Typed Buffer

**Topic:** Thread synchronization — predicate-based waiting and correct synchronization (a variation of the bounded producer/consumer problem)

---

Implement a bounded buffer (capacity 50) shared by **two producers** and **two consumers**. Items are integers, each tagged by parity (odd or even). The twist that makes this a synchronization problem rather than a queueing exercise: **each consumer can only take items of its own type**, so it must block until an item of that type is present.

**You must use POSIX threads (pthreads) in C or C++.** All four producers and consumers are pthreads created with `pthread_create` and joined with `pthread_join`; synchronization uses the pthreads/POSIX primitives listed in §5. Solutions built on any other threading model or language will not be accepted.

This gives you **three distinct wait conditions** over one shared buffer:

- a consumer waiting for an **odd** item to exist,
- a consumer waiting for an **even** item to exist,
- a producer waiting for **free space**.

Keeping these three conditions distinct — so that a thread blocked on one of them resumes only when *that* condition holds — is the point of the assignment. A single "an item exists" condition cannot tell a consumer waiting for an odd item apart from one waiting for an even item; working out how to separate the three conditions (with whatever primitives you choose) is the core of the work.

The container itself is unimportant. Use any structure you like (array, ring buffer, `std::deque`, `std::vector`) as long as **all locking is your own**. Ordering within the buffer does not matter; you may remove any item of the requested type. Do **not** spend effort on a fancy data structure — spend it on the synchronization.