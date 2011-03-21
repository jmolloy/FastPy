#ifndef PY_FUNCTION_H
#define PY_FUNCTION_H

#include <PyObject.h>

struct PyFunction : public PyObject {
    void *fn;
};

extern "C" {

PyFunction *PyFunction_Create(void *fn);

}

#endif
