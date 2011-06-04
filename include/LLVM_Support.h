#if defined(WITH_LLVM)
#ifndef LLVM_SUPPORT_H
#define LLVM_SUPPORT_H

#include <llvm/PassManager.h>
#include <llvm/Support/DebugLoc.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Type.h>
#include <llvm/Value.h>

#include <map>
#include <utility>
#include <iostream>

class Function;
class Object;

llvm::Module *LLVM_Initialize();
Object *LLVM_Invoke(void *p);
bool LLVM_AddressContainedInJITFunction(void *address);
Function *LLVM_GetFunction(void *address);
int LLVM_GetLineNo(void *address);

extern std::map<const llvm::Function*, Function*> g_llvm_function_map;

extern llvm::Type *g_object_ty;
extern const llvm::Type *g_u8_ptr_ty, *g_nint_ty;
extern llvm::Function *g_fn_callc0, *g_fn_callc1, *g_fn_callc2, *g_fn_callc3, *g_fn_callc4, *g_fn_callc5;
extern llvm::Value *g_llvm_eh_exception, *g_llvm_eh_selector, *g_personality;
extern llvm::Value *g_cxa_begin_catch, *g_cxa_end_catch, *g_unwind_resume_or_rethrow;
extern llvm::MDNode *g_llvm_compilation_unit;
extern llvm::FunctionPassManager *g_llvm_fpm;

#endif
#endif
