#include <Constant.h>
#include <constants.h>
#include <map>

ConstantBool *Constant::GetBool(bool b) {
    static ConstantBool t(true);
    static ConstantBool f(false);

    return (b) ? &t : &f;
}

ConstantInt *Constant::GetInt(int n) {
    static std::map<int,ConstantInt*> m;
    ConstantInt *c = m[n];
    if(!c) {
        c = new ConstantInt(n);
        m[n] = c;
    }
    return c;
}
ConstantInt64 *Constant::GetInt64(int64_t n) {
    static std::map<int64_t,ConstantInt64*> m;
    ConstantInt64 *c = m[n];
    if(!c) {
        c = new ConstantInt64(n);
        m[n] = c;
    }
    return c;
}
ConstantFloat *Constant::GetFloat(double n) {
    static std::map<double,ConstantFloat*> m;
    ConstantFloat *c = m[n];
    if(!c) {
        c = new ConstantFloat(n);
        m[n] = c;
    }
    return c;
}
ConstantString *Constant::GetString(const std::string &str) {
    static std::map<std::string,ConstantString*> m;
    ConstantString *c = m[str];
    if(!c) {
        c = new ConstantString(str);
        m[str] = c;
    }
    return c;
}
ConstantNone *Constant::GetNone() {
    static ConstantNone n;
    return &n;
}
