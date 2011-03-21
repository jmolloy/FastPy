#include <PyRuntime.h>
#include <PyFunction.h>
#include <PyNone.h>
#include <PyDict.h>
#include <PyString.h>
#include <Type.h>

#include <stdlib.h>
#include <stdio.h>

void *PyRuntime_CheckCall(PyObject *obj) {
    if(obj == NULL) {
        /**@todo Throw exception */
        abort();
    }

    static uint16_t fn_tag = Type::TagFor(Type::GetFunctionTy());
    if(obj->tag == fn_tag) {
        return ((PyFunction*)obj)->fn;
    }
    /** @todo Throw exception */
    abort();
}

PyObject *PyRuntime_Print(PyObject *obj) {
    if(obj->tag == Type::TagFor(Type::GetStringTy())) {
        printf("%s\n", ((PyString*)obj)->str );
    } else {
        /** @todo Throw exception */
        abort();
    }

    return PyNone_Create();
}

void PopulateDictWithBuiltins(PyDict *dict) {
    PyDict_Insert(dict, PyString_Create("print"), PyFunction_Create((void*)&PyRuntime_Print));
}
