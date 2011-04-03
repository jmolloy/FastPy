#ifndef DB_H
#define DB_H

#include <set>

#define DB_PRINT_BYTECODE 1
#define DB_PRINT_IR       2
#define DB_PRINT_LJ       4
#define DB_PRINT_LLVM     8
#define DB_PRINT_ASM      16
#define DB_PRINT_C_CALLS  32

bool db_print(Function *fn, int type);

extern int g_db_print;
extern bool g_force_llvm;
extern bool g_db_traceback_builtins;
extern std::set<std::string> g_db_funcs;
extern std::set<std::string> g_db_phases;

#endif
