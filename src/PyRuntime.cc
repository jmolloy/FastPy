#include <PyRuntime.h>
#include <Type.h>
#include <Function.h>
#include <variables.h>
#include <constants.h>
#include <exceptions.h>

#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>

#include <db.h>

#include <jit/jit.h>

void *FPyRuntime_CheckCall(Object *obj) {
    if(obj == NULL) {
        /**@todo Throw exception */
        abort();
    }

    Function *fn = dynamic_cast<Function*>(obj);
    if(!fn) {
        /**@todo Throw exception*/
        abort();
    }

    return fn->GetFnPtr();
}

Object *FPyRuntime_Print(Object *obj) {
    if(dynamic_cast<ConstantString*>(obj) != 0) {
        printf("%s\n", (dynamic_cast<ConstantString*>(obj))->str().c_str() );
    } else {
        /** @todo Throw exception */
        abort();
    }

    return (Object*)Constant::GetNone();
}
Object *FPyRuntime_PrintItem(Object *obj) {
    printf("%s", obj->Repr().c_str() );
    return (Object*)Constant::GetNone();
}
Object *FPyRuntime_PrintNewline() {
    printf("\n");
    return (Object*)Constant::GetNone();
}

Object *FPyRuntime_CallC_LJ(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5) {
#if defined(TRACE_C_CALLS)
    if(db_print(0, DB_PRINT_C_CALLS)) {
        char **syms = backtrace_symbols(&fn, 1);
        fprintf(stderr, "CallC_LJ: %s (%p, %p, %p, %p, %p, %p)\n", syms[0], self, p1, p2, p3, p4, p5);
        free(syms);
    }
#endif
    try {
        typedef Object *(*FnTy)(Object*,Object*,Object*,Object*,Object*,Object*);
        FnTy _fn = (FnTy)fn;

        Object *r = _fn(self, p1, p2, p3, p4, p5);
#if defined(TRACE_C_CALLS)
//        fprintf(stderr, "  -> %ld\n", (long)r);
#endif
        return r;
    } catch(Exception *e) {
        jit_exception_throw(e);
    }
}

void PopulateDictWithBuiltins(Dict *dict) {
    dict->Set(Constant::GetString("print"), new BuiltinFunction((void*)&FPyRuntime_Print));
}
