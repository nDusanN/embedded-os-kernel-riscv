#ifndef mySemaphore
#define mySemaphore
#include "list.hpp"
#include "tcb.hpp"
#include "memoryAllocator.hpp"
class MySemaphore {
private:
    bool closed;
    uint64 value;
    List<TCB> blockedThrList;           //lista niti koje cekaju na semaforu, pomocu ulancane liste
public:
    explicit MySemaphore(unsigned val=1):value(val){
        closed= false;
    }
    int wait();
    int signal();
    int trywait();
    int getValue() const { return value; }
    int close();
    static MySemaphore* createSemaphore(unsigned val=1);
    //operatori NEW I DELETE
    void* operator new(size_t size) {
        size+= sizeof(memoryAllocatorMy::MemHeader);
        if(size%MEM_BLOCK_SIZE!=0){
            size=size+MEM_BLOCK_SIZE-size%MEM_BLOCK_SIZE;
        }
        size/=MEM_BLOCK_SIZE;
        return MemoryAllocator::mem_alloc(size);
    }
    void* operator new[](size_t size) {
        size+= sizeof(memoryAllocatorMy::MemHeader);
        if(size%MEM_BLOCK_SIZE!=0){
            size=size+MEM_BLOCK_SIZE-size%MEM_BLOCK_SIZE;
        }
        size/=MEM_BLOCK_SIZE;
        return MemoryAllocator::mem_alloc(size);
    }
    void operator delete(void *ptr) {
        MemoryAllocator::mem_free(ptr);
    }
    void operator delete[](void *ptr) {
        MemoryAllocator::mem_free(ptr);
    }
};

#endif //mySemaphore