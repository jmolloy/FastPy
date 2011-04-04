#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <Instruction.h>
#include <variables.h>

#include <Function.h>

#include <list>
#include <map>

class StoreGlobal : public Instruction {
public:
    StoreGlobal(Value *global, Value *val, int id) :
        Instruction("store-global", id) {
        m_args.push_back(global);
        m_args.push_back(val);
    }

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
#if defined(WITH_LLVM)
    virtual llvm::Value *_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
#endif
};
class StoreLocal : public Instruction {
public:
    StoreLocal(Value *local, Value *val, int id) :
        Instruction("store-local", id) {
        m_args.push_back(local);
        m_args.push_back(val);
    }
};
class LoadGlobal : public Instruction {
public:
    LoadGlobal(Value *global, int id) :
        Instruction("load-global", id) {
        m_args.push_back(global);
    }

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
#if defined(WITH_LLVM)
    virtual llvm::Value *_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
#endif
};
class LoadLocal : public Instruction {
public:
    LoadLocal(Value *local, int id) :
        Instruction("load-local", id) {
        m_args.push_back(local);
    }
};
class Call : public Instruction {
public:
    Call(Value *callee, std::list<Value*> &positional_args, std::map<std::string,Value*> &keyword_args, int id) :
        Instruction("call", id),
        m_callee(callee), m_positional_args(positional_args),
        m_keyword_args(keyword_args) {
    }

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);

    virtual const std::string Repr();

    Value *m_callee;
    std::list<Value*> m_positional_args;
    std::map<std::string, Value*> m_keyword_args;
};
class Return : public Instruction {
public:
    Return(Value *ret, int id) :
        Instruction("return", id) {
        m_args.push_back(ret);
    }

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
#if defined(WITH_LLVM)
    virtual llvm::Value *_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
#endif
};
class TestIfFalse : public Instruction {
public:
    TestIfFalse(Value *val, int id) :
        Instruction("false?", id) {
        m_args.push_back(val);
    }
    
    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
#if defined(WITH_LLVM)
    virtual llvm::Value *_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
#endif
};
class TestIfTrue : public Instruction {
public:
    TestIfTrue(Value *val, int id) :
        Instruction("true?", id) {
        m_args.push_back(val);
    }
};
class GetAttr : public Instruction {
public:
    GetAttr(Value *obj, std::string op, int id) :
        Instruction("getattr", id),
        m_obj(obj), m_op(op) {
    }

    virtual const std::string Repr();

    Value *m_obj;
    std::string m_op;
};
class BuildTuple : public Instruction {
public:
    BuildTuple(int n, int id) :
        Instruction("build-tuple", id),
        m_n(n) {
    }

    void Push(Value *v) {
        m_args.push_back(v);
    }

    int m_n;

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
};
class BeginCatch_GetType : public Instruction {
public:
    BeginCatch_GetType(int id) :
        Instruction("begin-catch-get-type", id) {
    }

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
};
class BeginCatch_GetValue : public Instruction {
public:
    BeginCatch_GetValue(int id) :
        Instruction("begin-catch-get-value", id) {
    }

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
#if defined(WITH_LLVM)
    virtual llvm::Value *_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
#endif
};
class BeginCatch_GetTraceback : public Instruction {
public:
    BeginCatch_GetTraceback(int id) :
        Instruction("begin-catch-get-traceback", id) {
    }

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
};
class Compare : public Instruction {
public:
    Compare(int op, Value *v1, Value *v2, int id) :
        Instruction("compare", id), m_op((cmp_op)op) {
        m_args.push_back(v1);
        m_args.push_back(v2);
    }

    virtual const std::string Repr();

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
#if defined(WITH_LLVM)
    virtual llvm::Value *_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
#endif

    enum cmp_op {PyCmp_LT, PyCmp_LE, PyCmp_EQ, PyCmp_NE, PyCmp_GT, PyCmp_GE,
                 PyCmp_IN, PyCmp_NOT_IN, PyCmp_IS, PyCmp_IS_NOT, PyCmp_EXC_MATCH,
                 PyCmp_BAD};

    cmp_op m_op;
};
class PrintItem : public Instruction {
public:
    PrintItem(Value *v, int id) :
        Instruction("print-item", id) {
        m_args.push_back(v);
    }
    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
#if defined(WITH_LLVM)
    virtual llvm::Value *_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
#endif
};
class PrintNewline : public Instruction {
public:
    PrintNewline(int id) :
        Instruction("print-newline", id) {
    }
    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
#if defined(WITH_LLVM)
    virtual llvm::Value *_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f);
#endif
};
class ReRaise : public Instruction {
public:
    ReRaise(int id) :
        Instruction("reraise", id) {
    }

    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f);
};
class Phi : public Instruction {
public:
    Phi(int id) :
        Instruction("phi", id) {
    }

    void AddIncoming(Value *v, BasicBlock *src) {
        m_srcs.push_back(std::make_pair(v, src));
    }

    virtual const std::string Repr();

    std::list< std::pair<Value*,BasicBlock*> > m_srcs;
};

#endif
