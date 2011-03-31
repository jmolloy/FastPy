/**@file   PyMarshal.cc
 * @author James Molloy <james.molloy@arm.com>
 * @date   Sat Dec  4 13:08:48 2010
 * @brief  Reads python bytecode from a string or file. */

#include <Marshal.h>
#include <Object.h>
#include <Errors.h>
#include <constants.h>
#include <variables.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <stdlib.h>
#include <limits.h>

#define MAGIC (3151 | ((long)'\r'<<16) | ((long)'\n'<<24))

#define TYPE_NULL               '0'
#define TYPE_NONE               'N'
#define TYPE_FALSE              'F'
#define TYPE_TRUE               'T'
#define TYPE_STOPITER           'S'
#define TYPE_ELLIPSIS           '.'
#define TYPE_INT                'i'
#define TYPE_INT64              'I'
#define TYPE_FLOAT              'f'
#define TYPE_BINARY_FLOAT       'g'
#define TYPE_COMPLEX            'x'
#define TYPE_BINARY_COMPLEX     'y'
#define TYPE_LONG               'l'
#define TYPE_STRING             's'
#define TYPE_INTERNED           't'
#define TYPE_STRINGREF          'R'
#define TYPE_TUPLE              '('
#define TYPE_LIST               '['
#define TYPE_DICT               '{'
#define TYPE_CODE               'c'
#define TYPE_UNICODE            'u'
#define TYPE_UNKNOWN            '?'
#define TYPE_SET                '<'
#define TYPE_FROZENSET          '>'


Marshal::Marshal() {
    //g_stop_iteration_exception = new Object();
    //g_ellipsis = new Ellipsis();
}

std::string Marshal::ReadString(std::istream &is, int len) {
    assert(len < 512);
    char c[512];
    if(is.read(c, len).fail()) {
        return std::string();
    } else {
        c[len] = '\0';
        return std::string(c);
    }
}

int8_t Marshal::ReadByte(std::istream &is) {
    char c;
    is.read(&c, 1);
    return c;
}
int16_t Marshal::ReadShort(std::istream &is) {
    short c;
    is.read((char*)&c, 2);
    return c;
}
long Marshal::ReadLong(std::istream &is) {
    int32_t c;
    is.read((char*)&c, 4);
    return static_cast<long>(c);
}
int64_t Marshal::ReadLong64(std::istream &is) {
    int64_t c;
    is.read((char*)&c, 8);
    return c;
}

double Marshal::ReadFloat(std::istream &is) {
    double c;
    if(is.read((char*)&c, 8).fail()) {
        return 0.0f;
    } else {
        return c;
    }
}

