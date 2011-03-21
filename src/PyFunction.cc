#include <PyFunction.h>
#include <Type.h>

PyFunction *PyFunction_Create(void *fn) {
    PyFunction *f = new PyFunction();
    f->tag = Type::TagFor(Type::GetFunctionTy());
    f->fn = fn;

    return f;

}
