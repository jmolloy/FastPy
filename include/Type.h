#ifndef TYPE_H
#define TYPE_H

#include <inttypes.h>
#include <Constant.h>

/** A type is a descriptor, and should not be inherited. It should be composed
    into classes that subclass Constant or Variable. */
class Type : public Constant {
public:
    static Type *For(Object *obj);

    Type(std::string &name, Object *ex);
    
    virtual const std::string Repr();

private:
    /** Stringified representation of this type, for comparison and printing. */
    std::string m_name;
    /** An instance of this class for introspection. */
    Object *m_example;
};

#endif
