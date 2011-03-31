#include <PyRuntime.h>
#include <Type.h>
#include <Function.h>
#include <variables.h>
#include <constants.h>
#include <exceptions.h>
#include <Type.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <execinfo.h>
#include <cxxabi.h>

#include <db.h>

#include <jit/jit.h>

static int print_function_name(void *fn);

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
    if(dynamic_cast<ConstantString*>(obj) != 0) {
        printf("'%s'", dynamic_cast<ConstantString*>(obj)->str().c_str());
    } else {
        printf("%s", obj->Repr().c_str() );
    }
    return (Object*)Constant::GetNone();
}
Object *FPyRuntime_PrintNewline() {
    printf("\n");
    return (Object*)Constant::GetNone();
}

Object *FPyRuntime_CallC_LJ(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5) {
#if defined(TRACE_C_CALLS)
    int num_args;
    if(db_print(0, DB_PRINT_C_CALLS)) {
        printf("*** ");
        num_args = print_function_name(fn);
        printf("(");
        if(num_args > 0) printf("%.20s, ", self->Repr().c_str());
        if(num_args > 1) printf("%.20s, ", p1->Repr().c_str());
        if(num_args > 2) printf("%.20s, ", p2->Repr().c_str());
        if(num_args > 3) printf("%.20s, ", p3->Repr().c_str());
        if(num_args > 4) printf("%.20s, ", p4->Repr().c_str());
        if(num_args > 5) printf("%.20s, ", p5->Repr().c_str());

        if(num_args == -1) printf("%p, %p, %p, %p, %p, %p", self, p1, p2, p3, p4,p5);
        printf(")\n");
    }
#endif
    try {
        typedef Object *(*FnTy)(Object*,Object*,Object*,Object*,Object*,Object*);
        FnTy _fn = (FnTy)fn;

        Object *r = _fn(self, p1, p2, p3, p4, p5);
#if defined(TRACE_C_CALLS)
    if(db_print(0, DB_PRINT_C_CALLS)) {
        if(num_args == -1) {
            fprintf(stdout, "***  -> %p\n", r);
        } else {
            fprintf(stdout, "***  -> %.20s\n", r->Repr().c_str());
        }
    }
#endif
        return r;
    } catch(Exception *e) {
        jit_exception_throw(e);
    }
}

void PopulateDictWithBuiltins(Dict *dict) {
    dict->Set(Constant::GetString("print"), new BuiltinFunction((void*)&FPyRuntime_Print));
    dict->Set(Constant::GetString("int"), Type::For(Constant::GetInt(0)));
    dict->Set(Constant::GetString("str"), Type::For(Constant::GetString("")));
    dict->Set(Constant::GetString("Exception"), Type::For(new Exception("","")));
    dict->Set(Constant::GetString("RuntimeError"), Type::For(new RuntimeError("")));
    dict->Set(Constant::GetString("NotImplementedError"), Type::For(new NotImplementedError("")));
    dict->Set(Constant::GetString("KeyError"), Type::For(new KeyError("")));
    dict->Set(Constant::GetString("StandardError"), Type::For(new StandardError("")));
    dict->Set(Constant::GetString("TypeError"), Type::For(new TypeError("")));
}

static int print_function_name(void *fn) {

    char **syms = backtrace_symbols(&fn, 1);
    char *sym = syms[0];
    /* sym will look something like:
       ./fastpy(_ZN6Object10__Subscr__EPS_+0x55) [0x46e6b]

       We want it to look like:
       Object::__Subscr__() */
    int idx = 0;
    for(idx = 0; idx < strlen(sym); ++idx) {
        if(sym[idx] == '(') {
            ++idx;
            break;
        }
    }
    for(int idx2 = 0; idx2 < strlen(sym); ++idx2) {
        if(sym[idx2] == '+') {
            sym[idx2] = 0;
            break;
        }
    }

    if(idx >= strlen(sym)) {
        fprintf(stdout, "%s", sym);
        return -1;
    } else {
        int status = -1;
        char *realname = abi::__cxa_demangle(&sym[idx], NULL, 0, &status);
        if(status != 0) {
            fprintf(stdout, "%s", &sym[idx]);
            return -1;
        } else {
            int n_args = 1;
            int is_member = 0;
            for(int i = 0; i < strlen(realname); i++) {
                if(realname[i] == ':') {
                    is_member = 1;
                }
                if(realname[i] == ',') {
                    ++n_args;
                }
                if(realname[i] == ')' && realname[i-1] == '(') {
                    n_args = 0;
                    break;
                }
            }
            for(int i = 0; i < strlen(realname); i++) {
                if(realname[i] == '(') {
                    realname[i] = '\0';
                    break;
                }
            }

            fprintf(stdout, "%s", realname);

            free(realname);
            return n_args+is_member;
        }
    }
}
