#ifndef SHMCOMM_H
#define SHMCOMM_H

#include <stdlib.h>
#include <inttypes.h>

#define NBUFFERS 4

namespace SimulatorSHM {
enum Role{
    SERVER, CLIENT
};

enum BufferState{
    READING, WRITING, DATA, FREE
};

enum LockMode{
    WRITE_NO_OVERWRITE, WRITE_OVERWRITE_OLDEST, READ_OLDEST, READ_NEWEST
};

struct Buffer{
    size_t bufferSize;
    BufferState state;
    uint64_t writeId;
};



class SHMCommPrivate
{
public:
    SHMCommPrivate(Role role, int key,size_t bufsize);


    void detach();
    bool destroy();
    bool _attach();
    void *_lock(LockMode mode);
    void _unlock(void * buffer);

    ~SHMCommPrivate();
private:

    bool initialized;
    void * shmPtr;
    int shmId;
    int key;
    Role role;
    Buffer * buffers[NBUFFERS];
    uint64_t gWriteId;
    size_t buffersize;
    size_t shmsize;
};



template <typename DataType>
class SHMComm{
public:
    SHMComm(Role role, int key) : p(role,key,sizeof(DataType)){

    }

    void detach(){
        p.detach();
    }
    bool destroy(){
        return p.destroy();
    }
    bool attach(){
        return p._attach();
    }
    DataType *lock(LockMode mode){
        return (DataType*)p._lock(mode);
    }
    void unlock(DataType * buffer){
        p._unlock(buffer);
    }
private:

    SHMCommPrivate p;
};

}



#endif // SHMCOMM_H
