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
     * Implementing this to enable objects of type T to be assigned to
     * variables of type Id<T>. Will automatically wrap the object in
     * the Id<T> and assign a NEW unique id to the object of type T.
     */
    Id (T t) {
        (T)(*this) = t;
    }

    /*
     * This freaky statement, allows you to call the constructors
     * of the base class T directly. Neat!
     */
    using T::T;

    /*
     * This is intentionally made not private.
     * This way the id can still be changed in edge cases.
     */
    uint64_t id = ++LAST_ID;

    /*
     * Overloading the equality operator to test the id only.
     */
    bool operator==(const Id<T>& other) {

        return id == other.id;
    }
};

template<typename T>
uint64_t Id<T>::LAST_ID = 1;

#endif
