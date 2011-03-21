#include <Function.h>
#include <BasicBlock.h>
#include <variables.h>
#include <constants.h>
#include <sstream>
#include <Module.h>

std::map<std::string, Function*> g_functions;

Function::Function(std::string name, Code *code, Module *module) :
    m_name(name), m_code(code), m_module(module), m_jit_function(0) {
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

    g_functions[name] = this;
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

    GetEntryBlock()->LJ_Codegen();
    for(std::vector<BasicBlock*>::iterator it = m_blocks.begin();
        it != m_blocks.end();
        ++it) {
        (*it)->LJ_Codegen();
    }

    jit_function_compile(m_jit_function);

    return m_jit_function;
}
