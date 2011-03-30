#include <Function.h>
#include <BasicBlock.h>
#include <variables.h>
#include <constants.h>
#include <sstream>
#include <Module.h>

#include <stdlib.h>

#include <db.h>

#include <jit/jit-dump.h>

extern jit_context_t g_lj_ctx;

Function::Function(std::string name, Code *code, Module *module) :
    m_name(name), m_code(code), m_module(module), m_jit_function(0),
    m_current_block(0), m_lj_exception_object(NULL) {
    m_entry_block = new BasicBlock(this);

#if 0
    /**@todo kw args */
    int i = 0;
    for(std::vector<Object*>::iterator it = code->m_varnames.begin();
        it != code->m_varnames.end();
        it++) {
        if(i == code->m_argcount) {
            break;
        }
        
        m_arguments.push_back(dynamic_cast<String*>(*it)->m_str);
        i++;
    }
#endif
}

Function::~Function() {
    delete m_entry_block;
}

const std::string Function::Repr() {
    std::stringstream ss;
    ss << "function " << m_name << " {\n";

    ss << GetEntryBlock()->Repr() << "\n";
    for(std::vector<BasicBlock*>::iterator it = m_blocks.begin();
        it != m_blocks.end();
        ++it) {
        ss << (*it)->Repr() << "\n";
    }

    ss << "}";
    return ss.str();
}

jit_function_t Function::LJ_Codegen(jit_context_t ctx) {
    if(m_jit_function != 0) {
        return m_jit_function;
    }

    jit_context_build_start(ctx);

    /**@todo Types */
    int nargs = m_code->m_argcount + m_code->m_kwonlyargcount;
    jit_type_t *args = new jit_type_t[nargs];
    for(int i = 0; i < nargs; i++) {
        args[i] = jit_type_nuint;
    }
    jit_type_t signature = jit_type_create_signature(jit_abi_cdecl,
                                                     jit_type_nuint,
                                                     args,
                                                     nargs,
                                                     1);
    delete [] args;

    m_jit_function = jit_function_create(ctx, signature);
    jit_function_set_meta(m_jit_function, 0, (void*)this, NULL, 0);

    bool uses_catcher = false;
    for(std::vector<BasicBlock*>::iterator it = m_blocks.begin();
        it != m_blocks.end();
        ++it) {
        if((*it)->GetUnwindBlock()) {
            jit_insn_uses_catcher(m_jit_function);
            uses_catcher = true;
            break;
        }
    }

    m_current_block = GetEntryBlock();
    GetEntryBlock()->LJ_Codegen();
    for(std::vector<BasicBlock*>::iterator it = m_blocks.begin();
        it != m_blocks.end();
        ++it) {
        m_current_block = *it;
        (*it)->LJ_Codegen();
    }

    if(uses_catcher) {
        jit_insn_start_catcher(m_jit_function);

        for(std::vector<BasicBlock*>::iterator it = m_blocks.begin();
            it != m_blocks.end();
            ++it) {
            if((*it)->GetUnwindBlock()) {
                jit_insn_branch_if_pc_not_in_range(m_jit_function,
                                                   *(*it)->LJ_GetLabel(),
                                                   *(*it)->LJ_GetEndLabel(),
                                                   (*it)->GetUnwindBlock()->LJ_GetLabel());
            }
            jit_insn_rethrow_unhandled(m_jit_function);
        }

    }

    if(db_print(this, DB_PRINT_LJ)) {
        std::cout << "*** Function " << m_name << " - Libjit\n";
        jit_dump_function(stdout, m_jit_function, GetName().c_str());
        std::cout << "*** End Function " << m_name << "\n";
    }

    jit_function_compile(m_jit_function);

    if(db_print(this, DB_PRINT_ASM)) {
        std::cout << "*** Function " << m_name << " - Asm\n";
        jit_dump_function(stdout, m_jit_function, GetName().c_str());
        std::cout << "*** End Function " << m_name << " - Asm\n";
    }

    jit_context_build_end(ctx);

    return m_jit_function;
}

void *Function::GetFnPtr() {
    return jit_function_to_closure(LJ_Codegen(g_lj_ctx));
}

const std::string BuiltinFunction::Repr() {
    return "<built-in function>";
}
