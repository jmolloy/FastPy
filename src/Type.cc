#include <Type.h>
#include <map>
#include <sstream>
#include <typeinfo>

std::map<std::string, Type*> g_types;

Type *Type::For(Object *obj) {
    std::string name = typeid(*obj).name();
    /**@todo Locking */
    if(g_types.find(name) == g_types.end()) {
        g_types[name] = new Type(name, obj);
    }
    return g_types[name];
}

Type::Type(std::string &name, Object *ex) :
    m_name(name), m_example(ex) {
    /**@todo ex should be deep-copied here in case the example object changes. */
}

const std::string Type::Repr() {
    std::stringstream ss;
    ss << "<type '" << m_name << "'>";
    return ss.str();
}
