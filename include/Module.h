#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <Function.h>
#include <variables.h>

/** A container class for module-level functions and classes. */
class Module {
public:
    Module(const std::string &name, Code *c);
    ~Module();

    Function *GetMainFunction() {
        return m_main;
    }

    const std::string &GetName() {
        return m_name;
    }
    
    Dict *GetGlobals() {
        return m_globals;
    }

    void LJ_Codegen(jit_context_t ctx);

    void Dump();

private:
    std::string m_name;
    Function *m_main;
    
    Dict *m_globals;
};

#endif
