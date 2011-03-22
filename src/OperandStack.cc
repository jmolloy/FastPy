/**@file   OperandStack.cc
 * @author James Molloy <james.molloy@arm.com>
 * @date   Sun Dec  5 13:48:53 2010
 * @brief  A stack storing variable references during basic block construction. */

#include <OperandStack.h>
#include <cassert>
#include <iostream>
#include <Value.h>

OperandStack::OperandStack() {
}
OperandStack::~OperandStack() {
    for(std::list<Value*>::iterator it = m_stack.begin();
        it != m_stack.end();
        it++) {
        /**@todo Implement refcounting for nodes. */
        //(*it)->unref();
    }
}

void OperandStack::Push(Value *x) {
    m_stack.push_front(x);
}

Value *OperandStack::Pop() {
    if(!m_stack.empty()) { 
        Value *x = m_stack.front();
        m_stack.pop_front();
        return x;
    } else {
        return 0;
    }
}

Value *OperandStack::Peek() {
    if(m_stack.empty()) {
        return NULL;
    } else {
        Value *x = m_stack.front();
        return x;
    }
}


// So apparently Python doesn't actually give a shit about keeping an
// empty stack at the end of blocks. Eek.
void OperandStack::EnsureEmpty() {
    //assert(m_stack.empty());
}
