#include "../h/tcb.hpp"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"


extern void userMain();

int main()
{
    TCB *threads[5];

    MemoryAllocator::initFreeBlock();

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    threads[0] = TCB::createThread(nullptr,nullptr);
    TCB::running = threads[0];

    thread_create(&threads[1], reinterpret_cast<void (*)(void *)>(userMain), nullptr);

    while(!threads[1]->isFinished()) {
        thread_dispatch();
    }

    printString("Vratio sam se u main\n");

    return 0;
}
