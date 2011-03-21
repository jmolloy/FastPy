/**@file   BuiltinTypes.cc
 * @author James Molloy <james.molloy@arm.com>
 * @date   Sat Dec  4 13:44:55 2010
 * @brief  Definition of the built in type containers. */

#include <BuiltinTypes.h>
#include <opcode.h>
#include <Function.h>
#include <BasicBlock.h>

#include <stdio.h>
#include <cassert>

static bool s_inited = false;
static void InitOpcodeNames();
static const char *s_opcode_names[148];

Tuple::Tuple(int n) :
    Object(nTuple), m_v(n) {
}
void Tuple::Set(int idx, Object *obj) {
    m_v[idx] = obj;
}
void Tuple::Print(std::ostream &s) {
    s << "(";
    bool first = true;
    for(std::vector<Object*>::iterator it = m_v.begin();
        it != m_v.end();
        it++) {
        if(first) first = false; else s << ", ";
        (*it)->Print(s);
    }
    s << ")";
}

List::List(int n) :
    Object(nList), m_v(n) {
}
void List::Set(int idx, Object *obj) {
    m_v[idx] = obj;
}
void List::Print(std::ostream &s) {
    s << "[";
    bool first = true;
    for(std::vector<Object*>::iterator it = m_v.begin();
        it != m_v.end();
        it++) {
        if(first) first = false; else s << ", ";
        (*it)->Print(s);
    }
    s << "]";
}

Dict::Dict() :
    Object(nDict) {
}
void Dict::Set(Object *key, Object *val) {
    m_map[key] = val;
}
void Dict::Print(std::ostream &s) {
    s << "{";
    bool first = true;
    for(std::map<Object*,Object*>::iterator it = m_map.begin();
        it != m_map.end();
        it++) {
        if(first) first = false; else s << ", ";
        it->first->Print(s);
        s << ": ";
        it->second->Print(s);
    }
    s << "}";
}

MutableSet::MutableSet() {
}
void MutableSet::Insert(Object *obj) {
    m_s.insert(obj);
}
void MutableSet::Print(std::ostream &s) {
    s << "[";
    bool first = true;
    for(std::set<Object*>::iterator it = m_s.begin();
        it != m_s.end();
        it++) {
        if(first) first = false; else s << ", ";
        (*it)->Print(s);
    }
    s << "]";
}

FrozenSet::FrozenSet() {
}
void FrozenSet::Insert(Object *obj) {
    m_s.insert(obj);
}
void FrozenSet::Print(std::ostream &s) {
    s << "[";
    bool first = true;
    for(std::set<Object*>::iterator it = m_s.begin();
        it != m_s.end();
        it++) {
        if(first) first = false; else s << ", ";
        (*it)->Print(s);
    }
    s << "]";
}

Int::Int(long n) :
    Object(nInt), m_n(n) {
}
void Int::Print(std::ostream &s) {
    s << m_n;
}

Int64::Int64(int64_t n) :
    Object(nInt64), m_n(n) {
}
void Int64::Print(std::ostream &s) {
    s << m_n;
}

Float::Float(double d) :
    Object(nFloat), m_d(d) {
}
void Float::Print(std::ostream &s) {
    s << m_d;
}

String::String(std::string str) :
    Object(nString), m_str(str) {
}
void String::Print(std::ostream &s) {
    s << "\"" << m_str << "\"";
}
void String::Dot(std::ostream &s, std::string prefix) {
    s << "\"" << prefix << this << "\" [label=\"'" << m_str << "'\"];\n";
}

ByteString::ByteString(long l) :
    m_len(l) {
    m_ptr = new unsigned char[l];
}
void ByteString::Print(std::ostream &s) {
    s << "b\"";
    for(long i = 0; i < m_len; i++) {
        char c[3];
        sprintf(c, "%02x", m_ptr[i]);
        s << c << " ";
    }
    s << "\"";
}

