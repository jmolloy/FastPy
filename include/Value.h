#ifndef VALUE_H
#define VALUE_H

#include <Object.h>

#include <jit/jit.h>

class Function;

/** A value is any piece of data that can be used in an instruction. */
class Value : public Object {
public:
    Value() :
        m_jit_value(0), m_bytecode_offset(0) {
    }

    virtual bool IsValue() {
        return true;
    }

    /** This function performs libjit codegen.
        
        Ideally this would be in a low-cohesion unit that visits each instruction
        however this would require lots of type-checks at runtime and I felt
        that the high cohesion is worth it for the overhead reduction. */
    jit_value_t LJ_Codegen(jit_function_t func, Function *f) {
        if(!m_jit_value) {
            m_jit_value = _LJ_Codegen(func, f);
        }
        return m_jit_value;
    }

    void SetBytecodeOffset(int bo) {
        m_bytecode_offset = bo;
    }

private:
    virtual jit_value_t _LJ_Codegen(jit_function_t func, Function *f) {
        std::cerr << "LJ_Codegen: Unimplemented: " << Repr() << std::endl;
        return NULL;
    }
    jit_value_t m_jit_value;

protected:
    int m_bytecode_offset;
};

#endif
