#ifndef PY_FUNCTION_H
#define PY_FUNCTION_H

#include <PyObject.h>

struct FPyFunction : public FPyObject {
    void *fn;
};

extern "C" {

FPyFunction *FPyFunction_Create(void *fn);

}

#endif
