#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../test/printing.hpp"
void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

using Body = void (*)(void*);

void Riscv::handleSupervisorTrap() {
    uint64 scause = r_scause(); //citanje scause registra i odlucivanje u koju funkciju se ulazi/visestruko grananje
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        // ecall iz korisničkog režima || ecall iz sistemskog režima
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        uint64 codeOperation = Riscv::r_a0();     //ODLUKA:pri ecall-u prosledjujemo kod operacije putem a0, iz kog po ulasku u
        int returnValue;                          //prekidnu rutinu citamo kod i pomocu visetrukog grananja ulazimo u odgovarajuci
        MySemaphore *semHandlePtr;
        switch (codeOperation) {
            //MemoryAllocator deo
            case 0x01:
                //mem_alloc(size)
                size_t size;                      //po zahtevu projekta ABI-ju se prosledjuje vec zaokruzen broj blokova
                void *ptr;
                __asm__ volatile("mv %0, a1" : "=r" (size));
                ptr = MemoryAllocator::mem_alloc(size);

                __asm__ volatile("mv t0, %0" : : "r"(ptr));
                __asm__ volatile ("sw t0, 80(x8)");
                break;
            case 0x02:
                //mem_free(ptr)
                void *memptr;
                __asm__ volatile("mv %0, a1" : "=r" (memptr));
                returnValue = MemoryAllocator::mem_free(memptr);

                __asm__ volatile("mv t0, %0" : : "r"(returnValue));
                __asm__ volatile ("sw t0, 80(x8)");
                break;
            //Thread deo
            case 0x09:
                //thread_start
                TCB *tcbForStart;
                __asm__ volatile("mv %0, a1" : "=r" (tcbForStart));
                TCB::startThread(tcbForStart);
                break;
            case 0x10:
                TCB **tcbNoStart;
                Body bodyNoStart;
                void *argNoStart;
                __asm__ volatile ("mv %0, a1" : "=r" (tcbNoStart));
                __asm__ volatile ("mv %0, a2" : "=r" (bodyNoStart));
                __asm__ volatile ("mv %0, a7" : "=r" (argNoStart));
                *tcbNoStart = TCB::createThreadNoStart(bodyNoStart, argNoStart);

                if (*tcbNoStart != nullptr) {
                    __asm__ volatile ("li t0, 0");
                    __asm__ volatile ("sw t0, 80(x8)");
                } else {
                    __asm__ volatile ("li t0, -1");
                    __asm__ volatile ("sw t0, 80(x8)");
                }
                break;
            case 0x11:
                //thread_create
                TCB **tcb;
                Body body;
                void *arg;
                __asm__ volatile ("mv %0, a1" : "=r" (tcb));
                __asm__ volatile ("mv %0, a2" : "=r" (body));
                __asm__ volatile ("mv %0, a7" : "=r" (arg));
                *tcb = TCB::createThread(body, arg);
                if (*tcb != nullptr) {
                    __asm__ volatile ("li t0, 0");
                    __asm__ volatile ("sw t0, 80(x8)");
                } else {
                    __asm__ volatile ("li t0, -1");
                    __asm__ volatile ("sw t0, 80(x8)");
                }
                break;
            case 0x12:
                //thread_exit
                TCB::running->setFinished(true);
                TCB::dispatch();
                __asm__ volatile ("li t0, 0");
                __asm__ volatile ("sw t0, 80(x8)");
                break;
            case 0x13:
                //thread_dispatch
                TCB::dispatch();
                break;
            case 0x21:
                //sem_open
                unsigned init;
                MySemaphore **semHandle;
                __asm__ volatile ("mv %0, a2" : "=r" (init));
                __asm__ volatile ("mv %0, a1" : "=r" (semHandle));
                *semHandle = MySemaphore::createSemaphore(init);

                if (*semHandle != nullptr) {
                    __asm__ volatile ("li t0, 0");
                    __asm__ volatile ("sw t0, 80(x8)");
                } else {
                    __asm__ volatile ("li t0, -1");
                    __asm__ volatile ("sw t0, 80(x8)");
                }
                break;
            case 0x22:
                //sem_close
                __asm__ volatile ("mv %0, a1" : "=r" (semHandlePtr));
                if (semHandlePtr != nullptr) {
                    returnValue = semHandlePtr->close();
                } else returnValue = -2;

                __asm__ volatile ("mv t0, %0" : : "r"(returnValue));
                __asm__ volatile ("sw t0, 80(x8)");
                break;
            case 0x23:
                //sem_wait
                __asm__ volatile ("mv %0, a1" : "=r" (semHandlePtr));
                if (semHandlePtr != nullptr) {
                    returnValue = semHandlePtr->wait();
                } else returnValue = -2;

                __asm__ volatile ("mv t0, %0" : : "r"(returnValue));
                __asm__ volatile ("sw t0, 80(x8)");
                break;
            case 0x24:
                //sem_signal
                __asm__ volatile ("mv %0, a3" : "=r" (semHandlePtr));
                if (semHandlePtr != nullptr) {
                    returnValue = semHandlePtr->signal();
                }
                else {
                    returnValue = -2;
                }

                __asm__ volatile ("mv t0, %0" : : "r"(returnValue));
                __asm__ volatile ("sw t0, 80(x8)");
                break;
            case 0x26:
                //sem_trywait
                __asm__ volatile ("mv %0, a1" : "=r" (semHandlePtr));
                if (semHandlePtr != nullptr) {
                    returnValue = semHandlePtr->trywait();
                } else returnValue = -2;

                __asm__ volatile ("mv t0, %0" : : "r"(returnValue));
                __asm__ volatile ("sw t0, 80(x8)");
                break;
            //Console deo
            case 0x41:
                //getc
                returnValue = __getc();
                __asm__ volatile ("mv t0, %0" : : "r"(returnValue));
                __asm__ volatile ("sw t0, 80(x8)");
                break;
            case 0x42:
                //putc
                char c;
                __asm__ volatile ("mv %0, a1" : "=r" (c));
                __putc(c);
                break;
            default:
                break;
        }

        w_sstatus(sstatus);
        w_sepc(sepc);
    } else if (scause == 0x8000000000000001UL) {
        /*TCB::timeSliceCounter++;
        if(TCB::timeSliceCounter>=TCB::running->getTimeSlice()) {
            uint64 volatile sepca = r_sepc();
            uint64 volatile sstatusa = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatusa);
            w_sepc(sepca);
        }*/
        mc_sip(SIP_SSIP);

    } else if (scause == 0x8000000000000009UL) {
        console_handler();
    } else {
        printString("ERROR SCAUSE:");
        printInt(scause);
        printString("\n");
    }
}