#include <Traceback.h>
#include <jit/jit.h>
#include <execinfo.h>
#include <Function.h>
#include <string.h>
#include <cxxabi.h>
#include <constants.h>

extern jit_context_t g_lj_ctx;
extern bool g_db_traceback_builtins;

void Traceback::AddTraceFromHere() {
    m_stack_trace = jit_exception_get_stack_trace();
}

const std::string Traceback::Repr() {
    std::stringstream ss;
    /* Skip the top two frames as they are our and the exception's constructor. */
    for(int i = jit_stack_trace_get_size(m_stack_trace)-1; i >= 2 ; i--) {
        if(jit_stack_trace_get_function(g_lj_ctx, m_stack_trace, i) != NULL) {

            jit_function_t func = jit_stack_trace_get_function(g_lj_ctx, m_stack_trace, i);

            int bytecode_offset = jit_stack_trace_get_offset(g_lj_ctx, m_stack_trace, i);
            Function *f = (Function*)jit_function_get_meta(func, 0);
            int lno = f->GetCode()->GetLineNo(bytecode_offset);


            ss << "  File \"" << f->GetCode()->m_filename->str() << "\", line " << lno << ", in " << f->GetCode()->m_name->str() << "\n";
#if defined(TRACEBACK_SHOW_BUILTINS)
        } else if(g_db_traceback_builtins) {
            void *ptr = jit_stack_trace_get_pc(m_stack_trace, i);
            char **syms = backtrace_symbols(&ptr, 1);
            char *sym = syms[0];

            /* sym will look something like:
               ./fastpy(_ZN6Object10__Subscr__EPS_+0x55) [0x46e6b]

               We want it to look like:
               Object::__Subscr__() */
            int idx = 0;
            for(idx = 0; idx < strlen(sym); ++idx) {
                if(sym[idx] == '(') {
                    ++idx;
                    break;
                }
            }
            for(int idx2 = 0; idx2 < strlen(sym); ++idx2) {
                if(sym[idx2] == '+') {
                    sym[idx2] = 0;
                    break;
                }
            }

            if(idx >= strlen(sym)) {
                ss << "  <built-in> " << sym << "\n";
            } else {
                int status = -1;
                char *realname = abi::__cxa_demangle(&sym[idx], NULL, 0, &status);
                if(status != 0) {
                    ss << "  <built-in> " << &sym[idx] << "\n";
                } else {
                    ss << "  <built-in> " << realname << "\n";
                    free(realname);
                }
            }
            free(syms);
#endif
        }
    }
    return ss.str();
}
