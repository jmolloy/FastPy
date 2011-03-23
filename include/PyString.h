#ifndef PY_STRING_H
#define PY_STRING_H

#include <PyObject.h>

struct FPyString : public FPyObject {
    const char *str;
};

extern "C" {

FPyString *FPyString_Create(const char *str);

}

#endif
