#ifndef PY_DICT_H
#define PY_DICT_H

#include <PyObject.h>

#include <map>

bool FPyDict_Compare(FPyObject *obj, FPyObject *obj2);

struct _FPyDict_Compare {
    bool operator()(FPyObject *obj, FPyObject *obj2) {
        return FPyDict_Compare(obj, obj2);
    }
};

/** A PyDict is a Dict that is actually accessed and addressed by python code,
    unlike Dict : public Constant, that is just for marshalling and codegen.*/
struct FPyDict : public FPyObject {
    std::map<FPyObject*,FPyObject*, _FPyDict_Compare> m;
};

/** An attribute dict is a specialised version of a dict where certain special members
    can be accessed solely by a simple array access. */
struct FPyAttributeDict {
    enum SpecialCases {
        __hash__,
        __repr__,
        __str__,
        __cmp__,
        __END__
    };
    FPyObject *specials[__END__];
    std::map<FPyObject*,FPyObject*, _FPyDict_Compare> m;
};

extern "C" {

FPyDict *FPyDict_Create();
FPyObject *FPyDict_Lookup(FPyDict *dict, unsigned long hash, FPyObject *var);
void FPyDict_Insert(FPyDict *dict, FPyObject *key, FPyObject *value);

FPyDict *FPyAttributeDict_Create();

}

#endif

