#include <Function.h>
#include <set>

int g_db_print = 0;
bool g_db_traceback_builtins = false;
bool g_force_llvm = false;
std::set<std::string> g_db_funcs;
std::set<std::string> g_db_phases;

bool db_print(Function *fn, int type) {
    return (g_db_print & type) && (!fn ||
        (g_db_funcs.find(fn->GetName()) != g_db_funcs.end() ||
         g_db_funcs.find("all") != g_db_funcs.end()));
}
