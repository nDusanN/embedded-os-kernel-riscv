#ifndef syscall_cpp
#define syscall_cpp
#include "syscall_c.hpp"
void* operator new(size_t size);            //New i delete definisani u New.cpp
void operator delete(void* p) noexcept;
//THREAD
class Thread {
public:
    Thread(void (*body)(void*), void* arg);    //ovaj sluzi za sve ostale klas i upotrebe
    virtual ~Thread();
    int start();
    static void dispatch();
    static int sleep(time_t time);
protected:
    Thread();                                  //ovaj konstruktor sluzi za izvedene klase
    virtual void run();
private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;
    //wrapper za poziv run() metode
    static void runWrapper(void* thread) {
        if(thread) {
            ((Thread*)thread)->run();
        }
    }
};
//SEMAPHORE
class Semaphore {
public:
    Semaphore(unsigned init = 1);
    virtual ~Semaphore();

    int wait();
    int signal();
    int trywait();
private:
    sem_t myHandle;
};
//CONSOLE
class Console {
public:
    static void putc(char c);
    static char getc();
};


#endif //syscall_cpp