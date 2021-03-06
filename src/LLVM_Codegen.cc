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
static llvm::Value *_LLVM_GetObjPtr(llvm::IRBuilder<> &b, void *ptr) {
    llvm::Value *constant = llvm::ConstantInt::get(g_nint_ty, (uint64_t)ptr);
    assert(constant);
    return b.CreateIntToPtr(constant, g_object_ty);
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
    llvm::Value *v = b.CreateCall2(g_fn_callc1,
                                  _LLVM_GetVoidPtr(b, fp),
                                  arg0,
                                  name);
    assert(v);
    return v;
}
static llvm::Value *_LLVM_Call(llvm::IRBuilder<> &b, llvm::Function *func, const char *name, void *fp, llvm::Value *arg0, llvm::Value *arg1) {
    llvm::Value *v = b.CreateCall3(g_fn_callc2,
                                  _LLVM_GetVoidPtr(b, fp),
                                  arg0,
                                  arg1,
                                  name);
    assert(v);
    return v;
}
static llvm::Value *_LLVM_Call(llvm::IRBuilder<> &b, llvm::Function *func, const char *name, void *fp, llvm::Value *arg0, llvm::Value *arg1, llvm::Value *arg2) {
    llvm::Value *v = b.CreateCall4(g_fn_callc3,
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
                                      ptr,
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
    args.push_back(ptr);
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
    args.push_back(ptr);
    args.push_back(obj);
    args.push_back(arg0);
    args.push_back(arg1);

    /* Call that bad boy. */
    llvm::Value *call;

    if(blk->GetUnwindBlock() == 0) {
        call = b.CreateCall(g_fn_callc3, args.begin(), args.end());
    } else {
        llvm::BasicBlock *normal_blk = llvm::BasicBlock::Create(func->getContext(), "hot-callreturn", func);
        call = b.CreateInvoke(g_fn_callc3, normal_blk, blk->GetUnwindBlock()->LLVM_GetBlock(func), args.begin(), args.end());
        b.SetInsertPoint(normal_blk);
    }
    assert(call);
    return call;
}

static llvm::Value *_LLVM_AllocMemory(Instruction *i, llvm::IRBuilder<> &b, llvm::Function *f, size_t sz) {
    return _LLVM_Call(b, f, "malloc", (void*)&malloc,
                      _LLVM_GetObjPtr(b, (void*)sz));
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
        case PyCmp_IS: {
            llvm::Value *i = b.CreateICmpEQ(m_args[0]->LLVM_Codegen(b, func, f),
                                            m_args[1]->LLVM_Codegen(b, func, f));
            _LLVM_SetLine(i, f, m_bytecode_offset);
            return i;
        }
        case PyCmp_IS_NOT: {
            llvm::Value *i = b.CreateICmpNE(m_args[0]->LLVM_Codegen(b, func, f),
                                            m_args[1]->LLVM_Codegen(b, func, f));
            _LLVM_SetLine(i, f, m_bytecode_offset);
            return i;
        }

        case PyCmp_IN:
            vtable_idx = Object::idx__Contains__;
            break;
        case PyCmp_NOT_IN:
            vtable_idx = Object::idx__NotContains__;
            break;
        case PyCmp_EXC_MATCH: {
            llvm::Value *i = _LLVM_Call(b, func, "FPyRuntime_ExceptionCompare", (void*)&FPyRuntime_ExceptionCompare, 
                                        m_args[0]->LLVM_Codegen(b, func, f),
                                        m_args[1]->LLVM_Codegen(b, func, f));
            assert(i);
            _LLVM_SetLine(i, f, m_bytecode_offset);
            return i;
        }
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

    llvm::BasicBlock *falseBlock = blk->GetSuccessor(0)->LLVM_GetBlock(func);
    llvm::BasicBlock *trueBlock = blk->GetSuccessor(1)->LLVM_GetBlock(func);

    /* If we're branching based on an exception compare, ensure that the "matched" branch hops through a
       block that calls __cxa_end_exception. */
    if(dynamic_cast<Compare*>(m_args[0]) != 0) {
        Compare *c = dynamic_cast<Compare*>(m_args[0]);
        if(c->m_op == Compare::PyCmp_EXC_MATCH) {
            trueBlock = llvm::BasicBlock::Create(func->getContext(), "end-exception", func);
            llvm::IRBuilder<> b2(trueBlock);
            b2.CreateCall(g_cxa_end_catch);
            b2.CreateBr(blk->GetSuccessor(1)->LLVM_GetBlock(func));
        }
    }

    llvm::Value *branch_if = b.CreateCondBr(eq_test,
                                            falseBlock,
                                            trueBlock);
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

llvm::Value *_LLVM_CreateExceptionObject(llvm::IRBuilder<> &b, llvm::Function *func, Function *f, int bc_offset) {
        llvm::Value *eh = b.CreateCall(g_llvm_eh_exception, "eh_ptr");

        llvm::Value *eh_select = b.CreateCall3(g_llvm_eh_selector,
                                               eh,
                                               b.CreateBitCast(g_personality, g_u8_ptr_ty),
                                               llvm::Constant::getNullValue(g_u8_ptr_ty));
        _LLVM_SetLine(eh_select, f, bc_offset);

        llvm::Value *eh_ptr = b.CreateCall(g_cxa_begin_catch, eh);
        eh_ptr = b.CreateBitCast(eh_ptr, g_object_ty->getPointerTo());
        eh_ptr = b.CreateLoad(eh_ptr);

        f->LJ_GetCurrentBlock()->LLVM_SetExceptionObject(eh_ptr);

        llvm::BasicBlock *oldblk = b.GetInsertBlock();

        llvm::BasicBlock *blk = NULL;
        if (f->LJ_GetCurrentBlock()->LLVM_GetBlockForReRaise() == NULL) {
            blk = llvm::BasicBlock::Create(func->getContext(), "ppad", func);
        } else {
            blk = f->LJ_GetCurrentBlock()->LLVM_GetBlockForReRaise();
        }
        b.CreateBr(blk);

        b.SetInsertPoint(blk);

        // Block should start with a PHI instruction to marshal exception objects for reraises.
        // if (blk->empty()) {
        //     b.CreatePHI(eh_ptr->getType());
        // }

        // assert(!blk->empty());
        // llvm::PHINode *phi_insn = llvm::cast<llvm::PHINode>(&blk->front());
        // phi_insn->addIncoming(oldblk, eh_ptr);

        f->LJ_GetCurrentBlock()->LLVM_SetBlockForReRaise(blk);

        // return phi_insn;
        return eh_ptr;
}

llvm::Value *BeginCatch_GetValue::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {

    llvm::Value *eh_ptr = f->LJ_GetCurrentBlock()->LLVM_GetExceptionObject();
    if(eh_ptr == 0) {
        eh_ptr = _LLVM_CreateExceptionObject(b, func, f, m_bytecode_offset);
    }
    return eh_ptr;
}

llvm::Value *BeginCatch_GetType::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    llvm::Value *eh_ptr = f->LJ_GetCurrentBlock()->LLVM_GetExceptionObject();
    if(eh_ptr == 0) {
        eh_ptr = _LLVM_CreateExceptionObject(b, func, f, m_bytecode_offset);
    }

    /**@todo Type objects */

    return eh_ptr;
}

llvm::Value *BeginCatch_GetTraceback::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {

    llvm::Value *eh_ptr = f->LJ_GetCurrentBlock()->LLVM_GetExceptionObject();
    if(eh_ptr == 0) {
        eh_ptr = _LLVM_CreateExceptionObject(b, func, f, m_bytecode_offset);
    }

    int off = offsetof(Exception, m_traceback);
    return b.CreateIntToPtr(b.CreateAdd(b.CreatePtrToInt(eh_ptr,
                                                         g_nint_ty),
                                        llvm::ConstantInt::get(g_nint_ty, off)),
                            g_object_ty);
}

llvm::Value *ReRaise::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    if(f->LJ_GetCurrentBlock()->GetUnwindBlock()) {

        llvm::BasicBlock *blk = NULL;
        if (f->LJ_GetCurrentBlock()->GetUnwindBlock()->LLVM_GetBlockForReRaise() == NULL) {
            blk = llvm::BasicBlock::Create(func->getContext(), "ppad", func);
        } else {
            blk = f->LJ_GetCurrentBlock()->GetUnwindBlock()->LLVM_GetBlockForReRaise();
        }
        llvm::Value *br = b.CreateBr(blk);

        // b.SetInsertPoint(blk);

        // // Block should start with a PHI instruction to marshal exception objects for reraises.
        // if (blk->empty()) {
        //     b.CreatePHI(eh_ptr->getType());
        // }

        // assert(!blk->empty());
        // llvm::PHINode *phi_insn = llvm::cast<llvm::PHINode>(&blk->front());
        // phi_insn->addIncoming(oldblk, eh_ptr);

        f->LJ_GetCurrentBlock()->GetUnwindBlock()->LLVM_SetBlockForReRaise(blk);

        return br;
    } else {
        llvm::Value *c = b.CreateCall(g_unwind_resume_or_rethrow, b.CreateCall(g_llvm_eh_exception, "eh_ptr"));
        assert(c);
        return c;
    }
}

