#ifndef PY_STRING_H
#define PY_STRING_H

#include <PyObject.h>

struct PyString : public PyObject {
    const char *str;
};

extern "C" {

PyString *PyString_Create(const char *str);

}

#endif
