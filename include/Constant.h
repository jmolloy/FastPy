#ifndef CONSTANT_H
#define CONSTANT_H

#include <Value.h>
#include <inttypes.h>
#include <cassert>
#include <sstream>

/** A constant is a Value in that it is a piece of data and can be passed around as such. */
class Constant : public Value {
public:
    static class ConstantBool   *GetBool(bool b);
    static class ConstantInt    *GetInt(long n);
    static class ConstantInt64  *GetInt64(int64_t n);
    static class ConstantFloat  *GetFloat(double n);
    static class ConstantString *GetString(const std::string &str);
    static class ConstantNone   *GetNone();

    
    // Constants are simple enough that we should show their actual value.
    virtual std::string RefRepr() {
        return Repr();
    }

    static Constant *From(Object *o) {
        Constant *s = dynamic_cast<Constant*>(o);
        assert(s);
        return s;
    }

    virtual jit_value_t LJ_Codegen(jit_function_t func, Function *f);
    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);

#if defined(WITH_LLVM)
    llvm::Value *LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
    llvm::Value *_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
#endif

protected:
    virtual char GetWart() {
        return '@';
    }
};

#endif
