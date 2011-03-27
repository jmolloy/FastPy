#include <constants.h>
#include <Marshal.h>

#include <string.h>
#include <stdlib.h>

#include <Function.h>
#include <Module.h>
#include <Exception.h>

#include <jit/jit.h>

#include <stdio.h>

jit_context_t g_lj_ctx;

int main(int argc, char **argv) {
    
    if(argc < 2) {
        std::cerr << "Error: No .pyc file specified!" << std::endl;
        exit(1);
    }

    /* Ghetto-tech argument parsing. */
    int i = 1;
    const char *fname = NULL;
    const char *dis_func = NULL;
    while(i < argc) {
        if(!strcmp(argv[i], "-dis")) {
            assert(argc > i+1);
            dis_func = argv[++i];
            i++;
            continue;
        }
        fname = argv[i];
        break;
    }

    if(fname == NULL) {
        std::cerr << "Error: No .pyc file specified!" << std::endl;
        exit(1);
    }

    Marshal m;
    Code *code = dynamic_cast<Code*>(m.ReadFile(std::string(fname)));
    assert(code);
    Module *__main__ = new Module("__main__", code);

//    code->Disassemble(std::cout);
    __main__->Dump();

    g_lj_ctx = jit_context_create();
    __main__->LJ_Codegen(g_lj_ctx);

//    exit(0);
    unsigned long result;
    jit_function_apply(__main__->GetMainFunction()->LJ_Codegen(g_lj_ctx),
                       0,
                       &result);
    if(jit_exception_get_last() != 0) {
        Exception *e = reinterpret_cast<Exception*>(jit_exception_get_last_and_clear());
        std::cerr << e->Repr() << std::endl;
    } 

#if 0
    Constant::GetBool(true)->Dump();

    ConstantInt *i = Constant::GetInt(5);
    i->SetId(1);
    i->SetProperty("ohai", "rawr");
    i->Dump();
#endif
    return 0;
}
