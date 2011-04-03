#if defined(WITH_LLVM)

#include <instructions.h>
#include <constants.h>
#include <BasicBlock.h>

#include <PyRuntime.h>
#include <Module.h>
#include <Exception.h>

#include <llvm/Type.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constant.h>
#include <llvm/Value.h>
#include <llvm/Function.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Intrinsics.h>

#include <LLVM_Support.h>

#include <stdio.h>


static llvm::Value *_LLVM_GetVoidPtr(llvm::IRBuilder<> &b, void *ptr) {
    llvm::Value *constant = llvm::ConstantInt::get(g_nint_ty, (uint64_t)ptr);
    assert(constant);
    return b.CreateIntToPtr(constant, g_u8_ptr_ty);
}

static void _LLVM_SetLine(llvm::Value *v, Function *f, int bc_off) {
    llvm::Instruction *i = dynamic_cast<llvm::Instruction*>(v);
    assert(i);
    
    llvm::LLVMContext &ctx = v->getContext();

    int line = f->GetCode()->GetLineNo(bc_off);
    i->setDebugLoc(llvm::DebugLoc::get(line, 0, g_llvm_compilation_unit));
}

static llvm::Value *_LLVM_Call(llvm::IRBuilder<> &b, llvm::Function *func, const char *name, void *fp) {
    llvm::Value *v = b.CreateCall(g_fn_callc0, _LLVM_GetVoidPtr(b, fp), name);
    assert(v);
    return v;
}
static llvm::Value *_LLVM_Call(llvm::IRBuilder<> &b, llvm::Function *func, const char *name, void *fp, llvm::Value *arg0) {
    llvm::Value *v = b.CreateCall2(g_fn_callc0,
                                  _LLVM_GetVoidPtr(b, fp),
                                  arg0,
                                  name);
    assert(v);
    return v;
}
static llvm::Value *_LLVM_Call(llvm::IRBuilder<> &b, llvm::Function *func, const char *name, void *fp, llvm::Value *arg0, llvm::Value *arg1) {
    llvm::Value *v = b.CreateCall3(g_fn_callc0,
                                  _LLVM_GetVoidPtr(b, fp),
                                  arg0,
                                  arg1,
                                  name);
    assert(v);
    return v;
}
static llvm::Value *_LLVM_Call(llvm::IRBuilder<> &b, llvm::Function *func, const char *name, void *fp, llvm::Value *arg0, llvm::Value *arg1, llvm::Value *arg2) {
    llvm::Value *v = b.CreateCall4(g_fn_callc0,
                                  _LLVM_GetVoidPtr(b, fp),
                                  arg0,
                                  arg1,
                                  arg2,
                                  name);
    assert(v);
    return v;
}
// static llvm::Value *_LLVM_Call(llvm::IRBuilder<> &b, llvm::Function *func, const char *name, void *fp, llvm::Value *arg0, llvm::Value *arg1, llvm::Value *arg2, llvm::Value *arg3) {
//     llvm::Value *v = b.CreateCall5(g_fn_callc0,
//                                   _LLVM_GetVoidPtr(b, fp),
//                                   arg0,
//                                   arg1,
//                                   arg2,
//                                   arg3,
//                                   name);
//     assert(v);
//     return v;
// }

