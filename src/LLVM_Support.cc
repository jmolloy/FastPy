#if defined(WITH_LLVM)

#include <LLVM_Support.h>

#include <llvm/ADT/Twine.h>
#include <llvm/Constant.h>
#include <llvm/DerivedTypes.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Function.h>
#include <llvm/Intrinsics.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Type.h>
#include <llvm/CodeGen/MachineFunction.h>

#include <Object.h>
#include <Exception.h>

#include <iostream>


struct EmissionDetails {
    llvm::JITEvent_EmittedFunctionDetails Details;
    llvm::Function *F;
};

class FastPyJITListener : public llvm::JITEventListener {
public:
    FastPyJITListener();
    virtual ~FastPyJITListener();

    virtual void NotifyFunctionEmitted(const llvm::Function &F,
                                       void *Code,
                                       size_t Size,
                                       const llvm::JITEvent_EmittedFunctionDetails &Details);

    virtual void NotifyFreeingMachineCode(void *OldPtr);
};
typedef std::map<uintptr_t,std::pair<size_t,EmissionDetails> > JittedFunctionMap;

llvm::Module *g_llvm_module;
llvm::ExecutionEngine *g_llvm_engine;
JittedFunctionMap g_llvm_jitted_fns;

std::map<const llvm::Function*, Function*> g_llvm_function_map;

llvm::MDNode *g_llvm_compilation_unit;

llvm::Type *g_object_ty;
const llvm::Type *g_u8_ptr_ty, *g_nint_ty;
llvm::Function *g_fn_callc0, *g_fn_callc1, *g_fn_callc2, *g_fn_callc3, *g_fn_callc4, *g_fn_callc5;
llvm::Value *g_llvm_eh_exception, *g_llvm_eh_selector, *g_personality;

static llvm::FunctionType * _LLVM_fn_signature(int nargs) {
    /* Account for the extra argument - must call FPyRuntime_CallC with it! */
    nargs++;

    std::vector<const llvm::Type*> args;
    for(int i = 0; i < nargs; i++) {
        args.push_back(g_object_ty);
    }
    llvm::FunctionType *fn_type = llvm::FunctionType::get(g_object_ty, args, false);
    assert(fn_type);

    return fn_type;
}


llvm::Module *LLVM_Initialize() {
    llvm::UnwindTablesMandatory = true;
    llvm::JITExceptionHandling = true;
    llvm::JITEmitDebugInfo = true;
    llvm::NoFramePointerElim = true;
    llvm::InitializeNativeTarget();

    llvm::LLVMContext &context = llvm::getGlobalContext();
    llvm::Module *m = new llvm::Module("FastPy", context);
    std::string error_str;
    llvm::EngineBuilder eb(m);
    eb.setErrorStr(&error_str);
    eb.setEngineKind(llvm::EngineKind::JIT);
    g_llvm_engine = eb.create();
    if(!g_llvm_engine) {
        std::cerr << "LLVM JIT creation failed: " << error_str << std::endl;
        exit(1);
    }
    assert(g_llvm_engine);

    g_llvm_engine->RegisterJITEventListener(new FastPyJITListener());

    g_u8_ptr_ty = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(m->getContext()));

    llvm::Type *vtable_ty = llvm::ArrayType::get(g_u8_ptr_ty, 20);

    g_object_ty = llvm::StructType::get(m->getContext(),
                                        llvm::PointerType::getUnqual(vtable_ty),
                                        NULL);
    g_object_ty = llvm::PointerType::getUnqual(g_object_ty);

    g_fn_callc0 = llvm::Function::Create(_LLVM_fn_signature(0),
                                         llvm::Function::ExternalLinkage,
                                         "FPyRuntime_CallC_LLVM",
                                         m);
    g_fn_callc1 = llvm::Function::Create(_LLVM_fn_signature(1),
                                         llvm::Function::ExternalLinkage,
                                         "FPyRuntime_CallC_LLVM1",
                                         m);
    g_fn_callc2 = llvm::Function::Create(_LLVM_fn_signature(2),
                                         llvm::Function::ExternalLinkage,
                                         "FPyRuntime_CallC_LLVM2",
                                         m);
    g_fn_callc3 = llvm::Function::Create(_LLVM_fn_signature(3),
                                         llvm::Function::ExternalLinkage,
                                         "FPyRuntime_CallC_LLVM3",
                                         m);
    g_fn_callc4 = llvm::Function::Create(_LLVM_fn_signature(4),
                                         llvm::Function::ExternalLinkage,
                                         "FPyRuntime_CallC_LLVM4",
                                         m);
    g_fn_callc5 = llvm::Function::Create(_LLVM_fn_signature(5),
                                         llvm::Function::ExternalLinkage,
                                         "FPyRuntime_CallC_LLVM5",
                                         m);

    assert(g_fn_callc0);
    assert(g_fn_callc1);
    assert(g_fn_callc2);
    assert(g_fn_callc3);
    assert(g_fn_callc4);
    assert(g_fn_callc5);

    g_nint_ty = (sizeof(void*) == 8) ? llvm::Type::getInt64Ty(m->getContext()) : llvm::Type::getInt32Ty(m->getContext());

    llvm::Intrinsic::getDeclaration(m, llvm::Intrinsic::eh_exception);
    llvm::Intrinsic::getDeclaration(m, llvm::Intrinsic::eh_selector);

    g_llvm_eh_exception = m->getFunction("llvm.eh.exception");
    assert(g_llvm_eh_exception);

    g_llvm_eh_selector = m->getFunction("llvm.eh.selector");
    assert(g_llvm_eh_selector);

    std::vector<const llvm::Type*> args;
    llvm::FunctionType *fn_type = llvm::FunctionType::get(g_u8_ptr_ty, args, true);
    assert(fn_type);

    g_personality = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, "__gxx_personality_v0", m);
    assert(g_personality);

    const llvm::Type *int32ty = llvm::Type::getInt32Ty(context);

    llvm::Value *args2[10];
    args2[0] = llvm::ConstantInt::get(int32ty, 458769); // [DW_TAG_compile_unit ]
    args2[1] = llvm::ConstantInt::get(int32ty, 0);
    args2[2] = llvm::ConstantInt::get(int32ty, 12);
    args2[3] = llvm::MDString::get(context, "x.c");
    args2[4] = llvm::MDString::get(context, "/y/z");
    args2[5] = llvm::MDString::get(context, "FastPy");
    args2[6] = llvm::ConstantInt::getTrue(context);
    args2[7] = llvm::ConstantInt::getFalse(context);
    args2[8] = llvm::MDString::get(context, "");
    args2[9] = llvm::ConstantInt::get(int32ty, 0);

    g_llvm_compilation_unit = llvm::MDNode::get(context, args2, 10);

    g_llvm_module = m;

    return m;
}

