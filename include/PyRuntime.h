#ifndef PY_RUNTIME_H
#define PY_RUNTIME_H

#include <Object.h>

class Dict;

#define TRACE_C_CALLS 1

extern "C" {

    void *FPyRuntime_CheckCall(Object *obj);

    Object *FPyRuntime_Print(Object *obj);
    Object *FPyRuntime_PrintItem(Object *obj);
    Object *FPyRuntime_PrintNewline();

    Object *FPyRuntime_ExceptionCompare(Object *obj, Object *obj2);

    Object *FPyRuntime_CallC_LJ(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5);
    Object *FPyRuntime_CallC_LLVM(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5);
    Object *FPyRuntime_CallC_LLVM1(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5);
    Object *FPyRuntime_CallC_LLVM2(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5);
    Object *FPyRuntime_CallC_LLVM3(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5);
    Object *FPyRuntime_CallC_LLVM4(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5);
}

void PopulateDictWithBuiltins(Dict *dict);

#endif
