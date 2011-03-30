#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Constant.h>
#include <Type.h>
#include <set>
#include <cassert>

class Tuple;
class Function;

class ConstantInt : public Constant {
public:
    ConstantInt(long n) : m_n(n) {
        SetType(Type::GetIntTy());
    }
    virtual const std::string Repr();
    long Get() {
        return m_n;
    }
//    virtual Object *__Compare__(Object *other);
private:
    long m_n;
};
class ConstantInt64 : public Constant {
public:
    ConstantInt64(int64_t n) : m_n(n) {
        SetType(Type::GetInt64Ty());
    }
    virtual const std::string Repr();
private:
    int64_t m_n;
};
class ConstantFloat : public Constant {
public:
    ConstantFloat(double n) : m_n(n) {
        SetType(Type::GetFloatTy());
    }
    virtual const std::string Repr();
private:
    double m_n;
};
class ConstantString : public Constant {
public:
    ConstantString(const std::string &str) : m_str(str) {
        SetType(Type::GetStringTy());
    }
    virtual const std::string Repr();

    static ConstantString *From(Object *o) {
        ConstantString *s = dynamic_cast<ConstantString*>(o);
        assert(s);
        return s;
    }

    std::string str() {
        return m_str;
    }

    virtual Object *__Cmp__(Object *idx);

private:
    const std::string m_str;
};
class ConstantByteString : public Constant {
public:
    ConstantByteString(int sz) : m_sz(sz) {
        SetType(Type::GetByteStringTy());
        m_ptr = new unsigned char[sz];
    }
    ~ConstantByteString() {
        delete [] m_ptr;
    }

    unsigned char *GetPtr() {
        return m_ptr;
    }

    virtual const std::string Repr();

    unsigned char *m_ptr;
    int m_sz;
};
class ConstantBool : public Constant {
public:
    ConstantBool(bool b) : m_b(b) {
        SetType(Type::GetBoolTy());
    }
    virtual const std::string Repr();
private:
    bool m_b;
};
class ConstantNone : public Constant {
public:
    ConstantNone() {
        SetType(Type::GetNoneTy());
    }
    virtual const std::string Repr();
};
class FrozenSet : public Constant {
public:
    FrozenSet() {
        SetType(Type::GetFrozenSetTy());
    }
    virtual const std::string Repr();

    void Insert(Value *v) {
        m_s.insert(v);
    }
private:
    std::set<Value*> m_s;
};

class Code : public Constant {
public:
    Code(long argcount, long kwonlyargcount, long nlocals, long stacksize,
         long flags, ConstantByteString *code, Tuple *consts, Tuple *names,
         Tuple *varnames, Tuple *freevars, Tuple *cellvars, ConstantString *filename,
         ConstantString *name, long firstlineno, ConstantByteString *lnotab);

    virtual const std::string Repr();

    void Disassemble(std::ostream &s);

    unsigned char NextOp(long &i, long &arg);

    void Walk(Function *f);

    int GetLineNo(int offs);

    long m_argcount;
    long m_kwonlyargcount;
    long m_nlocals;
    long m_stacksize;
    long m_flags;
    ConstantByteString *m_code;
    Tuple *m_consts;
    Tuple *m_names;
    Tuple *m_varnames;
    Tuple *m_freevars;
    Tuple *m_cellvars;
    ConstantString *m_filename;
    ConstantString *m_name;
    long m_firstlineno;
    ConstantByteString *m_lnotab;
};

#endif