Code::Code(long argcount, long kwonlyargcount, long nlocals, long stacksize,
           long flags, Object *code, Object *consts, Object *names,
           Object *varnames, Object *freevars, Object *cellvars, Object *filename,
           Object *name, long firstlineno, Object *lnotab) {

    m_argcount = argcount;
    m_kwonlyargcount = kwonlyargcount;
    m_nlocals = nlocals;
    m_stacksize = stacksize;
    m_flags = flags;
    m_code = dynamic_cast<ByteString*>(code)->m_ptr;
    m_code_len = dynamic_cast<ByteString*>(code)->m_len;
    m_consts = dynamic_cast<Tuple*>(consts)->m_v;
    m_names = dynamic_cast<Tuple*>(names)->m_v;
    m_varnames = dynamic_cast<Tuple*>(varnames)->m_v;
    m_freevars = dynamic_cast<Tuple*>(freevars)->m_v;
    m_cellvars = dynamic_cast<Tuple*>(cellvars)->m_v;
    m_filename = dynamic_cast<String*>(filename)->m_str;
    m_name = dynamic_cast<String*>(name)->m_str;
    m_firstlineno = firstlineno;
    m_lnotab = dynamic_cast<ByteString*>(lnotab)->m_ptr;
    m_lnotab_len = dynamic_cast<ByteString*>(lnotab)->m_len;

#if 0
    std::cout << "Code: argcount: " << argcount << std::endl;
    std::cout << "Code: kwonlyargcount: " << kwonlyargcount << std::endl;
    std::cout << "Code: nlocals: " << nlocals << std::endl;
    std::cout << "Code: stacksize: " << stacksize << std::endl;
    std::cout << "Code: flags: " << std::hex << flags << std::dec << std::endl;

    std::cout << "Code: code: ";
    code->Print(std::cout);
    std::cout << std::endl;

    std::cout << "Code: consts: ";
    consts->Print(std::cout);
    std::cout << std::endl;

    std::cout << "Code: names: ";
    names->Print(std::cout);
    std::cout << std::endl;

    std::cout << "Code: varnames: ";
    varnames->Print(std::cout);
    std::cout << std::endl;

    std::cout << "Code: freevars: ";
    freevars->Print(std::cout);
    std::cout << std::endl;

    std::cout << "Code: cellvars: ";
    cellvars->Print(std::cout);
    std::cout << std::endl;

    std::cout << "Code: filename: ";
    filename->Print(std::cout);
    std::cout << std::endl;

    std::cout << "Code: name: ";
    name->Print(std::cout);
    std::cout << std::endl;

    std::cout << "Code: firstlineno: " << firstlineno << std::endl;

    std::cout << "Code: lnotab: ";
    lnotab->Print(std::cout);
    std::cout << std::endl;

    Disassemble(std::cout);
#endif
}
void Code::Print(std::ostream &s) {
    s << "<code object @ " << this << ">";
}

unsigned char Code::NextOp(long &i, long &arg) {
    if(i >= m_code_len) {
        return STOP_CODE;
    }
    unsigned char opcode = m_code[i++];

    arg = 0;
    if(opcode == EXTENDED_ARG) {
        unsigned char tmp = m_code[i++];
        unsigned char tmp2 = m_code[i++];
        arg = (tmp<<16) | (tmp2<<24);

        opcode = m_code[i++];
    }

    if(HAS_ARG(opcode)) {
        unsigned char tmp = m_code[i++];
        unsigned char tmp2 = m_code[i++];
        if(arg == 0) {
            /* No extender argument, so sign extend ourselves. */
            short t = (tmp2 << 8) | tmp;
            arg = (long)t;
        } else {
            /* Extender argument, so just OR in the lower short. */
            arg |= (tmp2 << 8) | tmp;
        }
    }

    return opcode;
}

void Code::Disassemble(std::ostream &s) {
    InitOpcodeNames();

    long i = 0;
    long arg;
    char c[256];
    unsigned char opcode;
    while((opcode=NextOp(i, arg)) != STOP_CODE) {
        const char *opcode_str = s_opcode_names[opcode];
        if(HAS_ARG(opcode)) {
            sprintf(c, "%4ld %-32s%ld\n", (i-3), opcode_str, arg);
        } else {
            sprintf(c, "%4ld %-32s\n", (i-1), opcode_str);
        }
        s << c;
    }
}

