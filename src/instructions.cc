#include <instructions.h>
#include <sstream>

const std::string Call::Repr() {
    std::stringstream ss;
    ss << "    " << RefRepr() << " = call " << m_callee->RefRepr() << " (";
    bool first = true;
    for(std::list<Value*>::iterator it = m_positional_args.begin();
        it != m_positional_args.end();
        ++it) {
        if(first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << (*it)->RefRepr();
    }
    for(std::map<std::string,Value*>::iterator it = m_keyword_args.begin();
        it != m_keyword_args.end();
        ++it) {
        if(first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << it->first << "=" << it->second->RefRepr();
    }
    ss << ")";
    return ss.str();
}

const std::string GetAttr::Repr() {
    std::stringstream ss;
    ss << "    " << RefRepr() << " = getattr " << m_obj->RefRepr() << ", " << m_op;
    return ss.str();
}

const std::string Compare::Repr() {

    const char *opname = "bad";
    switch(m_op) {
        case PyCmp_LT: opname="lt"; break;
        case PyCmp_LE: opname="le"; break;
        case PyCmp_EQ: opname="eq"; break;
        case PyCmp_NE: opname="ne"; break;
        case PyCmp_GT: opname="gt"; break;
        case PyCmp_GE: opname="ge"; break;
        case PyCmp_IN: opname="in"; break;
        case PyCmp_NOT_IN: opname="not in"; break;
        case PyCmp_IS: opname="is"; break;
        case PyCmp_IS_NOT: opname="is not"; break;
        case PyCmp_EXC_MATCH: opname="exactly"; break;
    }

    std::stringstream ss;
    ss << "    " << RefRepr() << " = compare " << opname << ", " << m_args[0]->RefRepr() << ", " << m_args[1]->RefRepr();
    return ss.str();
}
