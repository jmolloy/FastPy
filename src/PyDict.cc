#include <PyDict.h>
#include <PyString.h>
#include <Type.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FPyDict *FPyDict_Create() {
    FPyDict *d = new FPyDict();
    d->tag = Type::TagFor(Type::GetDictTy());
    return d;
}

FPyObject *FPyDict_Lookup(FPyDict *dict, unsigned long hash, FPyObject *var) {
    //fprintf(stderr, "FPyDict_Lookup(%p, %lx, %p)\n", dict, hash, var);

    if(dict->m.find(var) != dict->m.end()) {
        return dict->m[var];
    }

    /**@todo Throw exception */
    abort();
    return 0;
}

void FPyDict_Insert(FPyDict *dict, FPyObject *key, FPyObject *value) {
    //fprintf(stderr, "FPyDict_Insert(%p, %p, %p)\n", dict, key, value);
    dict->m[key] = value;
}

bool FPyDict_Compare(FPyObject *obj, FPyObject *obj2) {
    static uint16_t str_ty = Type::TagFor(Type::GetStringTy());

    if(obj->tag == str_ty && obj2->tag == str_ty) {
        return ! (strcmp( ((FPyString*)obj)->str,
                          ((FPyString*)obj2)->str ) == 0);
    } else {
        /// @todo __cmp__ or __eq__
        return ! (obj == obj2);
    }
}
