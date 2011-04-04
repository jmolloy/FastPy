#include <variables.h>
#include <sstream>
#include <Constant.h>
#include <exceptions.h>

//std::map<std::string, Cell*> g_globals;

const std::string Set::Repr() {
    std::stringstream ss;
    bool first = true;
    ss << "set(";
    for(std::set<Value*>::iterator it = m_s.begin();
        it != m_s.end();
        it++) {
        if(first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << (*it)->RefRepr();
    }
    ss << ")";
    return ss.str();
}

const std::string List::Repr() {
    std::stringstream ss;
    bool first = true;
    ss << "[";
    for(std::vector<Value*>::iterator it = m_l.begin();
        it != m_l.end();
        it++) {
        if(first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << (*it)->RefRepr();
    }
    ss << "]";
    return ss.str();
}

const std::string Tuple::Repr() {
    std::stringstream ss;
    bool first = true;
    ss << "(";
    for(std::vector<Object*>::iterator it = m_l.begin();
        it != m_l.end();
        it++) {
        if(*it == 0) {
            continue;
        }
        if(first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << (*it)->RefRepr();
    }
    ss << ")";
    return ss.str();
}

const std::string Dict::Repr() {
    std::stringstream ss;
    bool first = true;
    ss << "{";
    for(std::map<Object*,Object*>::iterator it = m_m.begin();
        it != m_m.end();
        it++) {
        if(first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << (*it).first->Repr() << ": ";
        ss << (*it).second->RefRepr();
    }
    ss << "}";
    return ss.str();
}
Object *Dict::__Subscr__(Object *idx) {
    /* Oh my gosh, this is just utterly horrendous.

       Like, actually, O(n) horrendous. */
    for(std::map<Object*,Object*>::iterator it = m_m.begin();
        it != m_m.end();
        ++it) {
        Object *k = it->first;
        if(k->__Eq__(idx) == (Object*)Constant::GetBool(true)) {
            return it->second;
        }
    }
    std::stringstream ss;
    ss << "Key does not exist: " << idx->Repr();
    throw new KeyError(ss.str());
}
Object *Dict::__StoreSubscr__(Object *idx, Object *value) {
    m_m[idx] = value;
    return value;
}
Object *Dict::__DelSubscr__(Object *idx) {
    /* Oh my gosh, this is just utterly horrendous.

       Like, actually, O(n) horrendous. */
    for(std::map<Object*,Object*>::iterator it = m_m.begin();
        it != m_m.end();
        ++it) {
        Object *k = it->first;
        if(k->__Eq__(idx) == (Object*)Constant::GetBool(true)) {
            m_m.erase(it);
            return (Object*)Constant::GetNone();
        }
    }
}

const std::string Cell::Repr() {
    return m_name;
}
std::string Cell::RefRepr() {
    std::stringstream ss;
    ss << "$" << m_name;
    return ss.str();
}
// Cell *Cell::GetGlobal(std::string name) {
//     Cell *g = g_globals[name];
//     if(g) {
//         return g;
//     } else {
//         g = new Cell(name);
//         g_globals[name] = g;
//         return g;
//     }
// }
