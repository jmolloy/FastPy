#ifndef PY_RUNTIME_H
#define PY_RUNTIME_H

#include <PyObject.h>

struct FPyDict;

extern "C" {

    void *FPyRuntime_CheckCall(FPyObject *obj);

    FPyObject *FPyRuntime_Print(FPyObject *obj);
    FPyObject *FPyRuntime_PrintItem(FPyObject *obj);
    FPyObject *FPyRuntime_PrintNewline();

}

void PopulateDictWithBuiltins(FPyDict *dict);

#endif
