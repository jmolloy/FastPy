#ifndef VARIABLE_H
#define VARIABLE_H

#include <Value.h>
#include <inttypes.h>

/** A variable is data that is mutable. */
class Variable : public Value {
public:
    static Variable *GetGlobal(std::string var);
protected:
    virtual char GetWart() {
        return '$';
    }
};

#endif
