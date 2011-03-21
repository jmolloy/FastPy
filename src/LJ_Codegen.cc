#include <instructions.h>
#include <constants.h>
#include <jit/jit.h>

#include <PyDict.h>
#include <PyString.h>
#include <PyNone.h>
#include <PyRuntime.h>
#include <Module.h>

#include <stdio.h>

jit_value_t LoadGlobal::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_value_t g = jit_value_create_nint_constant(func, jit_type_nuint, (jit_nint)f->GetModule()->GetGlobals());
    
    jit_value_t hash_value;
    if(dynamic_cast<Constant*>(m_args[0]) != NULL) {
        hash_value = jit_value_create_nint_constant(func, jit_type_nuint, 1);
    } else {
        hash_value = jit_value_create_nint_constant(func, jit_type_nuint, 2);
    }

    jit_type_t tyargs[3];
    tyargs[0] = jit_type_nuint;
    tyargs[1] = jit_type_nuint;
    tyargs[2] = jit_type_nuint;

    jit_type_t sig = jit_type_create_signature(jit_abi_cdecl,
                                               jit_type_nuint,
                                               tyargs,
                                               3,
                                               1);

    jit_value_t args[3];
    args[0] = g;
    args[1] = hash_value;
    args[2] = m_args[0]->LJ_Codegen(func, f);
    jit_value_t call = jit_insn_call_native(func, "PyDict_lookup", (void*)&PyDict_Lookup, sig, args, 3, 0);

    return call;
}

jit_value_t Return::_LJ_Codegen(jit_function_t func, Function *f) {
    jit_insn_return(func, m_args[0]->LJ_Codegen(func, f));
    return NULL;
}

jit_value_t Call::_LJ_Codegen(jit_function_t func, Function *f) {
    /* Call out to an external helper that will provide type checking */
    jit_type_t tyargs[1];
    tyargs[0] = jit_type_nuint;

    /** @todo Keyword args */
    int nargs = m_positional_args.size();
    jit_type_t *tyargs2 = new jit_type_t[nargs];
    for(int i = 0; i < nargs; i++) {
        tyargs2[i] = jit_type_nuint;
    }

    jit_type_t sig_call = jit_type_create_signature(jit_abi_cdecl,
                                    jit_type_nuint,
                                    tyargs2,
                                    nargs,
                                    1);

    delete [] tyargs2;


    jit_type_t sig = jit_type_create_signature(jit_abi_cdecl,
                                               sig_call,
                                               tyargs,
                                               1,
                                               1);

    jit_value_t arg = m_callee->LJ_Codegen(func, f);
    jit_value_t callee = jit_insn_call_native(func, "PyRuntime_CheckCall", (void*)&PyRuntime_CheckCall, sig, &arg, 1, 0);

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

jit_value_t ConstantNone::_LJ_Codegen(jit_function_t func, Function *f) {
    static PyObject *obj = PyNone_Create();
    return jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)obj);
}
jit_value_t ConstantString::_LJ_Codegen(jit_function_t func, Function *f) {
    PyObject *obj = PyString_Create(str().c_str());
    return jit_value_create_nint_constant(func, jit_type_nuint, (jit_nuint)obj);
}
