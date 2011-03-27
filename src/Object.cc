#include <Object.h>
#include <Type.h>
#include <Constant.h>
#include <sstream>
#include <typeinfo>

#include <exceptions.h>

Object *Object::__Hash__() {
    return (Object*)Constant::GetInt((unsigned long)this);
}

Object *Object::__Repr__() {
    std::stringstream ss;
    ss << "<";
    ss << typeid(this).name() << "instance @ ";
    ss << std::hex << this << std::dec << ">";

    return (Object*)Constant::GetString(ss.str());
}

Object *Object::__Str__() {
    return __Repr__();
}

Object *Object::__Cmp__(Object *other) {
    if (this == other) {
        return (Object*)Constant::GetInt(0);
    } else if (this > other) {
        return (Object*)Constant::GetInt(-1);
    } else {
        return (Object*)Constant::GetInt(1);
    }
}

Object *Object::__Getattr__(Object *attr) {
    throw new NotImplementedError("Not implemented for this datatype");
}

Object *Object::__Setattr__(Object *attr, Object *value) {
    throw new NotImplementedError("Not implemented for this datatype");
}

Object *Object::__Hasattr__(Object *attr) {
    throw new NotImplementedError("Not implemented for this datatype");
}

Object *Object::__StoreSubscr__(Object *attr, Object *value) {
    throw new NotImplementedError("Not implemented for this datatype");
}

Object *Object::__Subscr__(Object *attr) {
    throw new NotImplementedError("Not implemented for this datatype");
}

Object *Object::__DelSubscr__(Object *attr) {
    throw new NotImplementedError("Not implemented for this datatype");
}
