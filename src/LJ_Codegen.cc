#include <instructions.h>
#include <constants.h>
#include <jit/jit.h>

#include <PyDict.h>
#include <PyString.h>
#include <PyNone.h>
#include <PyRuntime.h>
#include <Module.h>

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

static jit_value_t _LJ_CallVtable(jit_function_t func, int idx, jit_value_t obj, jit_value_t p1, jit_value_t p2) {
    jit_value_t vtable_ptr = jit_insn_load_relative(func, obj, 0, jit_type_create_pointer(jit_type_nuint, 1));
    jit_value_t fnp = jit_insn_load_elem(func,
                                         vtable_ptr,
                                         jit_value_create_nint_constant(func, jit_type_nuint, idx),
                                         jit_type_nuint);
    
    jit_value_t args[4];
    args[0] = fnp;
    args[1] = obj;
    args[2] = p1;
    args[3] = p2;
    return jit_insn_call_native(func, "FPyRuntime_CallC_LJ", (void*)&FPyRuntime_CallC_LJ, _LJ_fn_signature(3), args, 4, 0);
}


jit_value_t LoadGlobal::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_value_t g = jit_value_create_nint_constant(func, jit_type_nuint, (jit_nint)f->GetModule()->GetGlobals());
    
    jit_value_t hash_value;
    if(dynamic_cast<Constant*>(m_args[0]) != NULL) {
        hash_value = jit_value_create_nint_constant(func, jit_type_nuint, 1);
    } else {
        hash_value = jit_value_create_nint_constant(func, jit_type_nuint, 2);
    }

    jit_insn_mark_offset(func, m_bytecode_offset);

    return _LJ_CallVtable(func,
                          Object::idx__Subscr__,
                          g, hash_value, m_args[0]->LJ_Codegen(func, f));
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

    jit_value_t arg = m_callee->LJ_Codegen(func, f);
    jit_insn_mark_offset(func, m_bytecode_offset);
    jit_value_t callee = jit_insn_call_native(func, "FPyRuntime_CheckCall", (void*)&FPyRuntime_CheckCall, sig, &arg, 1, 0);

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

jit_value_t PrintItem::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);
    return _LJ_Call(func, "FPyRuntime_PrintItem", (void*)&FPyRuntime_PrintItem, m_args[0]->LJ_Codegen(func, f));
}
jit_value_t PrintNewline::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_mark_offset(func, m_bytecode_offset);
    return _LJ_Call(func, "FPyRuntime_PrintNewline", (void*)&FPyRuntime_PrintNewline);
}

jit_value_t ConstantNone::_LJ_Codegen(jit_function_t func, Function *f) {
    return jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)this);
}
jit_value_t ConstantString::_LJ_Codegen(jit_function_t func, Function *f) {
    return jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)this);
}
