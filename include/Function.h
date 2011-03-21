#ifndef FUNCTION_H
#define FUNCTION_H

#include <Constant.h>
#include <map>
#include <vector>
#include <string>

#include <jit/jit.h>

class Cell;
class BasicBlock;
class Code;
class Module;

class Function : public Constant {
public:
    /** Creates a new function with the given identifying name. */
    Function(std::string name, Code *code, Module *module);
    ~Function();

    /** Returns the entry block of this function. */
    BasicBlock *GetEntryBlock() {
        return m_entry_block;
    }

    Code *GetCode() {
        return m_code;
    }

    Module *GetModule() {
        return m_module;
    }

    void AddBlock(BasicBlock *b) {
        m_blocks.push_back(b);
    }

    const std::string &GetName() {
        return m_name;
    }

    virtual const std::string Repr();

    jit_function_t LJ_Codegen(jit_context_t ctx=0);

private:
    /** Entry block */
    BasicBlock *m_entry_block;

    /** All basic blocks. */
    std::vector<BasicBlock*> m_blocks;

    /** Function name */
    std::string m_name;

    /** Python code object */
    Code *m_code;

    Module *m_module;

    /** Named arguments */
    std::vector<std::string> m_arguments;

    jit_function_t m_jit_function;
};

#endif
