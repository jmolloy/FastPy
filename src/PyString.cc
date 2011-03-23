#include <PyString.h>
#include <Type.h>

#include <string.h>

FPyString *FPyString_Create(const char *str) {
    str = strdup(str);

    FPyString *x = new FPyString();
    x->tag = Type::TagFor(Type::GetStringTy());
    x->str = str;

    return x;
}
