#ifndef PY_RUNTIME_H
#define PY_RUNTIME_H

#include <PyObject.h>

struct PyDict;

extern "C" {

    void *PyRuntime_CheckCall(PyObject *obj);

    PyObject *PyRuntime_Print(PyObject *obj);

}

void PopulateDictWithBuiltins(PyDict *dict);

#endif
