#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <Value.h>

#include <vector>
#include <iostream>

/** The instruction class is a superset of all Python instructions.

    They are objects because code is data, obviously. */
class Instruction : public Value {
public:
    Instruction(const char *mnemonic, int id);

    virtual const std::string Repr();

protected:
    virtual char GetWart() {
        return '%';
    }

    const char *m_mnemonic;
    std::vector<Value*> m_args;
};

#endif
