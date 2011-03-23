#include <PyFunction.h>
#include <Type.h>

FPyFunction *FPyFunction_Create(void *fn) {
    FPyFunction *f = new FPyFunction();
    f->tag = Type::TagFor(Type::GetFunctionTy());
    f->fn = fn;

    return f;

}
