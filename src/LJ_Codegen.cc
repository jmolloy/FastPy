#include <instructions.h>
#include <constants.h>
#include <BasicBlock.h>
#include <jit/jit.h>

#include <PyRuntime.h>
#include <Module.h>
#include <Exception.h>

#include <stdio.h>

static jit_type_t _LJ_fn_signature(int nargs) {
    /* Account for extra argument - must call FPyRuntime_CallC with it! */
    nargs++;

    jit_type_t *tyargs = new jit_type_t[nargs];
    for(int i = 0; i < nargs; i++) {
        tyargs[i] = jit_type_nuint;
    }
    jit_type_t sig = jit_type_create_signature(jit_abi_cdecl,
                                               jit_type_nuint,
                                               tyargs,
                                               nargs,
                                               1);
    assert(sig);
    delete [] tyargs;
    return sig;
}

static jit_value_t _LJ_Call(jit_function_t func, const char *name, void *fp) {
    jit_value_t args[1];
    args[0] = jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)fp);
    return jit_insn_call_native(func, name, (void*)&FPyRuntime_CallC_LJ, _LJ_fn_signature(0), args, 1, 0);
}
static jit_value_t _LJ_Call(jit_function_t func, const char *name, void *fp, jit_value_t arg0) {
    jit_value_t args[2];
    args[0] = jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)fp);
    args[1] = arg0;
    return jit_insn_call_native(func, name, (void*)&FPyRuntime_CallC_LJ, _LJ_fn_signature(1), args, 2, 0);
}
static jit_value_t _LJ_Call(jit_function_t func, const char *name, void *fp, jit_value_t arg0, jit_value_t arg1) {
    jit_value_t args[3];
    args[0] = jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)fp);
    args[1] = arg0;
    args[2] = arg1;
    return jit_insn_call_native(func, name, (void*)&FPyRuntime_CallC_LJ, _LJ_fn_signature(2), args, 3, 0);
}
static jit_value_t _LJ_Call(jit_function_t func, const char *name, void *fp, jit_value_t arg0, jit_value_t arg1, jit_value_t arg2) {
    jit_value_t args[4];
    args[0] = jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)fp);
    args[1] = arg0;
    args[2] = arg1;
    args[3] = arg2;
    return jit_insn_call_native(func, name, (void*)&FPyRuntime_CallC_LJ, _LJ_fn_signature(3), args, 4, 0);
}

static jit_value_t _LJ_CallVtable(jit_function_t func, int idx, jit_value_t obj) {
    jit_value_t vtable_ptr = jit_insn_load_relative(func, obj, 0, jit_type_create_pointer(jit_type_nuint, 1));
    jit_value_t fnp = jit_insn_load_elem(func,
                                         vtable_ptr,
                                         jit_value_create_nint_constant(func, jit_type_nuint, idx),
                                         jit_type_nuint);
    
    jit_value_t args[2];
    args[0] = fnp;
    args[1] = obj;
    return jit_insn_call_native(func, "FPyRuntime_CallC_LJ", (void*)&FPyRuntime_CallC_LJ, _LJ_fn_signature(1), args, 2, 0);
}

static jit_value_t _LJ_CallVtable(jit_function_t func, int idx, jit_value_t obj, jit_value_t p1) {
    jit_value_t vtable_ptr = jit_insn_load_relative(func, obj, 0, jit_type_create_pointer(jit_type_nuint, 1));
    assert(vtable_ptr);
    jit_value_t fnp = jit_insn_load_elem(func,
                                         vtable_ptr,
                                         jit_value_create_nint_constant(func, jit_type_nuint, idx),
                                         jit_type_nuint);
    assert(fnp);
    
    jit_value_t args[4];
    args[0] = fnp;
    args[1] = obj;
    args[2] = p1;
    return jit_insn_call_native(func, "FPyRuntime_CallC_LJ", (void*)&FPyRuntime_CallC_LJ, _LJ_fn_signature(2), args, 3, 0);
}


