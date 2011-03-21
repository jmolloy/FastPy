#include <PyString.h>
#include <Type.h>

#include <string.h>

PyString *PyString_Create(const char *str) {
    str = strdup(str);

    PyString *x = new PyString();
    x->tag = Type::TagFor(Type::GetStringTy());
    x->str = str;

    return x;
}
