#include "../h/mySemaphore.hpp"

MySemaphore* MySemaphore::createSemaphore(unsigned int val) {
    return new MySemaphore(val);
}
int MySemaphore::signal() {
    if(this->closed){return -1;}
    if(++this->value>=0 && blockedThrList.peekFirst()){
        TCB* unblock=MySemaphore::blockedThrList.removeFirst();
        unblock->setBlocked(false);
        Scheduler::put(unblock);
    }
    return 0;
}
int MySemaphore::wait() {
    if(this->closed){return -1;}
    if((int)this->value--<=0){
        TCB::running->setBlocked(true);
        this->blockedThrList.addLast(TCB::running);
        TCB::dispatch();
    }
    return 0;
}
int MySemaphore::close() {
    if(this->closed){return -1;}
    this->closed=true;
    while(this->blockedThrList.peekFirst()){
        TCB* tcb=MySemaphore::blockedThrList.removeFirst();
        tcb->setBlocked(false);
        Scheduler::put(tcb);
    }
    return 0;
}
int MySemaphore::trywait() {
    if (closed) {
        return -1;
    }
    value--;
    if (value <= 0) {
        return 0;
    }
    return 1;
}
