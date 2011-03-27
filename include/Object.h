/**@file   Object.h
 * @author James Molloy <james.molloy@arm.com>
 * @date   Sat Dec  4 13:26:16 2010
 * @brief  The base class for all objects pulled in from the python bytecode. */

#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <stdlib.h>
#include <map>
#include <sstream>

class Value;

/** An object is the base class of all instructions, constants and variables - everything representable in the system.

    Each object has a set of "properties", which can be set and queried. It also has an "ID", which can be used as a reference
    in stringified representation.

    An object has a "wart" - a single character used to identify the type of object. Obvious warts are '%' for instructions,
    '@' for constants and '$' for variables. */
class Object {
public:
    /** Properties are expressed as key/value pairs of strings. */
    typedef std::map<std::string, std::string> PropertyMap;

    Object() : m_id(-1) {}

    enum VtableIdx {
        idx__Hash__ = 0,
        idx__Repr__ = 1,
        idx__Str__ = 2,
        idx__Cmp__ = 3,
        idx__Getattr__ = 4,
        idx__Setattr__ = 5,
        idx__Hasattr__ = 6,
        idx__StoreSubscr__ = 7,
        idx__Subscr__ = 8,
        idx__DelSubscr__ = 9
    };  

    /*
     * Virtual functions to be implemented by subclasses.
     */
    virtual Object *__Hash__();
    virtual Object *__Repr__();
    virtual Object *__Str__();
    virtual Object *__Cmp__(Object *other);
    virtual Object *__Getattr__(Object *attr);
    virtual Object *__Setattr__(Object *attr, Object *value);
    virtual Object *__Hasattr__(Object *attr);
    virtual Object *__StoreSubscr__(Object *idx, Object *value);
    virtual Object *__Subscr__(Object *idx);
    virtual Object *__DelSubscr__(Object *idx);


    /** Provide a string representation of the object. */
    virtual const std::string Repr() = 0;

    /** Provide a string representation of the object ID + wart */
    virtual std::string RefRepr() {
        std::stringstream ss;
        ss << GetWart();
        if(GetId() == -1) {
            ss << "?";
        } else {
            ss << GetId();
        }
        return ss.str();
    }   

    /** Print a string representation of the object to the stream, including any ID,
        and any properties. */
    void Print(std::ostream &s) {
        s << GetWart();
        if(GetId() == -1) {
            s << "?";
        } else {
            s << GetId();
        }
        s << " = " << Repr();
        if(m_properties.size() > 0) {
            s << " \t{";
            bool first = true;
            for(PropertyMap::iterator it = m_properties.begin();
                it != m_properties.end();
                ++it) {
                if(first) first = false; else s << ", ";
                s << "\"" << it->first << "\": \"" << it->second << "\"";
            }
            s << "}";
        }
        s << "\n";
    }

    /** Performs a Print to stderr - callable from GDB. */
    void Dump() {
        Print(std::cerr);
    }

    int GetId() {
        return m_id;
    }
    void SetId(int id) {
        m_id = id;
    }

    PropertyMap &Properties() {
        return m_properties;
    }

    const std::string &Property(const std::string &key) {
        return m_properties[key];
    }
    int PropertyAsInt(const std::string &key) {
        /// \bug This nasty!
        return atoi(m_properties[key].c_str());
    }

    void SetProperty(const std::string &key, const std::string &value) {
        m_properties[key] = value;
    }

    Value *AsValue() {
        if(IsValue()) {
            return (Value*)this;
        } else {
            std::cerr << "AsValue: not a value." << std::endl;
            return NULL;
        }
    }

protected:
    /** Return the 'wart' for this object type ('%', '@', '$', ':') */
    virtual char GetWart() = 0;
    virtual bool IsValue() {
        return false;
    }

private:
    int m_id;
    PropertyMap m_properties;
};

#endif
