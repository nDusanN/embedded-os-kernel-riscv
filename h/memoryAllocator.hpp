#ifndef memoryAllocatorMy
#define memoryAllocatorMy
#include "../lib/hw.h"
struct MemHeader {      //ODLUKA:cuvanje podataka o slobodnim i zauzetim blokovim apomocu ove strukture
    MemHeader* next;    //DVOSTRUKO ULANCANA LISTA
    MemHeader* prev;    //mnogo efikasnije od jednostruke
    size_t size;
};
class MemoryAllocator {
public:
    static MemHeader* free;
    static MemHeader* used;
    static void* mem_alloc(size_t size);
    static int mem_free (void* ptr);
    static void tryToJoin(MemHeader* curr);
    static void initFreeBlock() {
        MemoryAllocator::free = (MemHeader*)((char*)HEAP_START_ADDR);//ODLUKA:u listi cuvam pokazivac na sam slobodan deo, a vracam posle MemHeader-a
        used = nullptr;
        free->next = nullptr;
        free->prev  = nullptr;
        free->size = ((char*)HEAP_START_ADDR - (char*)HEAP_END_ADDR );//pocetna velicina ceo hip
    }
};
#endif //memoryAllocatorMy
