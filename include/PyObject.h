#ifndef PY_OBJECT_H
#define PY_OBJECT_H

#include <stdint.h>

class Type;

/** Base class of all objects actually used in the runtime. */
struct FPyObject {
    /** Type ID */
    uint16_t tag;
    /** Attributes as a special dict */
    struct FPyAttributeDict *attrs;
};

#endif