static llvm::Value *_LLVM_CallVtable(llvm::IRBuilder<> &b, llvm::Function *func, int idx, llvm::Value *obj) {
    /* Assume that obj is of a subtype of g_object_ty */

    /* Dereference the first member of obj to get the vtable as an array of i8*. */
    llvm::Value *vtable = b.CreateExtractValue(b.CreateLoad(obj), 0, "vtable");
    assert(vtable);
    
    vtable = b.CreateLoad(vtable);

    /* Grab the correct i8* from the vtable. */
    llvm::Value *ptr = b.CreateExtractValue(vtable, idx, "virtual-ptr");
    assert(ptr);

    /* Call that bad boy. */
    llvm::Value *call = b.CreateCall2(g_fn_callc0,
                                      b.CreateBitCast(ptr, g_object_ty),
                                      obj);
    assert(call);
    return call;
}
static llvm::Value *_LLVM_CallVtable(llvm::IRBuilder<> &b, llvm::Function *func, Function *f, int idx, llvm::Value *obj, llvm::Value *arg0) {
    /* Assume that obj is of a subtype of g_object_ty */

    /* Dereference the first member of obj to get the vtable as an array of i8*. */
    llvm::Value *vtable = b.CreateExtractValue(b.CreateLoad(obj), 0, "vtable");
    assert(vtable);
    
    vtable = b.CreateLoad(vtable);

    /* Grab the correct i8* from the vtable. */
    llvm::Value *ptr = b.CreateExtractValue(vtable, idx, "virtual-ptr");
    assert(ptr);

    BasicBlock *blk = f->LJ_GetCurrentBlock();

    std::vector<llvm::Value*> args;
    args.push_back(b.CreateBitCast(ptr, g_object_ty));
    args.push_back(obj);
    args.push_back(arg0);

    /* Call that bad boy. */
    llvm::Value *call;

    if(blk->GetUnwindBlock() == 0) {
        call = b.CreateCall(g_fn_callc2, args.begin(), args.end());
    } else {
        llvm::BasicBlock *normal_blk = llvm::BasicBlock::Create(func->getContext(), "hot-callreturn", func);
        call = b.CreateInvoke(g_fn_callc2, normal_blk, blk->GetUnwindBlock()->LLVM_GetBlock(func), args.begin(), args.end());
        b.SetInsertPoint(normal_blk);
    }
    assert(call);
    return call;
}
static llvm::Value *_LLVM_CallVtable(llvm::IRBuilder<> &b, llvm::Function *func, Function *f, int idx, llvm::Value *obj, llvm::Value *arg0, llvm::Value *arg1) {
    /* Assume that obj is of a subtype of g_object_ty */

    /* Dereference the first member of obj to get the vtable as an array of i8*. */
    llvm::Value *vtable = b.CreateExtractValue(b.CreateLoad(obj), 0, "vtable");
    assert(vtable);

    vtable = b.CreateLoad(vtable);
    
    /* Grab the correct i8* from the vtable. */
    llvm::Value *ptr = b.CreateExtractValue(vtable, idx, "virtual-ptr");
    assert(ptr);

    BasicBlock *blk = f->LJ_GetCurrentBlock();

    std::vector<llvm::Value*> args;
    args.push_back(b.CreateBitCast(ptr, g_object_ty));
    args.push_back(obj);
    args.push_back(arg0);
    args.push_back(arg1);

    /* Call that bad boy. */
    llvm::Value *call;

    if(blk->GetUnwindBlock() == 0) {
        call = b.CreateCall(g_fn_callc2, args.begin(), args.end());
    } else {
        llvm::BasicBlock *normal_blk = llvm::BasicBlock::Create(func->getContext(), "hot-callreturn", func);
        call = b.CreateInvoke(g_fn_callc2, normal_blk, blk->GetUnwindBlock()->LLVM_GetBlock(func), args.begin(), args.end());
        b.SetInsertPoint(normal_blk);
    }
    assert(call);
    return call;
}

llvm::Value *LoadGlobal::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    llvm::Value *cint = llvm::ConstantInt::get(g_nint_ty, (uint64_t)f->GetModule()->GetGlobals());
    assert(cint);
    llvm::Value *g = b.CreateIntToPtr(cint, g_object_ty);
    assert(g);

    llvm::Value *i = _LLVM_CallVtable(b, func, f, Object::idx__Subscr__,
                                            g, m_args[0]->LLVM_Codegen(b, func, f));
    _LLVM_SetLine(i, f, m_bytecode_offset);
    return i;
}

llvm::Value *StoreGlobal::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    llvm::Value *cint = llvm::ConstantInt::get(g_nint_ty, (uint64_t)f->GetModule()->GetGlobals());
    assert(cint);
    llvm::Value *g = b.CreateIntToPtr(cint, g_object_ty);
    assert(g);

    llvm::Value *i = _LLVM_CallVtable(b, func, f, Object::idx__StoreSubscr__,
                                      g, m_args[0]->LLVM_Codegen(b, func, f),
                                      m_args[1]->LLVM_Codegen(b, func, f));
    _LLVM_SetLine(i, f, m_bytecode_offset);
    return i;
}

