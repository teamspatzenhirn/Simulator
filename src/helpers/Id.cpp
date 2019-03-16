#include "helpers/Id.h"

uint64_t getId() {

    /*
     * This has space for 18446744073709552615 IDs.
     * That should be enough.
     */
    static uint64_t last_id = 1;

    return ++last_id;
}