void Code::Walk(Function *f) {

    InitOpcodeNames();

    f->SetCode(this);
    BasicBlock *b = f->GetEntryBlock();

    std::map<long,BasicBlock*> blocks;
    long i = 0; 
    bool end = false;
    while(!end) {
        long arg;
        unsigned char opcode = NextOp(i, arg);
        switch(opcode) {
            case STOP_CODE:
                end = true;
                break;
            case POP_JUMP_IF_FALSE:
            case POP_JUMP_IF_TRUE:
            case JUMP_IF_TRUE_OR_POP:
            case JUMP_IF_FALSE_OR_POP:
                if(blocks[i] == NULL) {
                    blocks[i] = new BasicBlock(f);
                }
                /* Fall through */
            case JUMP_ABSOLUTE:
                if(blocks[arg] == NULL) {
                    blocks[arg] = new BasicBlock(f);
                }
                
                break;
            case JUMP_FORWARD:
                if(blocks[arg+i] == NULL) {
                    blocks[arg+i] = new BasicBlock(f);
                }
                break;
        }
    }

    i = 0;
    bool ignore = false;
    while(true) {
        BasicBlock *b2 = blocks[i];
        if(b2 != NULL) {
            if(!ignore)
                b->Jump(b2);
            b = b2;
        }
        ignore = false;

        long arg;
        unsigned char opcode = NextOp(i, arg);

        switch(opcode) {
            case STOP_CODE:
                return;

            case LOAD_CONST:
                b->LoadConstant(m_consts[arg]);
                break;

            case LOAD_NAME:
                b->LoadGlobal(dynamic_cast<String*>(m_names[arg])->m_str);
                break;

            case LOAD_GLOBAL:
                b->LoadGlobal(dynamic_cast<String*>(m_names[arg])->m_str);
                break;

            case LOAD_FAST:
                b->LoadLocal(dynamic_cast<String*>(m_varnames[arg])->m_str);
                break;

            case STORE_NAME:
                b->StoreGlobal(dynamic_cast<String*>(m_names[arg])->m_str);
                break;

            case STORE_GLOBAL:
                b->StoreGlobal(dynamic_cast<String*>(m_names[arg])->m_str);
                break;

            case STORE_FAST:
                b->StoreLocal(dynamic_cast<String*>(m_varnames[arg])->m_str);
                break;

            case MAKE_FUNCTION:
                b->BindClosure(arg /*Num Default Parameters*/);
                break;

            case CALL_FUNCTION:
                b->Call( arg&0xFF, /* Num Positional Parameters */
                         (arg>>8)&0xFF); /* Num Keyword Parameters */
                break;

            case POP_TOP:
                b->Pop();
                break;

            case RETURN_VALUE:
                b->Return();
                break;

            case POP_JUMP_IF_FALSE: {
                BasicBlock *bl1 = blocks[arg];
                BasicBlock *bl2 = blocks[i];
                assert(bl1);
                assert(bl2);
                b->ConditionalJump(true, false, bl1, bl2);
                b = bl2;
                break;
            }

            case POP_JUMP_IF_TRUE: {
                BasicBlock *bl1 = blocks[arg];
                BasicBlock *bl2 = blocks[i];
                assert(bl1);
                assert(bl2);
                b->ConditionalJump(true, true, bl1, bl2);
                b = bl2;
                break;
            }

            case JUMP_FORWARD: {
                BasicBlock *bl = blocks[arg+i];
                assert(bl);
                b->Jump(bl);
                ignore = true;
                break;
            }

            case BINARY_ADD:
                b->BinaryOp("__add__");
                break;

            case LOAD_ATTR:
                b->GetAttr(dynamic_cast<String*>(m_names[arg])->m_str);
                break;
                
            default:
                std::cerr << "Unhandled opcode: " << s_opcode_names[opcode] << "\n";
        }
    }
    
    return;
}

