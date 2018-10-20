#include "shmcomm.h"
#include <iostream>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

using namespace SimulatorSHM;
using namespace std;

SHMCommPrivate::SHMCommPrivate(Role role, int key, size_t bufsize)
{
    this->role = role;
    this->key = key;
    this->buffersize = bufsize;
    this->shmId = -1;
    this->shmPtr = nullptr;
    this->gWriteId = 0;
}

size_t align(size_t sz) {

    if (sz % 8 != 0) {
        sz += 8 - sz % 8;
    }

    return sz;
}

bool SHMCommPrivate::_attach()
{
    shmsize = align(sizeof(Buffer)) + align(buffersize);
    shmsize *= NBUFFERS;

    /*
     * If we want a shared memory segment and it is not
     * there, we create it. We don't care if we are the
     * server or the client or whatever ...
     */
    shmId = shmget(key, shmsize, IPC_CREAT | 0666);

    if (shmId < 0) {
        cerr << "shmget failed miserably: " << strerror(errno) << endl;
        return false;
    }

    shmPtr = shmat(shmId, nullptr, 0);

    if (shmPtr == (void*)-1) {
        cerr << "shmat failed miserably: " << strerror(errno) << endl;
        return false;
    }

    char * cptr = (char*)shmPtr;
    size_t offset = 0;

    for (int i = 0; i < NBUFFERS; i++) {
        buffers[i] = (Buffer*)(cptr+offset);
        offset += align(sizeof(Buffer));
        buffers[i]->bufferSize = buffersize;
        buffers[i]->state = FREE;
        buffers[i]->writeId = 0;
        offset += align(sizeof(buffersize));
    }
    
    return true;
}

void SHMCommPrivate::detach()
{
    if (shmPtr != nullptr) {
        shmdt(shmPtr);
        shmPtr = nullptr;
    }
}

bool SHMCommPrivate::destroy()
{
    // if(shmId < 0) shmId = shmget(key, shmsize, 0666);
    // if(shmId < 0) return false;
    // shmctl(shmId, IPC_RMID, nullptr);
    return true;
}

void *SHMCommPrivate::_lock(LockMode mode)
{
    uint64_t bestId = 0;

    if (mode == WRITE_OVERWRITE_OLDEST || mode == READ_OLDEST) {
        bestId = -1;
    }

    Buffer * bestBuf = nullptr;

    for(int i = 0; i < NBUFFERS; i++){
        if(mode == WRITE_NO_OVERWRITE || mode == WRITE_OVERWRITE_OLDEST){
            if(buffers[i]->state == FREE){
                gWriteId++;
                buffers[i]->state = WRITING;
                buffers[i]->writeId = gWriteId;

                char * ptr = (char*)buffers[i];
                ptr+=align(sizeof(Buffer));

                return ptr;
            }else if(buffers[i]->state == DATA && mode == WRITE_OVERWRITE_OLDEST){
                if(buffers[i]->writeId < bestId){
                    bestBuf = buffers[i];
                    bestId = buffers[i]->writeId;
                }
            }
        } else if (buffers[i]->state == DATA) {
            if(mode == READ_OLDEST){
                if(buffers[i]->writeId < bestId){
                    bestBuf = buffers[i];
                    bestId = buffers[i]->writeId;
                }
            }else{
                if(buffers[i]->writeId > bestId){
                    bestBuf = buffers[i];
                    bestId = buffers[i]->writeId;
                }
            }
        }
    }

    if(mode == WRITE_NO_OVERWRITE) {
        return nullptr;
    } else if (bestBuf == nullptr) {
        return nullptr;
    } else if (mode == WRITE_OVERWRITE_OLDEST) {
        gWriteId++;
        bestBuf->state = WRITING;
        bestBuf->writeId = gWriteId;
    } else {
        bestBuf->state = READING;
    }

    char * ptr = (char*)bestBuf;
    ptr+=align(sizeof(Buffer));

    return ptr;
}

void SHMCommPrivate::_unlock(void *buffer)
{
    char* ptr = (char*)buffer;
    ptr = ptr-align(sizeof (Buffer));
    Buffer * bp = (Buffer*)ptr;

    if (bp->state == READING) bp->state = FREE;
    else bp->state = DATA;

    return;
}

SHMCommPrivate::~SHMCommPrivate() {

    detach();

    /*
     * We do NOT remove the shared memory from the system!
     * This makes more problems that it's worth anyway.
     * If lightdm (notice, "light", haha ... \s) allocates
     * 120MB of shared memory forever, we might as well
     * do the same and give no shit about deleting it.
     */
}
