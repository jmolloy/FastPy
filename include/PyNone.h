#ifndef PY_NONE_H
#define PY_NONE_H

#include <PyObject.h>

struct FPyNone : public FPyObject {
};

extern "C" {

FPyNone *FPyNone_Create();

}

#endif
