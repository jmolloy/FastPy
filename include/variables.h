#ifndef VARIABLES_H
#define VARIABLES_H

#include <Variable.h>
#include <Type.h>
#include <set>
#include <map>
#include <vector>
#include <cassert>
#include <iostream>
#include <Constant.h>

class Set : public Variable {
public:
    Set() {
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
    }
    virtual const std::string Repr();

    static Tuple *From(Object *o) {
        Tuple *t = dynamic_cast<Tuple*>(o);
        assert(t);
        return t;
    }

    void Set(long i, Object *v) {
        m_l[i] = v;
    }
    Object *Get(int idx) {
        return m_l[idx];
    }
    int Length() {
        return m_l.size();
    }
private:
    std::vector<Object*> m_l;
};

class Dict : public Variable {
public:
    Dict() {
    }
    virtual const std::string Repr();

    void Set(Object *k, Object *v) {
        m_m[k] = v;
    }

    virtual Object *__Subscr__(Object *idx);
    virtual Object *__StoreSubscr__(Object *idx, Object *value);
    virtual Object *__DelSubscr__(Object *idx);
        
private:
    std::map<Object*,Object*> m_m;
};

/** Used for global and local variables - points to a value. */
class Cell : public Variable {
public:
    Cell(std::string name) :
        m_name(name) {
    }
    virtual const std::string Repr();
    virtual std::string RefRepr();

    static Cell *GetGlobal(std::string name);
    
    std::string m_name;
};

#endif
