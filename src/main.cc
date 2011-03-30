#include <constants.h>
#include <Marshal.h>

#include <string.h>
#include <stdlib.h>

#include <Function.h>
#include <Module.h>
#include <Exception.h>

#include <jit/jit.h>

#include <stdio.h>
#include <getopt.h>
#include <db.h>

jit_context_t g_lj_ctx;

int g_db_print = 0;
std::set<std::string> g_db_funcs;
std::set<std::string> g_db_phases;

bool db_print(Function *fn, int type) {
    return (g_db_print & type) && (!fn ||
        (g_db_funcs.find(fn->GetName()) != g_db_funcs.end() ||
         g_db_funcs.find("all") != g_db_funcs.end()));
}

extern char *optarg;
extern int optind;

int main(int argc, char **argv) {
   

    struct option options[4];
    options[0].name = "db-print";
    options[0].has_arg = required_argument;
    options[0].flag = 0;
    options[0].val = 0;

    options[1].name = "db-func";
    options[1].has_arg = required_argument;
    options[1].flag = 0;
    options[1].val = 0;

    options[2].name = "db-phase";
    options[2].has_arg = required_argument;
    options[2].flag = 0;
    options[2].val = 0;

    memset(&options[3], 0, sizeof(struct option));

    int indexptr;
    char ret;
    while((ret=getopt_long(argc, argv, "", options, &indexptr)) != -1) {
        if(ret == '?') {
            exit(1);
        }
        if(!strcmp(options[indexptr].name, "db-print")) {
            if(!strcmp(optarg, "bytecode")) {
                g_db_print |= DB_PRINT_BYTECODE;
            } else if(!strcmp(optarg, "ir")) {
                g_db_print |= DB_PRINT_IR;
            } else if(!strcmp(optarg, "lj")) {
                g_db_print |= DB_PRINT_LJ;
            } else if(!strcmp(optarg, "llvm")) {
                g_db_print |= DB_PRINT_LLVM;
            } else if(!strcmp(optarg, "asm")) {
                g_db_print |= DB_PRINT_ASM;
            } else if(!strcmp(optarg, "c-calls")) {
                g_db_print |= DB_PRINT_C_CALLS;
            } else if(!strcmp(optarg, "all")) {
                g_db_print = ~0;
            } else {
                fprintf(stderr, "Unrecognised --db-print option: %s\nRecognised options are: ir lj llvm asm c-calls all\n", optarg);
                exit(1);
            }
        } else if(!strcmp(options[indexptr].name, "db-func")) {
            g_db_funcs.insert(std::string(optarg));
        } else if(!strcmp(options[indexptr].name, "db-phase")) {
            g_db_phases.insert(std::string(optarg));
        } else {
            abort();
        }
    }

    if(optind >= argc) {
        std::cerr << "Error: No .pyc file specified!" << std::endl;
        exit(1);
    }

    const char *fname = argv[optind];
    if(fname == NULL) {
        std::cerr << "Error: No .pyc file specified!" << std::endl;
        exit(1);
    }

    Marshal m;
    Code *code = dynamic_cast<Code*>(m.ReadFile(std::string(fname)));
    assert(code);
    Module *__main__ = new Module("__main__", code);

    g_lj_ctx = jit_context_create();
    __main__->LJ_Codegen(g_lj_ctx);

    unsigned long result;
    jit_function_apply(__main__->GetMainFunction()->LJ_Codegen(g_lj_ctx),
                       0,
                       &result);
    if(jit_exception_get_last() != 0) {
        Exception *e = reinterpret_cast<Exception*>(jit_exception_get_last_and_clear());
        std::cerr << e->Repr() << std::endl;
    } 

    return 0;
}
