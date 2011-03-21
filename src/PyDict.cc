#include <PyDict.h>
#include <PyString.h>
#include <Type.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PyDict *PyDict_Create() {
    PyDict *d = new PyDict();
    d->tag = Type::TagFor(Type::GetDictTy());
    return d;
}

PyObject *PyDict_Lookup(PyDict *dict, unsigned long hash, PyObject *var) {
    //fprintf(stderr, "PyDict_Lookup(%p, %lx, %p)\n", dict, hash, var);

    if(dict->m.find(var) != dict->m.end()) {
        return dict->m[var];
    }

    /**@todo Throw exception */
    abort();
    return 0;
}

void PyDict_Insert(PyDict *dict, PyObject *key, PyObject *value) {
    //fprintf(stderr, "PyDict_Insert(%p, %p, %p)\n", dict, key, value);
    dict->m[key] = value;
}

bool PyDict_Compare(PyObject *obj, PyObject *obj2) {
    static uint16_t str_ty = Type::TagFor(Type::GetStringTy());

    if(obj->tag == str_ty && obj2->tag == str_ty) {
        return ! (strcmp( ((PyString*)obj)->str,
                          ((PyString*)obj2)->str ) == 0);
    } else {
        return ! (obj == obj2);
    }
}
