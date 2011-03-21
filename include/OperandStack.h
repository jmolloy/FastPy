/**@file   OperandStack.h
 * @author James Molloy <james.molloy@arm.com>
 * @date   Sun Dec  5 13:36:06 2010
 * @brief  A stack storing variable references during basic block construction. */

#ifndef OPERAND_STACK_H
#define OPERAND_STACK_H

#include <list>

class Value;

class OperandStack {
public:
    OperandStack();
    ~OperandStack();

    void Push(Value *x);
    Value *Pop();
    Value *Peek();

    void EnsureEmpty();

private:
    std::list<Value*> m_stack;
};

#endif
