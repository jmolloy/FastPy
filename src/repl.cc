#include <Marshal.h>
#include <Module.h>
#include <constants.h>
#include <Exception.h>

#include <python2.7/Python.h>
#include <jit/jit.h>

extern jit_context_t g_lj_ctx;

extern std::string get_statement();
extern "C" PyObject* PyMarshal_WriteObjectToString(PyObject *, int);

/** This function must be away from repl.c because readline's header
    includes a "Function" type that clashes with ours. */
void repl() {
    /* Skip initialization of signal handlers, please */
    Py_InitializeEx(0);

    g_lj_ctx = jit_context_create();

    Marshal m;

    while(true) {
        std::string s = get_statement();

        PyObject *obj = Py_CompileString(s.c_str(), "<stdin>", Py_single_input);
        if(!obj) {
            PyErr_Print();
            continue;
        }
        PyObject *str = PyMarshal_WriteObjectToString(obj, 2);
        char *c_str = PyString_AsString(str);
        int len = PyString_Size(str);

        std::string s_(c_str, len);
        Code *code = dynamic_cast<Code*>(m.ReadString(s_));
        assert(code);

        Module *__main__ = new Module("__main__", code);
        __main__->LJ_Codegen(g_lj_ctx);

        Object *result;
        jit_function_apply(__main__->GetMainFunction()->LJ_Codegen(g_lj_ctx),
                           0,
                           (void*)&result);
        if(jit_exception_get_last() != 0) {
            Exception *e = reinterpret_cast<Exception*>(jit_exception_get_last_and_clear());
            std::cerr << e->Repr() << std::endl;
        } else {
            if(result != Constant::GetNone()) {
                std::cout << result->Repr() << std::endl;
            }
        }

        Py_DECREF(obj);
        Py_DECREF(str);
        delete __main__;
    }
}
