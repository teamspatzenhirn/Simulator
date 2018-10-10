#ifndef INC_2019_ID_H
#define INC_2019_ID_H

#include <cstdint>

/*
 * This is a little fancy class definition that allows one to add a (reasonably)
 * unique ID to any class/struct. This provides a better identification
 * mechanism for objects then using e.g. memory addresses. 
 */
template<typename T> class Id : public T {

    /*
     * This has space for 18446744073709552615 IDs.
     * Not joking. That should be enough.
     */
    static uint64_t LAST_ID;

public:

    /*
     * This freaky statement, allows you to call the constructors
     * of the base class T directly. Neat!
     */
    using T::T;

    /*
     * This is intentionally made not private.
     * This way the id can still be changed in edge cases.
     */
    uint64_t id = LAST_ID++;
};

template<typename T>
uint64_t Id<T>::LAST_ID = 0;

#endif
