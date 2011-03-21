#ifndef PY_DICT_H
#define PY_DICT_H

#include <PyObject.h>

#include <map>

bool PyDict_Compare(PyObject *obj, PyObject *obj2);

struct _PyDict_Compare {
    bool operator()(PyObject *obj, PyObject *obj2) {
        return PyDict_Compare(obj, obj2);
    }
};

/** A PyDict is a Dict that is actually accessed and addressed by python code,
    unlike Dict : public Constant, that is just for marshalling and codegen.*/
struct PyDict : public PyObject {
    std::map<PyObject*,PyObject*, _PyDict_Compare> m;
};

extern "C" {

PyDict *PyDict_Create();
PyObject *PyDict_Lookup(PyDict *dict, unsigned long hash, PyObject *var);
void PyDict_Insert(PyDict *dict, PyObject *key, PyObject *value);

}

#endif

