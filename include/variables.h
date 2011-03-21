#ifndef VARIABLES_H
#define VARIABLES_H

#include <Variable.h>
#include <Type.h>
#include <set>
#include <map>
#include <vector>
#include <cassert>
#include <iostream>

class Set : public Variable {
public:
    Set() {
        SetType(Type::GetSetTy());
    }
    virtual const std::string Repr();

    void Insert(Value *v) {
        m_s.insert(v);
    }
private:
    std::set<Value*> m_s;
};

class List : public Variable {
public:
    List(int n) : m_l(n) {
        SetType(Type::GetListTy());
    }
    virtual const std::string Repr();

    void Set(int i, Value *v) {
        m_l[i] = v;
    }
private:
    std::vector<Value*> m_l;
};

class Tuple : public Variable {
public:
    Tuple(int n) : m_l(n) {
        SetType(Type::GetTupleTy());
    }
    virtual const std::string Repr();

    static Tuple *From(Object *o) {
        Tuple *t = dynamic_cast<Tuple*>(o);
        assert(t);
        return t;
    }

    void Set(long i, Value *v) {
        m_l[i] = v;
    }
    Value *Get(int idx) {
        return m_l[idx];
    }
private:
    std::vector<Value*> m_l;
};

class Dict : public Variable {
public:
    Dict() {
        SetType(Type::GetDictTy());
    }
    virtual const std::string Repr();

    void Set(std::string k, Value *v) {
        m_m[k] = v;
    }
private:
    std::map<std::string,Value*> m_m;
};

/** Used for global and local variables - points to a value. */
class Cell : public Variable {
public:
    Cell(std::string name) :
        m_name(name) {
        SetType(Type::GetCellTy());
    }
    virtual const std::string Repr();  
    virtual std::string RefRepr();

    static Cell *GetGlobal(std::string name);
    
    std::string m_name;
};

#endif
