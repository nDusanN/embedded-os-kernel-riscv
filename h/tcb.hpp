#ifndef ThreadMy   // Thread Control Block
#define ThreadMy
#include "memoryAllocator.hpp"
#include "scheduler.hpp"
class TCB{
public:

    using Body = void (*)(void*);
    static TCB *running;
    uint64 getTimeSlice() const {return timeSlice;}
    bool isFinished() const { return finished; }
    void setFinished(bool value) { finished = value; }
    bool isBlocked() const { return this->blocked; }
    void setBlocked(bool value) { this->blocked = value; }
    bool isMain() const { return this->main; }

    static TCB *createThread(Body body, void* arg);
    static TCB *createThreadNoStart(Body body, void* arg);
    static void startThread(TCB* tcbToStart) {
        Scheduler::put(tcbToStart);
    }

    static void dispatch();
    static void yield();

    ~TCB() { delete[] stack; }
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

private:
    TCB(Body body, void* arg) :
            body(body),
            stack(body != nullptr ? new uint64[DEFAULT_STACK_SIZE] : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            timeSlice(DEFAULT_TIME_SLICE),
            finished(false),
            blocked(false),
            main(body == nullptr),
            arg(arg)
    {}

private:
    struct Context{
        uint64 ra;
        uint64 sp;
    };
    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    bool blocked;
    bool main;
    void* arg;

    static uint64 timeSliceCounter;
    friend class Riscv;

    static void threadWrapper();
    static void contextSwitch(Context *oldContext, Context *runningContext);


};

#endif //ThreadMy
