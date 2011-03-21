#include <Type.h>
#include <map>

uint16_t Type::TagFor(Type *t) {
    static std::map<Type*,uint16_t> m;
    static int n = 0;

    if(m[t] == 0) {
        m[t] = n++;
    }
    return m[t];
}

Type *Type::GetIntTy() {
    static Type t;
    return &t;
}
Type *Type::GetInt64Ty() {
    static Type t;
    return &t;
}
Type *Type::GetFloatTy() {
    static Type t;
    return &t;
}
Type *Type::GetStringTy() {
    static Type t;
    return &t;
}
Type *Type::GetByteStringTy() {
    static Type t;
    return &t;
}
Type *Type::GetBoolTy() {
    static Type t;
    return &t;
}
Type *Type::GetNoneTy() {
    static Type t;
    return &t;
}
Type *Type::GetFrozenSetTy() {
    static Type t;
    return &t;
}
Type *Type::GetSetTy() {
    static Type t;
    return &t;
}
Type *Type::GetListTy() {
    static Type t;
    return &t;
}
Type *Type::GetTupleTy() {
    static Type t;
    return &t;
}
Type *Type::GetDictTy() {
    static Type t;
    return &t;
}
Type *Type::GetCodeTy() {
    static Type t;
    return &t;
}
Type *Type::GetCellTy() {
    static Type t;
    return &t;
}

Type *Type::GetFunctionTy() {
    static Type t;
    return &t;
}
