#include <Object.h>
#include <Type.h>
#include <Constant.h>
#include <constants.h>
#include <sstream>
#include <typeinfo>

#include <stdio.h>
#include <exceptions.h>

Object *Object::__Hash__() {
    return (Object*)Constant::GetInt((unsigned long)this);
}

Object *Object::__Repr__() {
    std::stringstream ss;
    ss << "<";
    ss << typeid(*this).name() << " instance @ ";
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

Object *Object::__Lt__(Object *idx) {
    ConstantInt *i = dynamic_cast<ConstantInt*>(__Cmp__(idx));
    if(!i) {
        throw new TypeError("comparison did not return an int");
    }
    int n = i->Get();

    fprintf(stderr, "Lt: %p, %p, %d (%s, %s)\n", this, idx, n, Repr().c_str(), idx->Repr().c_str());

    return (Object*) ((n < 0) ? Constant::GetBool(true) : Constant::GetBool(false));
}
Object *Object::__Le__(Object *idx) {
    ConstantInt *i = dynamic_cast<ConstantInt*>(__Cmp__(idx));
    if(!i) {
        throw new TypeError("comparison did not return an int");
    }
    int n = i->Get();   
    return (Object*) ((n <= 0) ? Constant::GetBool(true) : Constant::GetBool(false));
}
Object *Object::__Eq__(Object *idx) {
    ConstantInt *i = dynamic_cast<ConstantInt*>(__Cmp__(idx));
    if(!i) {
        throw new TypeError("comparison did not return an int");
    }
    int n = i->Get();

    return (Object*) ((n == 0) ? Constant::GetBool(true) : Constant::GetBool(false));
}
Object *Object::__Ne__(Object *idx) {
    ConstantInt *i = dynamic_cast<ConstantInt*>(__Cmp__(idx));
    if(!i) {
        throw new TypeError("comparison did not return an int");
    }
    int n = i->Get();   

    return (Object*) ((n != 0) ? Constant::GetBool(true) : Constant::GetBool(false));
}
Object *Object::__Gt__(Object *idx) {
    ConstantInt *i = dynamic_cast<ConstantInt*>(__Cmp__(idx));
    if(!i) {
        throw new TypeError("comparison did not return an int");
    }
    int n = i->Get();   

    return (Object*) ((n > 0) ? Constant::GetBool(true) : Constant::GetBool(false));
}
Object *Object::__Ge__(Object *idx) {
    ConstantInt *i = dynamic_cast<ConstantInt*>(__Cmp__(idx));
    if(!i) {
        throw new TypeError("comparison did not return an int");
    }
    int n = i->Get();   

    return (Object*) ((n >= 0) ? Constant::GetBool(true) : Constant::GetBool(false));
}
