/**@file   BasicBlock.h
 * @author James Molloy <james.molloy@arm.com>
 * @date   Sun Dec  5 13:16:52 2010
 * @brief  The nodes in the control flow graph. */

#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <Variable.h>
#include <Constant.h>
#include <Value.h>
#include <list>
#include <map>
#include <vector>

#include <jit/jit.h>

class OperandStack;
class Function;
class Instruction;
class Cell;

/** A block is also a value (used as a jump target) */
class BasicBlock : public Value {
public:
    /** Default constructor. */
    BasicBlock(Function *parent);
    ~BasicBlock();
    
    /** Loads the argument as a constant, pushing onto the stack. */
    void LoadConstant(Constant *arg, int &id, int bytecode_offset);
    /** Loads the named global variable, pushing onto the stack. */
    void LoadGlobal(std::string arg, int &id, int bytecode_offset);
    /** Loads the named local (or parameter) variable, pushing onto the stack. */
    void LoadLocal(std::string arg, int &id, int bytecode_offset);

    /** Stores TOS to the named global. */
    void StoreGlobal(std::string arg, int &id, int bytecode_offset);
    /** Stores TOS to the named local. */
    void StoreLocal(std::string arg, int &id, int bytecode_offset);

    /** Creates a new function or closure, with the given number of default parameters, fished from the stack BEFORE
        the code object is expected. */
    void BindClosure(long arg, int &id, int bytecode_offset);

    /** Performs a "standard" function call, with the given number of positional arguments and keyword arguments. */
    void Call(unsigned char num_positional,
              unsigned char num_keywords, int &id, int bytecode_offset);

    /** Pops TOS */
    void Pop(int &id);

    /** Returns TOS. This is a terminator instruction. */
    void Return(int &id, int bytecode_offset);

    /** Conditional jump. This is a terminator instruction.
        @param alwaysPop If true, Pop the TOS always; else only pop the item if the branch was NOT taken.
        @param condition True or false comparison of TOS.
        @param trueBranch Block to branch to if the condition succeeds.
        @param falseBranch Block to branch to if the condition fails. */
    void ConditionalJump(bool alwaysPop, bool condition, BasicBlock *trueBranch, BasicBlock *falseBranch, int &id, int bytecode_offset);

    /** Jump to block. This is a terminator instruction. */
    void Jump(BasicBlock *branch, int &id, int bytecode_offset);

    /** Perform a binary operation on TOS and TOS1 */
    void BinaryOp(std::string op, int &id, int bytecode_offset);

    /** Get an attribute of TOS. */
    void GetAttr(std::string attr, int &id, int bytecode_offset);

    void Dup();
    
    void BuildTuple(int n, int &id, int bytecode_offset);

    void BeginCatch(int &id);

    void Compare(int op, int &id, int bytecode_offset);

    void PrintItem(int &id, int bytecode_offset);
    void PrintNewline(int &id, int bytecode_offset);

    void ReRaise(int &id, int bytecode_offset);

    void AddPredecessor(BasicBlock *b, int &id);

    virtual const std::string Repr();

    void SetUnwindBlock(BasicBlock *uw) {
        m_unwind_block = uw;
    }
    BasicBlock *GetUnwindBlock() {
        return m_unwind_block;
    }

    BasicBlock *GetSuccessor(int n) {
        return m_successors[n];
    }

    void LJ_Codegen();
    jit_label_t *LJ_GetLabel();
    jit_label_t *LJ_GetEndLabel();

protected:
    char GetWart() {
        return ':';
    }
    
private:
    /** Successor blocks */
    BasicBlock *m_successors[2];

    std::list<BasicBlock*> m_predecessors;

    /** How many successors the block has. */
    int m_num_successors;

    /** The stack - only used during block creation. */
    OperandStack *m_stack;

    std::map<std::string,Value*> m_globals, m_locals;

    /** Function parent. */
    Function *m_fn;

    /** All instructions in the block. */
    std::vector<Instruction*> m_all_instructions;

    std::vector<Instruction*> m_dfg_roots;
    std::vector<Value*> m_dfg_leafs;
    std::map<std::string,Value*> m_store_global_roots;
    std::map<std::string,Value*> m_store_local_roots;

    BasicBlock *m_unwind_block;

    jit_label_t m_jit_label, m_jit_end_label;
};

#endif