Object * LLVM_Invoke(void *p) {
    Object * (*fnp)(void) = ( Object* (*)(void) ) p;

    try{
        Object *result = fnp();
        return result;
    } catch(Exception *e) {
        std::cerr << e->Repr() << std::endl;
        return 0;
    }
    
}


FastPyJITListener::FastPyJITListener() {
}

FastPyJITListener::~FastPyJITListener() {
}

void FastPyJITListener::NotifyFunctionEmitted(const llvm::Function &F,
                                              void *Code,
                                              size_t Size,
                                              const llvm::JITEvent_EmittedFunctionDetails &Details) {
    /** @todo Locking */
    EmissionDetails d;
    d.Details = Details;
    d.F = (llvm::Function*)&F;
    g_llvm_jitted_fns[(uintptr_t)Code] = std::make_pair(Size,d);
}

void FastPyJITListener::NotifyFreeingMachineCode(void *OldPtr) {
    /** @todo Locking */
    JittedFunctionMap::iterator it = g_llvm_jitted_fns.find((uintptr_t)OldPtr);
    if(it != g_llvm_jitted_fns.end()) {
        g_llvm_jitted_fns.erase(it);
    } else {
        std::cerr << "Warning: Trying to remove nonexistant function address " << std::hex << OldPtr << std::dec << " from function map.\n";
    }
}

bool LLVM_AddressContainedInJITFunction(void *address) {
    for(JittedFunctionMap::iterator it = g_llvm_jitted_fns.begin();
        it != g_llvm_jitted_fns.end();
        it++) {
        if((uintptr_t)address > it->first &&
           (uintptr_t)address < (it->first+it->second.first)) {
            return true;
        }
    }
}
Function *LLVM_GetFunction(void *address) {
    for(JittedFunctionMap::iterator it = g_llvm_jitted_fns.begin();
        it != g_llvm_jitted_fns.end();
        it++) {
        if((uintptr_t)address > it->first &&
           (uintptr_t)address < (it->first+it->second.first)) {
            const llvm::Function *f = it->second.second.F;
            assert(f);

            Function *f_ = g_llvm_function_map[f];
            assert(f_);
            return f_;
        }
    }
    return 0;
}
int LLVM_GetLineNo(void *address) {
    for(JittedFunctionMap::iterator it = g_llvm_jitted_fns.begin();
        it != g_llvm_jitted_fns.end();
        it++) {
        if((uintptr_t)address > it->first &&
           (uintptr_t)address < (it->first+it->second.first)) {
            const llvm::Function *f = it->second.second.F;
            assert(f);
            
            llvm::JITEvent_EmittedFunctionDetails &Details = it->second.second.Details;
            int line = 0;
            for(std::vector<llvm::JITEvent_EmittedFunctionDetails::LineStart>::iterator it = Details.LineStarts.begin();
                it != Details.LineStarts.end();
                it++) {
                if((uintptr_t)address >= it->Address) {
                    line = it->Loc.getLine();
                }
            }
            return line;
        }
    }
    return 0;
}


#endif
