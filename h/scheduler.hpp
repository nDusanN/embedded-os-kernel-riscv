#ifndef schedulerMy
#define schedulerMy
#include "list.hpp"
class TCB;

class Scheduler{                        //ODLUKA:koriscenje ulancane liste na nacin kako funkcionise i red
public:                                 //dodavanje na kraj, uzimanje sa pocetka
    static List<TCB> readyThreadQueue;
    static TCB *get();
    static void put(TCB *ccb);

};
#endif //schedulerMy
