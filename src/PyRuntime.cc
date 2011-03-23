#include <PyRuntime.h>
#include <PyFunction.h>
#include <PyNone.h>
#include <PyDict.h>
#include <PyString.h>
#include <Type.h>

#include <stdlib.h>
#include <stdio.h>

#include <jit/jit.h>

void *FPyRuntime_CheckCall(FPyObject *obj) {
    if(obj == NULL) {
        /**@todo Throw exception */
        abort();
    }

    static uint16_t fn_tag = Type::TagFor(Type::GetFunctionTy());
    if(obj->tag == fn_tag) {
        return ((FPyFunction*)obj)->fn;
    }
    /** @todo Throw exception */
    abort();
}

FPyObject *FPyRuntime_Print(FPyObject *obj) {
    if(obj->tag == Type::TagFor(Type::GetStringTy())) {
        printf("%s\n", ((FPyString*)obj)->str );
    } else {
        /** @todo Throw exception */
        abort();
    }

    return FPyNone_Create();
}
FPyObject *FPyRuntime_PrintItem(FPyObject *obj) {
    if(obj->tag == Type::TagFor(Type::GetStringTy())) {
        printf("%s", ((FPyString*)obj)->str );
    } else {
        /** @todo Throw exception */
        abort();
    }

    return FPyNone_Create();
}
FPyObject *FPyRuntime_PrintNewline() {
    printf("\n");
    return FPyNone_Create();
}

void PopulateDictWithBuiltins(FPyDict *dict) {
    FPyDict_Insert(dict, FPyString_Create("print"), FPyFunction_Create((void*)&FPyRuntime_Print));
}
