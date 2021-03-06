#include <constants.h>
#include <sstream>
#include <string.h>

const std::string ConstantInt::Repr() {
    std::stringstream ss;
    ss << m_n;
    return ss.str();
}
// virtual Object *ConstantInt::__Compare__(Object *other) {
//     if(dynamic_cast<ConstantInt*>(other
// }

const std::string ConstantInt64::Repr() {
    std::stringstream ss;
    ss << m_n;
    return ss.str();
}
const std::string ConstantFloat::Repr() {
    std::stringstream ss;
    ss << m_n << "f";
    return ss.str();
}
const std::string ConstantString::Repr() {
    std::stringstream ss;
    ss << "\"" << m_str << "\"";
    return ss.str();
}
Object *ConstantString::__Cmp__(Object *other) {
    if(other == this) {
        return (Object*)Constant::GetInt(0);
    }

    ConstantString *s = dynamic_cast<ConstantString*>(other);
    if(!s) {
        return (Object*)Constant::GetInt(1);
    }

    return (Object*)Constant::GetInt( strcmp(m_str.c_str(), s->m_str.c_str()) );
}

const std::string ConstantByteString::Repr() {
    std::stringstream ss;
    ss << "b\"";
    for(int i = 0; i < m_sz; i++) {
        ss << "\\";
        ss << (int)m_ptr[i];
    }
    ss << "\"";
    return ss.str();
}
const std::string ConstantBool::Repr() {
    return (m_b) ? "True" : "False";
}
const std::string ConstantNone::Repr() {
    return "None";
}
const std::string FrozenSet::Repr() {
    std::stringstream ss;
    bool first = true;
    ss << "frozenset(";
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
