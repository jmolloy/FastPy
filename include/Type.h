#ifndef TYPE_H
#define TYPE_H

#include <inttypes.h>

/** A type is a descriptor, and should not be inherited. It should be composed
    into classes that subclass Constant or Variable. */
class Type {
public:
    static Type *GetIntTy();
    static Type *GetInt64Ty();
    static Type *GetFloatTy();
    static Type *GetStringTy();
    static Type *GetByteStringTy();
    static Type *GetBoolTy();
    static Type *GetNoneTy();
    static Type *GetFrozenSetTy();
    static Type *GetSetTy();
    static Type *GetListTy();
    static Type *GetDictTy();
    static Type *GetTupleTy();
    static Type *GetCodeTy();
    static Type *GetCellTy();
    static Type *GetFunctionTy();

    static uint16_t TagFor(Type *t);
};

#endif
