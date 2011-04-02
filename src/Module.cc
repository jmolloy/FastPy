#include <Module.h>
#include <constants.h>

#include <sstream>
#include <stdio.h>

#include <jit/jit-dump.h>

#include <PyRuntime.h>
#include <db.h>

Module::Module(const std::string &name, Code *c) :
    m_name(name),
    m_globals(new Dict()) {
    std::stringstream ss;
    ss << name << ".main";

    m_main = new Function(ss.str(), c, this);
    c->Walk(m_main);

    if(db_print(m_main, DB_PRINT_BYTECODE)) {
        std::cout << "*** Function " << ss.str() << " - Bytecode\n";
        c->Disassemble(std::cout);
        std::cout << "*** End Function " << ss.str() << " - Bytecode\n";
    }
    if(db_print(m_main, DB_PRINT_IR)) {
        std::cout << "*** Function " << ss.str() << " - IR\n";
        std::cout << m_main->Repr() << "\n";
        std::cout << "*** End Function " << ss.str() << " - IR\n";
    }

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
}
#if defined(WITH_LLVM)
void Module::LLVM_Codegen(llvm::Module *m) {
    llvm::Function *f = m_main->LLVM_Codegen(m);
}
#endif