static jit_value_t _LJ_CallVtable(jit_function_t func, int idx, jit_value_t obj, jit_value_t p1, jit_value_t p2) {
    jit_value_t vtable_ptr = jit_insn_load_relative(func, obj, 0, jit_type_create_pointer(jit_type_nuint, 1));
    assert(vtable_ptr);
    jit_value_t fnp = jit_insn_load_elem(func,
                                         vtable_ptr,
                                         jit_value_create_nint_constant(func, jit_type_nuint, idx),
                                         jit_type_nuint);
    assert(fnp);

    jit_value_t args[4];
    args[0] = fnp;
    args[1] = obj;
    args[2] = p1;
    args[3] = p2;
    return jit_insn_call_native(func, "FPyRuntime_CallC_LJ", (void*)&FPyRuntime_CallC_LJ, _LJ_fn_signature(3), args, 4, 0);
}

static jit_value_t _LJ_AllocMemory(Instruction *i, jit_function_t func, size_t sz) {
    return _LJ_Call(func, "malloc", (void*)&malloc, jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)sz));
}


jit_value_t LoadGlobal::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_value_t g = jit_value_create_nint_constant(func, jit_type_nuint, (jit_nint)f->GetModule()->GetGlobals());
    assert(g);

    jit_insn_mark_offset(func, m_bytecode_offset);

    return _LJ_CallVtable(func,
                          Object::idx__Subscr__,
                          g, m_args[0]->LJ_Codegen(func, f));
}

jit_value_t StoreGlobal::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_value_t g = jit_value_create_nint_constant(func, jit_type_nuint, (jit_nint)f->GetModule()->GetGlobals());
    assert(g);
    
    jit_insn_mark_offset(func, m_bytecode_offset);

    return _LJ_CallVtable(func,
                          Object::idx__StoreSubscr__,
                          g, m_args[0]->LJ_Codegen(func, f), m_args[1]->LJ_Codegen(func, f));
}

jit_value_t Return::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);
    jit_insn_return(func, m_args[0]->LJ_Codegen(func, f));
    return NULL;
}

jit_value_t Call::_LJ_Codegen(jit_function_t func, Function *f) {
    /* Call out to an external helper that will provide type checking */
    jit_type_t tyargs[1];
    tyargs[0] = jit_type_nuint;

    /** @todo Keyword args */
    int nargs = m_positional_args.size();

    jit_type_t sig_call = _LJ_fn_signature(nargs);

    jit_type_t sig = jit_type_create_signature(jit_abi_cdecl,
                                               sig_call,
                                               tyargs,
                                               1,
                                               1);
    assert(sig);
    jit_value_t arg = m_callee->LJ_Codegen(func, f);
    jit_insn_mark_offset(func, m_bytecode_offset);
    jit_value_t callee = jit_insn_call_native(func, "FPyRuntime_CheckCall", (void*)&FPyRuntime_CheckCall, sig, &arg, 1, 0);
    assert(callee);
    /* Then call the function itself */

    jit_value_t *args = new jit_value_t[nargs];
    int i = 0;
    for(std::list<Value*>::iterator it = m_positional_args.begin();
        i < nargs;
        it++) {
        args[i++] = (*it)->LJ_Codegen(func, f);
    }

    return jit_insn_call_indirect(func, callee, sig_call, args, nargs, 0);
}

jit_value_t Compare::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);

    int vtable_idx = -1;
    switch(m_op) {
        case PyCmp_LT:
            vtable_idx = Object::idx__Lt__;
            break;
        case PyCmp_LE:
            vtable_idx = Object::idx__Le__;
            break;
        case PyCmp_EQ:
            vtable_idx = Object::idx__Eq__;
            break;
        case PyCmp_NE:
            vtable_idx = Object::idx__Ne__;
            break;
        case PyCmp_GT:
            vtable_idx = Object::idx__Gt__;
            break;
        case PyCmp_GE:
            vtable_idx = Object::idx__Ge__;
            break;
        case PyCmp_IS:
            return  jit_insn_eq(func,
                                m_args[0]->LJ_Codegen(func, f),
                                m_args[1]->LJ_Codegen(func, f));
        case PyCmp_IS_NOT:
            return  jit_insn_ne(func,
                                m_args[0]->LJ_Codegen(func, f),
                                m_args[1]->LJ_Codegen(func, f));

        case PyCmp_IN:
            vtable_idx = Object::idx__Contains__;
            break;
        case PyCmp_NOT_IN:
            vtable_idx = Object::idx__NotContains__;
            break;
        case PyCmp_EXC_MATCH: {
            return _LJ_Call(func, "FPyRuntime_ExceptionCompare", (void*)&FPyRuntime_ExceptionCompare,
                            m_args[0]->LJ_Codegen(func, f),
                            m_args[1]->LJ_Codegen(func, f));
        }
        default:
            std::cerr << "Unimplemented compare operation\n";
    }

    return _LJ_CallVtable(func,
                          vtable_idx,
                          m_args[0]->LJ_Codegen(func, f), m_args[1]->LJ_Codegen(func, f));
}