llvm::Value *Return::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
//    jit_insn_mark_offset(func, m_bytecode_offset);
    llvm::Value *v = b.CreateRet(m_args[0]->LLVM_Codegen(b, func, f));
    _LLVM_SetLine(v, f, m_bytecode_offset);
    return v;
}

llvm::Value *Compare::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    int vtable_idx = -1;
    switch(m_op) {
        case PyCmp_LT:
            vtable_idx = Object::idx__Lt__;
            break;
        case PyCmp_LE:
            vtable_idx = Object::idx__Le__;
            break;
        case PyCmp_EQ:
            vtable_idx = Object::idx__Eq__;
            break;
        case PyCmp_NE:
            vtable_idx = Object::idx__Ne__;
            break;
        case PyCmp_GT:
            vtable_idx = Object::idx__Gt__;
            break;
        case PyCmp_GE:
            vtable_idx = Object::idx__Ge__;
            break;
        case PyCmp_IS:
        case PyCmp_IS_NOT:
        case PyCmp_EXC_MATCH:
        case PyCmp_IN:
        case PyCmp_NOT_IN:
        default:
            std::cerr << "Unimplemented compare operation\n";
    }

//    jit_insn_mark_offset(func, m_bytecode_offset);

    llvm::Value *i = _LLVM_CallVtable(b, func, f,
                                      vtable_idx,
                                      m_args[0]->LLVM_Codegen(b, func, f),
                                      m_args[1]->LLVM_Codegen(b, func, f));
    _LLVM_SetLine(i, f, m_bytecode_offset);
    return i;
}

llvm::Value *TestIfFalse::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
//    jit_insn_mark_offset(func, m_bytecode_offset);

    BasicBlock *blk = f->LJ_GetCurrentBlock();

    llvm::Value *eq_test = b.CreateICmpEQ(m_args[0]->LLVM_Codegen(b, func, f),
                                          Constant::GetBool(false)->LLVM_Codegen(b, func, f), "false_test");
    assert(eq_test);

    llvm::Value *branch_if = b.CreateCondBr(eq_test,
                                            blk->GetSuccessor(0)->LLVM_GetBlock(func),
                                            blk->GetSuccessor(1)->LLVM_GetBlock(func));
    assert(branch_if);
    _LLVM_SetLine(branch_if, f, m_bytecode_offset);

    return branch_if;
}

llvm::Value *PrintItem::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
//    jit_insn_mark_offset(func, m_bytecode_offset);
    return _LLVM_Call(b, func, "FPyRuntime_PrintItem", (void*)&FPyRuntime_PrintItem, m_args[0]->LLVM_Codegen(b, func, f));
}
llvm::Value *PrintNewline::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
//    jit_insn_mark_offset(func, m_bytecode_offset);
    return _LLVM_Call(b, func, "FPyRuntime_PrintNewline", (void*)&FPyRuntime_PrintNewline);
}

llvm::Value *BeginCatch_GetValue::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {

    llvm::Value *eh_ptr = b.CreateCall(g_llvm_eh_exception, "eh_ptr");

    llvm::Value *eh_select = b.CreateCall3(g_llvm_eh_selector,
                                           eh_ptr,
                                           b.CreateBitCast(g_personality, g_u8_ptr_ty),
                                           llvm::Constant::getNullValue(g_u8_ptr_ty));
    _LLVM_SetLine(eh_select, f, m_bytecode_offset);
    return eh_select;
}

/** Note the lack of underscore - these are not cached because they require
    bitcasts, which mustn't cross fn boundaries. */
llvm::Value *Constant::LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    return _LLVM_Codegen(b, func, f);
}
llvm::Value *Constant::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    llvm::Value *obj = _LLVM_GetVoidPtr(b, (void*)this);
    assert(obj);
    return obj;
}

#endif