Object *Marshal::ReadObject(std::istream &is, Errors &e) {

    int type = ReadByte(is);
    switch(type) {

        // case EOF:
        //     e.Error("EOF read where object expected");
        //     return NULL;

        case TYPE_NULL:
            return NULL;

        case TYPE_NONE:
            return Constant::GetNone();

        case TYPE_STOPITER:
            e.Error("TYPE_STOPITER not supported yet");
            e.Garble();
            return NULL;

        case TYPE_ELLIPSIS:
            e.Error("TYPE_ELLIPSIS not supported yet");
            e.Garble();
            return NULL;

        case TYPE_FALSE:
            return Constant::GetBool(false);

        case TYPE_TRUE:
            return Constant::GetBool(true);

        case TYPE_INT:
            return Constant::GetInt(ReadLong(is));

        case TYPE_INT64:
            return Constant::GetInt64(ReadLong64(is));

        case TYPE_LONG:
            e.Error("PyLong marshalling not implemented");
            return NULL;

        case TYPE_FLOAT: {
            std::string str = ReadString(is, ReadByte(is));
            return Constant::GetFloat(strtod(str.c_str(), NULL));
        }

        case TYPE_BINARY_FLOAT:
            return Constant::GetFloat(ReadFloat(is));

        /** @todo Placeholder for TYPE_COMPLEX and TYPE_BINARY_COMPLEX. */

        case TYPE_STRING:
        case TYPE_INTERNED:
            return Constant::GetString(ReadString(is, ReadLong(is)));

        case TYPE_UNICODE: {
            long n = ReadLong(is);
            if(n < 0 || n > INT_MAX) {
                e.Error("Bad marshal data (unicode size out of range)");
                return NULL;
            }
            std::string s = ReadString(is, n);
            /** @todo UTF-8 decode. */
            return Constant::GetString(s);
        }

        case TYPE_TUPLE: {
            long n = ReadLong(is);
            if(n < 0 || n > INT_MAX) {
                e.Error("Bad marshal data (tuple size out of range)");
                return NULL;
            }

            Tuple *t = new Tuple(n);
            for (long i = 0; i < n; i++) {
                Object *o = ReadObject(is, e);
                if(!o) {
                    return NULL;
                }
                t->Set(i, o->AsValue());
            }
            return t;
        }

        case TYPE_LIST: {
            long n = ReadLong(is);
            if(n < 0 || n > INT_MAX) {
                e.Error("Bad marshal data (list size out of range)");
                return NULL;
            }

            List *l = new List(n);
            for (long i = 0; i < n; i++) {
                Object *o = ReadObject(is, e);
                if(!o) {
                    return NULL;
                }
                l->Set(i, o->AsValue());
            }
            return l;
        }

        case TYPE_DICT: {
            Dict *d = new Dict();
            while(true) {
                Object *key = ReadObject(is, e);
                if(!key) {
                    break;
                }
                Object *val = ReadObject(is, e);
                if(val) {
                    d->Set(ConstantString::From(key), val->AsValue());
                }
                delete key;
            }
            if(e.WasError()) {
                return NULL;
            } else {
                return d;
            }
        }

        case TYPE_FROZENSET: {
            long n = ReadLong(is);
            if(n < 0 || n > INT_MAX) {
                e.Error("Bad marshal data (set size out of range)");
                return NULL;
            }
            FrozenSet *s = new FrozenSet();
            
            for(long i = 0; i < n; i++) {
                Object *o = ReadObject(is, e);
                if(!o) {
                    return NULL;
                }
                s->Insert(o->AsValue());
            }
            return s;
        }

        case TYPE_SET: {
            long n = ReadLong(is);
            if(n < 0 || n > INT_MAX) {
                e.Error("Bad marshal data (set size out of range)");
                return NULL;
            }
            Set *s = new Set();

            for(long i = 0; i < n; i++) {
                Object *o = ReadObject(is, e);
                if(!o) {
                    return NULL;
                }
                s->Insert(o->AsValue());
            }
            return s;   
        }

        case TYPE_CODE: {
#define X(y) Object *y = ReadObject(is, e); if(!y) {return NULL;}

            long argcount = ReadLong(is);
            long kwonlyargcount = 0; // ReadLong(is);
            long nlocals = ReadLong(is);
            long stacksize = ReadLong(is);
            long flags = ReadLong(is);

            /* Code must be handled differently as it is a string that contains zeroes. */
            ReadByte(is);
            long l = ReadLong(is);
            ConstantByteString *code = new ConstantByteString(l);
            is.read((char*)code->m_ptr, l);
            X(consts);
            X(names);
            X(varnames);
            X(freevars);
            X(cellvars);
            X(filename);
            X(name);
            long firstlineno = ReadLong(is);

            /* lnotab must be handled differently as it is a string that contains zeroes. */
            ReadByte(is);
            l = ReadLong(is);
            ConstantByteString *lnotab = new ConstantByteString(l);
            is.read((char*)lnotab->m_ptr, l);

            return new Code(argcount, kwonlyargcount, nlocals, stacksize,
                            flags, code, Tuple::From(consts), Tuple::From(names), Tuple::From(varnames), Tuple::From(freevars),
                            Tuple::From(cellvars), ConstantString::From(filename), ConstantString::From(name), firstlineno, lnotab);
        }

        default:
            e.Error("Bad marshal data (unknown type code)");
            return NULL;
    }

}

Object *Marshal::ReadString(std::string str) {
    std::istringstream ss(str, std::ios::binary);
    
    Errors e;
    Object *o = ReadObject(ss, e);
    if(!o || e.WasError()) {
        e.Garble();
    }
    return o;
}

Object *Marshal::ReadFile(std::string fname) {
    std::ifstream s(fname.c_str(), std::ios::binary);

    Errors e;

    long magic = ReadLong(s);
    /*if(magic != MAGIC) {
        e.Error("Wrong magic number in .pyc file.");
        e.Garble();
        }*/
    (void)ReadLong(s);

    Object *o = ReadObject(s, e);
    if(!o || e.WasError()) {
        e.Garble();
    }
    return o;
}
