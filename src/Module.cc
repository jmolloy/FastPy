#include <Module.h>
#include <constants.h>

#include <sstream>
#include <stdio.h>

#include <jit/jit-dump.h>

#include <PyRuntime.h>

Module::Module(const std::string &name, Code *c) :
    m_name(name),
    m_globals(PyDict_Create()) {
    std::stringstream ss;
    ss << name << "_main";

    m_main = new Function(ss.str(), c, this);
    c->Walk(m_main);

    PopulateDictWithBuiltins(m_globals);
}

Module::~Module() {
    delete m_main;
}

void Module::Dump() {
    std::cerr << "module " << m_name << "\n";
    m_main->Dump();
}

void Module::LJ_Codegen(jit_context_t ctx) {
    jit_function_t f = m_main->LJ_Codegen(ctx);
    jit_dump_function(stdout, f, m_main->GetName().c_str());
}
