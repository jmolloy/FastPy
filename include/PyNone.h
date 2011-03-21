#ifndef PY_NONE_H
#define PY_NONE_H

#include <PyObject.h>

struct PyNone : public PyObject {
};

extern "C" {

PyNone *PyNone_Create();

}

#endif
