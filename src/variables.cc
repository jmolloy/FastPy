#include <variables.h>
#include <sstream>

std::map<std::string, Cell*> g_globals;

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
    ss << ")";
    return ss.str();
}

const std::string Dict::Repr() {
    std::stringstream ss;
    bool first = true;
    ss << "{";
    for(std::map<std::string,Value*>::iterator it = m_m.begin();
        it != m_m.end();
        it++) {
        if(first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << (*it).first << ": ";
        ss << (*it).second->RefRepr();
    }
    ss << "}";
    return ss.str();
}

const std::string Cell::Repr() {
    return m_name;
}
std::string Cell::RefRepr() {
    std::stringstream ss;
    ss << "$" << m_name;
    return ss.str();
}
Cell *Cell::GetGlobal(std::string name) {
    Cell *g = g_globals[name];
    if(g) {
        return g;
    } else {
        g = new Cell(name);
        g_globals[name] = g;
        return g;
    }
}
