/**@file   Marshal.h
 * @author James Molloy <james.molloy@arm.com>
 * @date   Sat Dec  4 13:11:06 2010
 * @brief  Provides the interface for the Marshal, which reads CPython bytecode. */

#ifndef MARSHAL_H
#define MARSHAL_H

#include <stdint.h>
#include <string>

class Object;
class Errors;

class Marshal {
public:
    Marshal();

    /** Reads an object from the given filename, garbling if there are errors. */
    Object *ReadFile(std::string fname);
    
    /** Reads an object from the given string, garbling if there are errors. */
    Object *ReadString(std::string str);

    /** Lower-level routine - reads an object from an istream. */
    Object *ReadObject(std::istream &is, Errors &e);

private:
    std::string ReadString(std::istream &is, int len);
    int8_t      ReadByte(std::istream &is);
    int16_t     ReadShort(std::istream &is);
    long        ReadLong(std::istream &is);
    int64_t     ReadLong64(std::istream &is);
    double      ReadFloat(std::istream &is);
};

#endif
