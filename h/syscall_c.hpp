#ifndef syscall_c
#define syscall_c
#include "tcb.hpp"
#include "mySemaphore.hpp"
//MemoryAllocator deo
void* mem_alloc(size_t size);
int mem_free(void* p);
//Thread deo
typedef TCB* thread_t;

int thread_create(thread_t* handle,void (*start_routine)(void*), void* arg);
int thread_exit();
void thread_dispatch();
//Semaphore deo
typedef MySemaphore* sem_t;

int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);
int thread_create_no_start(thread_t* handle, void (*start_routine)(void*), void* arg);
int thread_start(TCB* tcb);
int time_sleep(time_t time);//Ako uradim asinhroni deo
//Console deo
char getc();
void putc(char c);

#endif //syscall_c
