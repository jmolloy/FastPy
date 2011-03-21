/**@file   BasicBlock.cc
 * @author James Molloy <james.molloy@arm.com>
 * @date   Sun Dec  5 13:52:29 2010
 * @brief  The nodes in the control flow graph */

#include <Object.h>
#include <BasicBlock.h>
#include <Function.h>
#include <OperandStack.h>
#include <variables.h>
#include <instructions.h>
#include <constants.h>

#include <cassert>
#include <sstream>

BasicBlock::BasicBlock(Function *parent) :
    m_num_successors(0), m_stack(new OperandStack()), m_fn(parent),
    m_jit_label(jit_label_undefined),
    m_unwind_block(NULL)
{
    m_successors[0] = NULL;
    m_successors[1] = NULL;
}

BasicBlock::~BasicBlock() {
    // for(int i = 0; i < m_num_successors; i++) {
    //     m_successors[0]->unref();
    // }

    delete m_stack;

    // for(std::list<Object*>::iterator it = m_dfg_roots.begin();
    //     it != m_dfg_roots.end();
    //     it++) {
    //     (*it)->unref();
    // }
}

void BasicBlock::LoadConstant(Constant *arg, int &id) {
    m_stack->Push(arg);
    m_dfg_leafs.push_back(arg);
}

void BasicBlock::LoadGlobal(std::string arg, int &id) {
    if(m_globals[arg] == 0) {
        m_globals[arg] = new ::LoadGlobal(new ConstantString(arg), ++id);
        m_all_instructions.push_back((Instruction*)m_globals[arg]);
        m_dfg_leafs.push_back(m_globals[arg]);
    }
    m_stack->Push(m_globals[arg]);
}

void BasicBlock::LoadLocal(std::string arg, int &id) {
    if(m_locals[arg] == 0) {
        m_locals[arg] = new ::LoadLocal(new ConstantString(arg), ++id);
        m_all_instructions.push_back((Instruction*)m_locals[arg]);
        m_dfg_leafs.push_back(m_locals[arg]);
    }
    m_stack->Push(m_locals[arg]);
}

void BasicBlock::StoreGlobal(std::string arg, int &id) {
    m_globals[arg] = new ::StoreGlobal(new ConstantString(arg), m_stack->Pop(), ++id);
    m_all_instructions.push_back((Instruction*)m_globals[arg]);
    m_store_global_roots[arg] = m_globals[arg];
}

void BasicBlock::StoreLocal(std::string arg, int &id) {
    m_locals[arg] = new ::StoreLocal(new ConstantString(arg), m_stack->Pop(), ++id);
    m_all_instructions.push_back((Instruction*)m_locals[arg]);
    m_store_local_roots[arg] = m_locals[arg];
}

// void BasicBlock::BindClosure(long arg, int &id) {
//     std::list<Object*> defaults;
//     std::map<std::string, Object*> bindings;

//     Code *code = dynamic_cast<Code*>(m_stack->Pop());
//     assert(code);

//     for(long i = 0; i < arg; i++) {
//         defaults.push_back(m_stack->Pop());
//     }

//     Function *fn = new Function(code->m_name);

//     for(std::vector<Object*>::iterator it = code->m_freevars.begin();
//         it != code->m_freevars.end();
//         it++) {
//         String *s = dynamic_cast<String*> (*it);
//         assert(s);
//         fn->BindLocal(s->m_str, m_stack->Pop());
//     }

//     code->Walk(fn);

//     m_stack->Push(fn);
// }

void BasicBlock::Call(unsigned char num_positional,
                      unsigned char num_keywords, int &id) {
    std::list<Value*> positional_args;
    std::map<std::string, Value*> keyword_args;

    // for(unsigned char i = 0; i < num_keywords; i++) {
    //     Object *val = m_stack->Pop();
    //     String *key = dynamic_cast<String*>(m_stack->Pop());
    //     assert(key);
    //     keyword_args[key->m_str] = val;
    // }
    for(unsigned char i = 0; i < num_positional; i++) {
        Value *val = m_stack->Pop();
        /* Rightmost argument first, so push front. */
        positional_args.push_front(val);
    }
    
    Value *callee = m_stack->Pop();
    ::Call *call = new ::Call(callee, positional_args, keyword_args, ++id);
    m_all_instructions.push_back(call);
    m_stack->Push(call);

    /* Call might have sideeffects, so must add to DFG roots.
       
       With sideeffect analysis hopefully the call can be removed later. */
    m_dfg_roots.push_back(call);
}

void BasicBlock::Pop(int &id) {
    m_stack->Pop();
}

