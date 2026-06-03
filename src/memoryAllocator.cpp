#include "../h/memoryAllocator.hpp"
MemHeader* MemoryAllocator::free = nullptr;
MemHeader* MemoryAllocator::used = nullptr;
void *MemoryAllocator::mem_alloc(size_t size) {
    size_t newSize=size*MEM_BLOCK_SIZE;         //iz ABI-ja je vec prosledjen zaokruzen broj blokova, samo
    for(MemHeader* curr = MemoryAllocator::free; curr != nullptr; curr=curr->next) {
        if(curr->size<newSize) continue;            //Nije dovoljne velicine->trazimo dalje
        if(curr->size > newSize) {                  //Veci je->zauzimamo velicinu i ostaje jos prostora
            //Novi slobodni, pomeren za newSize od curr
            MemHeader *newBlock;
            newBlock = (MemHeader*) ((char*)curr + newSize);
            //Update size
            newBlock->size = curr->size - newSize;
            curr->size = newSize;
            //Update free list
            if(newBlock->size>MEM_BLOCK_SIZE+ sizeof(MemHeader)) {      //Provera da li je preostalo prostor efektivno dovoljno veliki da se
                if (curr->prev) curr->prev->next = newBlock;            //vrati korisniku
                else MemoryAllocator::free = newBlock;
                if (curr->next) curr->next->prev = newBlock;
                newBlock->prev = curr->prev;
                newBlock->next = curr->next;
            }else{
                if (curr->prev) curr->prev->next = curr->next;
                else MemoryAllocator::free = curr->next;
                if (curr->next) curr->next->prev = curr->prev;
            }

            //Update used list
            if(used == nullptr) {
                used = curr;
                curr->next = nullptr;
                curr->prev = nullptr;
            }
            else if((char*) curr < (char*)used) {
                //Novi zauzeti(curr) postaje usedHeader
                used->prev = curr;
                curr->prev = nullptr;
                curr->next = used;
                used = curr;
            }
            else {
                //Treba ulancati novi zauzeti(curr) na odredjeno mesto u listi
                MemHeader* currUsed = used;
                for(; currUsed->next && (char*)(currUsed->next) < (char*) curr; currUsed = currUsed->next);
                curr->next = currUsed->next;
                curr->prev = currUsed;
                if(curr->next) curr->next->prev = curr;
                currUsed->next = curr;
            }
        }
        else {
            //Slucaj kada su iste velicine->nista ne ostaje slobodno
            //Update free list
            if (curr->prev) curr->prev->next = curr->next;
            else MemoryAllocator::free = curr->next;
            if(curr->next) curr->next->prev = curr->prev;

            //Update used list
            if(used == nullptr) {
                used = curr;
                curr->next = nullptr;
                curr->prev = nullptr;
            }
            else if((char*) curr < (char*)used) {
                //Novi zauzeti(curr) postaje usedHeader
                used->prev = curr;
                curr->prev = nullptr;
                curr->next = used;
                used = curr;
            }
            else {
                //Treba ulancati novi zauzeti(curr) na odredjeno mesto u listiTreba ulancati novi zauzeti(curr) na odredjeno mesto u listi
                MemHeader* currUsed = used;
                for(; currUsed->next && (char*)(currUsed->next) < (char*) curr; currUsed = currUsed->next);

                curr->next = currUsed->next;
                curr->prev = currUsed;
                if(curr->next) curr->next->prev = curr;
                currUsed->next = curr;
            }
        }
        return (char*)curr + sizeof(MemHeader); //korisniku vracamo efektivno koristan prostor, korisnik ne treba da ima uvid u nasu implementaciju
    }                                           //ulancanih slobodnih i zauzetih lista blokova
    return nullptr;
}

int MemoryAllocator::mem_free(void* ptr) {
    if(used == nullptr) return -1;              //Nema nista u used->greska
    if(ptr == nullptr || ptr < HEAP_START_ADDR || ptr > HEAP_END_ADDR) return -2; //Pokazivac van okvira memorije->greska

    MemHeader* curr = (MemHeader*)((char*)ptr - sizeof(MemHeader));     //Korisniku vracamo pokazivac pomeren za velicinu MemHeader-a
    if(curr < used) return -3;                                          //kada ga oslobadjamo priistupamo internom ulancavanju->na pocetak header-a

    //Update used list
    if(used == curr) {
        used = curr->next;
        if(used) used->prev = nullptr;
        curr->next = nullptr;
        curr->prev = nullptr;
    } else {
        curr->prev->next = curr->next;
        if(curr->next) curr->next->prev = curr->prev;
        curr->next = nullptr;
        curr->prev = nullptr;
    }

    //Ubacivanje u free listu
    if (free == nullptr) {
        //Postaje freeHeader jer je free lista prazna
        free = curr;
        curr->next = nullptr;
        curr->prev = nullptr;
    }
    else if((char*)curr < (char*)free) {
        //Postaje freeHeader jer je manji od trenutnog freeHeader
        free->prev = curr;
        curr->prev = nullptr;
        curr->next = free;
        free = curr;
        tryToJoin(free);
    }
    else {
        //Treba ulancati novi slobodni(curr) na odredjeno mesto u listi
        MemHeader* currFree = free;
        for(; currFree->next && (char*)(currFree->next) < (char*) curr; currFree = currFree->next);

        curr->next = currFree->next;
        curr->prev = currFree;
        if(curr->next) curr->next->prev = curr;
        currFree->next = curr;
        tryToJoin(curr);
        tryToJoin(currFree);
    }
    return 0;
}

void MemoryAllocator::tryToJoin(MemHeader *curr) {
    if(curr->next && (char*)curr + sizeof(MemHeader) + curr->size == (char*)curr->next) {
        curr->size += curr->next->size + sizeof(MemHeader);
        curr->next = curr->next->next;
        if(curr->next) curr->next->prev = curr;
    }
}
