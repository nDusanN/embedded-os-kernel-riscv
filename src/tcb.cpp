#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

TCB *TCB::running = nullptr;
uint64 TCB::timeSliceCounter=0;
TCB *TCB::createThread(Body body, void* arg){
    TCB* newTCB = new TCB(body, arg);
    Scheduler::put(newTCB);
    return newTCB;
}

TCB *TCB::createThreadNoStart(Body body, void* arg){            //bez stavljanje niti u scheduler
    TCB* newTCB = new TCB(body, arg);
    return newTCB;
}

void TCB::yield(){
    Riscv::w_a0(0x13);
    __asm__ volatile ("ecall");
}

void TCB::dispatch(){
    TCB *old = running;
    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();

    if(running->isMain()) {                             //postavljanje SPP bita, ulazak u rezim U/S zavisno iz kog se uslo u prekid, MAIN/jedini S
        Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    }
    else {
        Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    }
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper(){
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    thread_dispatch();
}