jit_value_t TestIfFalse::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);

    BasicBlock *b = f->LJ_GetCurrentBlock();

    jit_value_t eq_test = jit_insn_eq(func,
                                      m_args[0]->LJ_Codegen(func, f),
                                      Constant::GetBool(false)->LJ_Codegen(func, f));
    assert(eq_test);
    jit_insn_branch_if(func, eq_test, b->GetSuccessor(0)->LJ_GetLabel());
}

jit_value_t PrintItem::_LJ_Codegen(jit_function_t func, Function *f) {
//    jit_insn_mark_offset(func, m_bytecode_offset);
    return _LJ_Call(func, "FPyRuntime_PrintItem", (void*)&FPyRuntime_PrintItem, m_args[0]->LJ_Codegen(func, f));
}
jit_value_t PrintNewline::_LJ_Codegen(jit_function_t func, Function *f) {
//    jit_insn_mark_offset(func, m_bytecode_offset);
    return _LJ_Call(func, "FPyRuntime_PrintNewline", (void*)&FPyRuntime_PrintNewline);
}

jit_value_t BeginCatch_GetType::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);
    jit_value_t exc = f->LJ_GetExceptionObject();
    if(exc == 0) {
        exc = jit_insn_thrown_exception(func);
        f->LJ_SetExceptionObject(exc);
        _LJ_Call(func, "jit_exception_clear_last", (void*)&jit_exception_clear_last);
    }

    /**@todo Type objects */

    return exc;
}

jit_value_t BeginCatch_GetValue::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);
    jit_value_t exc = f->LJ_GetExceptionObject();
    if(exc == 0) {
        exc = jit_insn_thrown_exception(func);
        f->LJ_SetExceptionObject(exc);
        _LJ_Call(func, "jit_exception_clear_last", (void*)&jit_exception_clear_last);
    }

    return exc;
}

jit_value_t BeginCatch_GetTraceback::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);
    jit_value_t exc = f->LJ_GetExceptionObject();
    if(exc == 0) {
        exc = jit_insn_thrown_exception(func);
        f->LJ_SetExceptionObject(exc);
        _LJ_Call(func, "jit_exception_clear_last", (void*)&jit_exception_clear_last);
    }

    int off = offsetof(Exception, m_traceback);
    return jit_insn_load_relative(func, exc, off, jit_type_nuint);
}

jit_value_t ReRaise::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);
    jit_value_t exc = f->LJ_GetExceptionObject();
    if(exc == 0) {
        exc = jit_insn_thrown_exception(func);
        f->LJ_SetExceptionObject(exc);
        _LJ_Call(func, "jit_exception_clear_last", (void*)&jit_exception_clear_last);
    }

    _LJ_Call(func, "jit_exception_clear_last", (void*)&jit_exception_clear_last);
    jit_insn_rethrow_unhandled(func);
    return 0;
}

/* Tuple::Tuple(int) */
extern void *_ZN5TupleC1Ei;
/* Tuple::Set(long, Value*) */
extern void *_ZN5Tuple3SetElP6Object;

jit_value_t BuildTuple::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);
    /* Allocate the memory for the tuple - this is seperated so that escape analysis can decide where
       the memory should be allocated (heap or stack). */
    jit_value_t v = _LJ_AllocMemory(this, func, sizeof(Tuple));
    _LJ_Call(func, "_ZN5TupleC1Ei", (void*)&_ZN5TupleC1Ei, v, jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)m_n));
    for(int i = 0; i < m_n; i++) {
        _LJ_Call(func, "_ZN5Tuple3SetElP6Object", (void*)&_ZN5Tuple3SetElP6Object, v, jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)i), m_args[i]->LJ_Codegen(func, f));
    }
    return v;
}

/** Note the lack of underscore - these are never cached because they must
    not cross function boundaries! */
jit_value_t Constant::LJ_Codegen(jit_function_t func, Function *f) {
    return _LJ_Codegen(func, f);
}
jit_value_t Constant::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_value_t v = jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)this);
    assert(v);
    return v;
}
