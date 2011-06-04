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
#include <demangle.h>

#include <db.h>

#include <jit/jit.h>

static char *get_function_name(void *fn);

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
        printf("%s", dynamic_cast<ConstantString*>(obj)->str().c_str());
    } else if(dynamic_cast<ConstantString*>(obj->__Repr__()) != 0) {
        printf("%s", dynamic_cast<ConstantString*>(obj->__Repr__())->str().c_str());
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
    if(db_print(0, DB_PRINT_C_CALLS)) {
        char *symn = get_function_name(fn);

        printf("*** ");
        symbol_t *sym = (symbol_t*)malloc(sizeof(symbol_t));
        memset(sym, 0, sizeof(symbol_t));

        demangle(LargeStaticString(symn), sym);
        if(sym->is_ctor) {
            printf("%s(<ctor>)\n", (const char*)sym->name);
        } else {
            Object *objs[] = {p1,p2,p3,p4,p5};

            bool is_member = sym->name.contains("::") && !sym->is_ctor;
            printf("%s(", (const char*)sym->name);
            if(is_member) {
                printf("%.32s", self->Repr().c_str());
            }
            for(int i = 0; i < sym->nParams; i++) {
                if(i > 0 || is_member) {
                    printf(", ");
                }

                if(sym->params[i] == "Object*") {
                    printf("%.32s", objs[i]->Repr().c_str());
                } else {
                    printf("%p", objs[i]);
                }
            }
            printf(")\n");
        }
        free(sym);
    }
#endif
    try {
        typedef Object *(*FnTy)(Object*,Object*,Object*,Object*,Object*,Object*);
        FnTy _fn = (FnTy)fn;

        Object *r = _fn(self, p1, p2, p3, p4, p5);
#if defined(TRACE_C_CALLS)
        if(db_print(0, DB_PRINT_C_CALLS)) {
            // if(num_args == -1 || is_ctor) {
            //     fprintf(stdout, "***  -> %p\n", r);
            // } else {
            //     fprintf(stdout, "***  -> %.20s\n", r->Repr().c_str());
            // }
        }
#endif
        return r;
    } catch(Exception *e) {
        jit_exception_throw(e);
    }
}

Object *FPyRuntime_CallC_LLVM(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5) {
#if defined(TRACE_C_CALLS)
    if(db_print(0, DB_PRINT_C_CALLS)) {
        char *symn = get_function_name(fn);

        printf("*** ");
        symbol_t *sym = (symbol_t*)malloc(sizeof(symbol_t));
        memset(sym, 0, sizeof(symbol_t));

        demangle(LargeStaticString(symn), sym);
        if(sym->is_ctor) {
            printf("%s(<ctor>)\n", (const char*)sym->name);
        } else {
            Object *objs[] = {p1,p2,p3,p4,p5};

            bool is_member = sym->name.contains("::") && !sym->is_ctor;
            printf("%s(", (const char*)sym->name);
            if(is_member) {
                printf("%.32s", self->Repr().c_str());
            }
            for(int i = 0; i < sym->nParams; i++) {
                if(i > 0 || is_member) {
                    printf(", ");
                }

                if(sym->params[i] == "Object*") {
                    printf("%.32s", objs[i]->Repr().c_str());
                } else {
                    printf("%p", objs[i]);
                }
            }
            printf(")\n");
        }
        free(sym);
    }
#endif
    typedef Object *(*FnTy)(Object*,Object*,Object*,Object*,Object*,Object*);
    FnTy _fn = (FnTy)fn;

    Object *r = _fn(self, p1, p2, p3, p4, p5);
#if defined(TRACE_C_CALLS)
    if(db_print(0, DB_PRINT_C_CALLS)) {
        // if(num_args == -1 || is_ctor) {
        //     fprintf(stdout, "***  -> %p\n", r);
        // } else {
        //     fprintf(stdout, "***  -> %.20s\n", r->Repr().c_str());
        // }
    }
#endif
    return r;
}


Object *FPyRuntime_CallC_LLVM1(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5) {
    return FPyRuntime_CallC_LLVM(fn, self, p1, p2, p3, p4, p5);
}
Object *FPyRuntime_CallC_LLVM2(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5) {
    return FPyRuntime_CallC_LLVM(fn, self, p1, p2, p3, p4, p5);
}
Object *FPyRuntime_CallC_LLVM3(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5) {
    return FPyRuntime_CallC_LLVM(fn, self, p1, p2, p3, p4, p5);
}
Object *FPyRuntime_CallC_LLVM4(void *fn, Object *self, Object *p1, Object *p2, Object *p3, Object *p4, Object *p5) {
    return FPyRuntime_CallC_LLVM(fn, self, p1, p2, p3, p4, p5);
}

static bool _ExceptionCompare(Object *obj, Object *obj2) {
    if(dynamic_cast<Tuple*>(obj)) {
        Tuple *t = dynamic_cast<Tuple*>(obj);
        for(int i = 0; i < t->Length(); i++) {
            if(_ExceptionCompare(t->Get(i), obj2)) {
                return true;
            }
        }
    } else {
        /* Massive abuse of the dynamic cast system - this function
           is designed for use only by the runtime and is G++
           specific. */
        
        Type *t = dynamic_cast<Type*>(obj);
        assert(t);
        Object *example = t->GetExample();

        void *ptr = (void*)obj2;
        bool b = typeid(*obj2).__do_upcast(dynamic_cast<const abi::__class_type_info*>(&typeid(*example)),
                                           &ptr);
        return b;
    }
    return false;
}

Object *FPyRuntime_ExceptionCompare(Object *obj, Object *obj2) {

#if defined(TRACE_C_CALLS)
    if(db_print(0, DB_PRINT_C_CALLS)) {
        printf("*** ExceptionCompare(");
        printf("%.30s, %.30s)\n", obj->Repr().c_str(), obj2->Repr().c_str());
    }
#endif
    /* Return true if obj2 is a subclass of anything, recursively, in
       obj. */
    bool b = _ExceptionCompare(obj, obj2);

    Object *ret = (Object*)Constant::GetBool(b);
#if defined(TRACE_C_CALLS)
    if(db_print(0, DB_PRINT_C_CALLS)) {
        printf("***    -> %.32s\n", ret->Repr().c_str());
    }
#endif  
    return ret;
}

void PopulateDictWithBuiltins(Dict *dict) {
    dict->Set(Constant::GetString("print"), new BuiltinFunction((void*)&FPyRuntime_Print));
    dict->Set(Constant::GetString("int"), Type::For(Constant::GetInt(0)));
    dict->Set(Constant::GetString("str"), Type::For(Constant::GetString("")));
    dict->Set(Constant::GetString("Exception"), Type::For(new Exception("","")));
    dict->Set(Constant::GetString("RuntimeError"), Type::For(new RuntimeError("")));
    dict->Set(Constant::GetString("NotImplementedError"), Type::For(new NotImplementedError("")));
    dict->Set(Constant::GetString("KeyError"), Type::For(new KeyError("")));
    dict->Set(Constant::GetString("ValueError"), Type::For(new ValueError("")));
    dict->Set(Constant::GetString("StandardError"), Type::For(new StandardError("")));
    dict->Set(Constant::GetString("TypeError"), Type::For(new TypeError("")));
}

static char *get_function_name(void *fn) {

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

    char *x = strdup(&sym[idx]);
    free(syms);
    return x;
}