void BasicBlock::Return(int &id) {
    /* Return always goes at the front. */
    ::Return *r = new ::Return(m_stack->Pop(), ++id);
    m_all_instructions.push_back(r);
    //m_fn->m_returns.push_back(r);
    m_dfg_roots.push_back(r);
    m_stack->EnsureEmpty();
}

void BasicBlock::ConditionalJump(bool alwaysPop, bool condition, BasicBlock *trueBranch, BasicBlock *falseBranch, int &id) {
    Value *tos = m_stack->Pop();

    Instruction *cond;
    if(condition) {
        cond = new TestIfTrue(tos, ++id);
    } else {
        cond = new TestIfFalse(tos, ++id);
    }
    m_all_instructions.push_back(cond);

    m_num_successors = 2;
    m_successors[0] = trueBranch;
    m_successors[1] = falseBranch;
    
    m_dfg_roots.push_back(cond);

    if(!alwaysPop) {
        trueBranch->LoadConstant(Constant::From(tos), id);
    }
}

void BasicBlock::Jump(BasicBlock *branch, int &id) {
    m_num_successors = 1;
    m_successors[0] = branch;

    m_stack->EnsureEmpty();
}

void BasicBlock::BinaryOp(std::string op, int &id) {
    Value *obj = m_stack->Pop();
    Value *obj2 = m_stack->Pop();
    
    std::list<Value*> positional_args;
    std::map<std::string, Value*> keyword_args;

    positional_args.push_back(obj2);

    ::GetAttr *ga = new ::GetAttr(obj, op, ++id);
    m_all_instructions.push_back(ga);
    ::Call *c = new ::Call(ga, positional_args, keyword_args, ++id);
    m_all_instructions.push_back(c);

    m_stack->Push(c);
}

void BasicBlock::GetAttr(std::string attr, int &id) {
    ::GetAttr *ga = new ::GetAttr(m_stack->Pop(), attr, ++id);
    m_all_instructions.push_back(ga);
    m_stack->Push(ga);
}

void BasicBlock::Dup() {
    m_stack->Push(m_stack->Peek());
}

void BasicBlock::BuildTuple(int n, int &id) {
    ::BuildTuple *bt = new ::BuildTuple(n, ++id);
    m_all_instructions.push_back(bt);
    for(int i = 0; i < n; i++) {
        bt->Push(m_stack->Pop());
    }
    m_stack->Push(bt);
}

void BasicBlock::BeginCatch(int &id) {
    Instruction *c = new ::BeginCatch_GetType(++id);
    m_stack->Push(c);
    m_all_instructions.push_back(c);

    c = new ::BeginCatch_GetValue(++id);
    m_stack->Push(c);
    m_all_instructions.push_back(c);

    c = new ::BeginCatch_GetTraceback(++id);
    m_stack->Push(c);
    m_all_instructions.push_back(c);
}

void BasicBlock::Compare(int op, int &id) {
    ::Compare *c = new ::Compare(op, m_stack->Pop(), m_stack->Pop(), ++id);
    m_stack->Push(c);
    m_all_instructions.push_back(c);
}

void BasicBlock::PrintItem(int &id) {
    ::PrintItem *c = new ::PrintItem(m_stack->Pop(), ++id);
    m_all_instructions.push_back(c);
}

void BasicBlock::PrintNewline(int &id) {
    ::PrintNewline *c = new ::PrintNewline(++id);
    m_all_instructions.push_back(c);
}

void BasicBlock::ReRaise(int &id) {
    ::ReRaise *i = new ::ReRaise(++id);
    m_all_instructions.push_back(i);
}

const std::string BasicBlock::Repr() {
    std::stringstream ss;
    if(GetId() != -1) {
        ss << GetId() << ":\n";
    } else {
        ss << "?:\n";
    }

    for(std::vector<Instruction*>::iterator it = m_all_instructions.begin();
        it != m_all_instructions.end();
        ++it) {
        ss << (*it)->Repr() << "\n";
    }

    ss << "    -> [";
    for(int i = 0; i < m_num_successors; i++) {
        if (i > 0) {
            ss << ", ";
        }
        ss << m_successors[i]->RefRepr();
    }
    ss << "]";
    if(m_unwind_block) {
        ss << " unwind " << m_unwind_block->RefRepr();
    }
    ss << "\n";

    return ss.str();
}

void BasicBlock::LJ_Codegen() {
    jit_function_t func = m_fn->LJ_Codegen();
 
    jit_insn_label(func, &m_jit_label);
    
    for(std::vector<Instruction*>::iterator it = m_all_instructions.begin();
        it != m_all_instructions.end();
        ++it) {
        (*it)->LJ_Codegen(func, m_fn);
    }
}

jit_label_t *BasicBlock::LJ_GetLabel() {
    return &m_jit_label;
}
