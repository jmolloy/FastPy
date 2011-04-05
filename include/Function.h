#ifndef FUNCTION_H
#define FUNCTION_H

#include <Constant.h>
#include <map>
#include <vector>
#include <string>

#include <jit/jit.h>
#if defined(WITH_LLVM)
#include <llvm/Function.h>
#endif

class Cell;
class BasicBlock;
class Code;
class Module;

class Function : public Constant {
public:
    /** Creates a new function with the given identifying name. */
    Function(std::string name, Code *code, Module *module);
    virtual ~Function();

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

#if defined(WITH_LLVM)
    llvm::Function *LLVM_Codegen(llvm::Module *m);
#endif

    BasicBlock *LJ_GetCurrentBlock() {
        return m_current_block;
    }

    virtual void *GetFnPtr();

    jit_value_t LJ_GetExceptionObject() {
        return m_lj_exception_object;
    }
    void LJ_SetExceptionObject(jit_value_t exc) {
        m_lj_exception_object = exc;
    }
#if defined(WITH_LLVM)
    llvm::Value *LLVM_GetExceptionObject() {
        return m_llvm_exception_object;
    }
    void LLVM_SetExceptionObject(llvm::Value *exc) {
        m_llvm_exception_object = exc;
    }
#endif

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

    jit_value_t m_lj_exception_object;

    /** Almost an iterator - the current block being codegenned.

        This is only valid during LJ_Codegen time. */
    BasicBlock *m_current_block;

#if defined(WITH_LLVM)
    llvm::Function *m_llvm_function;
    llvm::Value *m_llvm_exception_object;
#endif
};

class BuiltinFunction : public Function {
public:
    BuiltinFunction(void *fn) :
        Function("<builtin>", NULL, NULL),
        m_fn(fn) {
    }
    
    virtual void *GetFnPtr() {
        return m_fn;
    }

    virtual const std::string Repr();

private:
    void *m_fn;
};

#endif