static void InitOpcodeNames() {
    if(s_inited) {
        return;
    }
    s_inited = true;

    s_opcode_names[0] = "STOP_CODE";
    s_opcode_names[1] = "POP_TOP";
    s_opcode_names[2] = "ROT_TWO";
    s_opcode_names[3] = "ROT_THREE";
    s_opcode_names[4] = "DUP_TOP";
    s_opcode_names[5] = "ROT_FOUR";
    s_opcode_names[9] = "NOP";

    s_opcode_names[10] = "UNARY_POSITIVE";
    s_opcode_names[11] = "UNARY_NEGATIVE";
    s_opcode_names[12] = "UNARY_NOT";

    s_opcode_names[15] = "UNARY_INVERT";

    s_opcode_names[19] = "BINARY_POWER";

    s_opcode_names[20] = "BINARY_MULTIPLY";

    s_opcode_names[22] = "BINARY_MODULO";
    s_opcode_names[23] = "BINARY_ADD";
    s_opcode_names[24] = "BINARY_SUBTRACT";
    s_opcode_names[25] = "BINARY_SUBSCR";
    s_opcode_names[26] = "BINARY_FLOOR_DIVIDE";
    s_opcode_names[27] = "BINARY_TRUE_DIVIDE";
    s_opcode_names[28] = "INPLACE_FLOOR_DIVIDE";
    s_opcode_names[29] = "INPLACE_TRUE_DIVIDE";

    s_opcode_names[54] = "STORE_MAP";
    s_opcode_names[55] = "INPLACE_ADD";
    s_opcode_names[56] = "INPLACE_SUBTRACT";
    s_opcode_names[57] = "INPLACE_MULTIPLY";

    s_opcode_names[59] = "INPLACE_MODULO";
    s_opcode_names[60] = "STORE_SUBSCR";
    s_opcode_names[61] = "DELETE_SUBSCR";

    s_opcode_names[62] = "BINARY_LSHIFT";
    s_opcode_names[63] = "BINARY_RSHIFT";
    s_opcode_names[64] = "BINARY_AND";
    s_opcode_names[65] = "BINARY_XOR";
    s_opcode_names[66] = "BINARY_OR";
    s_opcode_names[67] = "INPLACE_POWER";
    s_opcode_names[68] = "GET_ITER";
    s_opcode_names[69] = "STORE_LOCALS";
    s_opcode_names[70] = "PRINT_EXPR";
    s_opcode_names[71] = "LOAD_BUILD_CLASS";

    s_opcode_names[75] = "INPLACE_LSHIFT";
    s_opcode_names[76] = "INPLACE_RSHIFT";
    s_opcode_names[77] = "INPLACE_AND";
    s_opcode_names[78] = "INPLACE_XOR";
    s_opcode_names[79] = "INPLACE_OR";
    s_opcode_names[80] = "BREAK_LOOP";
    s_opcode_names[81] = "WITH_CLEANUP";

    s_opcode_names[83] = "RETURN_VALUE";
    s_opcode_names[84] = "IMPORT_STAR";

    s_opcode_names[86] = "YIELD_VALUE";
    s_opcode_names[87] = "POP_BLOCK";
    s_opcode_names[88] = "END_FINALLY";
    s_opcode_names[89] = "POP_EXCEPT";

    s_opcode_names[90] = "HAVE_ARGUMENT";	/* Opcodes from here have an argument: */

    s_opcode_names[90] = "STORE_NAME";	/* Index in name list */
    s_opcode_names[91] = "DELETE_NAME";	/* "" */
    s_opcode_names[92] = "UNPACK_SEQUENCE";	/* Number of sequence items */
    s_opcode_names[93] = "FOR_ITER";
    s_opcode_names[94] = "UNPACK_EX";      /* Num items before variable part +
                                              (Num items after variable part << 8) */

    s_opcode_names[95] = "STORE_ATTR";	/* Index in name list */
    s_opcode_names[96] = "DELETE_ATTR";	/* "" */
    s_opcode_names[97] = "STORE_GLOBAL";	/* "" */
    s_opcode_names[98] = "DELETE_GLOBAL";	/* "" */
    s_opcode_names[99] = "DUP_TOPX";	/* number of items to duplicate */
    s_opcode_names[100] = "LOAD_CONST";	/* Index in const list */
    s_opcode_names[101] = "LOAD_NAME";	/* Index in name list */
    s_opcode_names[102] = "BUILD_TUPLE";	/* Number of tuple items */
    s_opcode_names[103] = "BUILD_LIST";	/* Number of list items */
    s_opcode_names[104] = "BUILD_SET";     /* Number of set items */
    s_opcode_names[105] = "BUILD_MAP";	/* Always zero for now */
    s_opcode_names[106] = "LOAD_ATTR";	/* Index in name list */
    s_opcode_names[107] = "COMPARE_OP";	/* Comparison operator */
    s_opcode_names[108] = "IMPORT_NAME";	/* Index in name list */
    s_opcode_names[109] = "IMPORT_FROM";	/* Index in name list */

    s_opcode_names[110] = "JUMP_FORWARD";	/* Number of bytes to skip */
    s_opcode_names[111] = "JUMP_IF_FALSE_OR_POP";	/* Target byte offset from beginning of code */
    s_opcode_names[112] = "JUMP_IF_TRUE_OR_POP";	/* "" */
    s_opcode_names[113] = "JUMP_ABSOLUTE";	/* "" */
    s_opcode_names[114] = "POP_JUMP_IF_FALSE";	/* "" */
    s_opcode_names[115] = "POP_JUMP_IF_TRUE";	/* "" */

    s_opcode_names[116] = "LOAD_GLOBAL";	/* Index in name list */

    s_opcode_names[119] = "CONTINUE_LOOP";	/* Start of loop (absolute) */
    s_opcode_names[120] = "SETUP_LOOP";	/* Target address (relative) */
    s_opcode_names[121] = "SETUP_EXCEPT";	/* "" */
    s_opcode_names[122] = "SETUP_FINALLY";	/* "" */

    s_opcode_names[124] = "LOAD_FAST";	/* Local variable number */
    s_opcode_names[125] = "STORE_FAST";	/* Local variable number */
    s_opcode_names[126] = "DELETE_FAST";	/* Local variable number */

    s_opcode_names[130] = "RAISE_VARARGS";	/* Number of raise arguments (1, 2 or 3) */
/* CALL_FUNCTION_S_OPCODE_NAMES opcodes defined below depend on this definition */
    s_opcode_names[131] = "CALL_FUNCTION";	/* #args + (#kwargs<<8) */
    s_opcode_names[132] = "MAKE_FUNCTION";	/* #defaults + #kwdefaults<<8 + #annotations<<16 */
    s_opcode_names[133] = "BUILD_SLICE";	/* Number of items */

    s_opcode_names[134] = "MAKE_CLOSURE";     /* same as MAKE_FUNCTION */
    s_opcode_names[135] = "LOAD_CLOSURE";     /* Load free variable from closure */
    s_opcode_names[136] = "LOAD_DEREF";     /* Load and dereference from closure cell */ 
    s_opcode_names[137] = "STORE_DEREF";     /* Store into cell */ 

/* The next 3 opcodes must be contiguous and satisfy
   (CALL_FUNCTION_VAR - CALL_FUNCTION) & 3 == 1  */
    s_opcode_names[140] = "CALL_FUNCTION_VAR";	/* #args + (#kwargs<<8) */
    s_opcode_names[141] = "CALL_FUNCTION_KW";	/* #args + (#kwargs<<8) */
    s_opcode_names[142] = "CALL_FUNCTION_VAR_KW";	/* #args + (#kwargs<<8) */

/* Support for opargs more than 16 bits long */
    s_opcode_names[143] = "EXTENDED_ARG";

    s_opcode_names[145] = "LIST_APPEND";
    s_opcode_names[146] = "SET_ADD";
    s_opcode_names[147] = "MAP_ADD";
}