/* Tuple::Tuple(int) */
extern void *_ZN5TupleC1Ei;
/* Tuple::Set(long, Value*) */
extern void *_ZN5Tuple3SetElP6Object;

llvm::Value *BuildTuple::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    /* Allocate the memory for the tuple - this is seperated so that escape analysis can decide where
       the memory should be allocated (heap or stack). */
    llvm::Value *v = _LLVM_AllocMemory(this, b, func, sizeof(Tuple));
    _LLVM_Call(b, func, "_ZN5TupleC1Ei", (void*)&_ZN5TupleC1Ei, v,
               _LLVM_GetObjPtr(b, (void*)m_n));
    for(int i = 0; i < m_n; i++) {
        _LLVM_Call(b, func, "_ZN5Tuple3SetElP6Object", (void*)&_ZN5Tuple3SetElP6Object, v,_LLVM_GetObjPtr(b, (void*)i) , m_args[i]->LLVM_Codegen(b, func, f));
    }
    return v;
}



/** Note the lack of underscore - these are not cached because they require
    bitcasts, which mustn't cross fn boundaries. */
llvm::Value *Constant::LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    return _LLVM_Codegen(b, func, f);
}
llvm::Value *Constant::_LLVM_Codegen(llvm::IRBuilder<> &b, llvm::Function *func, Function *f) {
    llvm::Value *obj = _LLVM_GetObjPtr(b, (void*)this);
    assert(obj);
    return obj;
}

#endif
