#include <constants.h>
#include <opcode.h>
#include <stdio.h>
#include <BasicBlock.h>
#include <Function.h>
#include <variables.h>

static bool s_inited = false;
static void InitOpcodeNames();
static const char *s_opcode_names[148];


Code::Code(long argcount, long kwonlyargcount, long nlocals, long stacksize,
           long flags, ConstantByteString *code, Tuple *consts, Tuple *names,
           Tuple *varnames, Tuple *freevars, Tuple *cellvars, ConstantString *filename,
           ConstantString *name, long firstlineno, ConstantByteString *lnotab) :
    m_argcount(argcount), m_kwonlyargcount(kwonlyargcount), m_nlocals(nlocals),
    m_stacksize(stacksize), m_flags(flags), m_code(code), m_consts(consts),
    m_names(names), m_varnames(varnames), m_freevars(freevars), m_cellvars(cellvars),
    m_filename(filename), m_name(name), m_lnotab(lnotab)
{
    SetType(Type::GetCodeTy());
}

const std::string Code::Repr() {
    return "<code>";
}

unsigned char Code::NextOp(long &i, long &arg) {
    if(i >= m_code->m_sz) {
        return STOP_CODE;
    }
    unsigned char *code = m_code->m_ptr;

    unsigned char opcode = code[i++];

    arg = 0;
    if(opcode == EXTENDED_ARG) {
        unsigned char tmp = code[i++];
        unsigned char tmp2 = code[i++];
        arg = (tmp<<16) | (tmp2<<24);

        opcode = code[i++];
    }

    if(HAS_ARG(opcode)) {
        unsigned char tmp = code[i++];
        unsigned char tmp2 = code[i++];
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

int Code::GetLineNo(int offs) {
    /* http://hg.python.org/cpython/file/default/Objects/lnotab_notes.txt */

    int lineno = m_firstlineno+1;
    int addr = 0;
    unsigned char *lnotab = m_lnotab->GetPtr();
    int i = 0;

    while(i < m_lnotab->m_sz) {
        unsigned char addr_incr = lnotab[i++];
        unsigned char line_incr = lnotab[i++];

        addr += addr_incr;
        if(addr > offs) {
            return lineno;
        }
        lineno += line_incr;
    }
    return lineno;
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

    int id = 1;
    BasicBlock *b = f->GetEntryBlock();
    b->SetId(++id);

    std::map<long,BasicBlock*> blocks;
    std::map<long,bool> catch_location;
    long i = 0;

    blocks[0] = b;

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
                    blocks[i]->SetId(++id);
                    f->AddBlock(blocks[i]);
                }
                /* Fall through */
            case JUMP_ABSOLUTE:
                if(blocks[arg] == NULL) {
                    blocks[arg] = new BasicBlock(f);
                    blocks[arg]->SetId(++id);
                    f->AddBlock(blocks[arg]);
                }
                
                break;
            case JUMP_FORWARD:
                if(blocks[arg+i] == NULL) {
                    blocks[arg+i] = new BasicBlock(f);
                    blocks[arg+i]->SetId(++id);
                    f->AddBlock(blocks[arg+i]);
                }
                if(blocks[i] == NULL) {
                    blocks[i] = new BasicBlock(f);
                    blocks[i]->SetId(++id);
                    f->AddBlock(blocks[i]);
                }
                break;
            case SETUP_EXCEPT:
                if(blocks[i-3] == NULL) {
                    blocks[i-3] = new BasicBlock(f);
                    blocks[i-3]->SetId(++id);
                    f->AddBlock(blocks[i-3]);
                }
                break;
            case POP_BLOCK:
            case SETUP_FINALLY:
            case END_FINALLY:
                if(blocks[i] == NULL) {
                    blocks[i] = new BasicBlock(f);
                    blocks[i]->SetId(++id);
                    f->AddBlock(blocks[i]);
                }
                break;
        }
    }

    id = 1;
    i = 0;
    bool ignore = false;
    while(true) {
        BasicBlock *oldb = b;
        BasicBlock *b2 = blocks[i];
        if(b2 != NULL) {
            if(!ignore && b2 != b) {
                b->Jump(b2, id, i);
                b2->AddPredecessor(b, id);
            }
            b = b2;
        }
        ignore = false;

        if(catch_location[i] == true) {
            b->BeginCatch(id);
        }

        int insn_bytecode_offset = i;

        long arg;
        unsigned char opcode = NextOp(i, arg);

        switch(opcode) {
            case STOP_CODE:
                return;

            case PRINT_ITEM:
                b->PrintItem(id, insn_bytecode_offset);
                break;

            case PRINT_NEWLINE:
                b->PrintNewline(id, insn_bytecode_offset);
                break;

            case LOAD_CONST:
                b->LoadConstant(Constant::From(m_consts->Get(arg)), id, insn_bytecode_offset);
                break;

            case LOAD_NAME:
                b->LoadGlobal(ConstantString::From(m_names->Get(arg))->str(), id, insn_bytecode_offset);
                break;

            case LOAD_GLOBAL:
                b->LoadGlobal(ConstantString::From(m_names->Get(arg))->str(), id, insn_bytecode_offset);
                break;

            case LOAD_FAST:
                b->LoadLocal(ConstantString::From(m_varnames->Get(arg))->str(), id, insn_bytecode_offset);
                break;

            case STORE_NAME:
                b->StoreGlobal(ConstantString::From(m_names->Get(arg))->str(), id, insn_bytecode_offset);
                break;

            case STORE_GLOBAL:
                b->StoreGlobal(ConstantString::From(m_names->Get(arg))->str(), id, insn_bytecode_offset);
                break;

            case STORE_FAST:
                b->StoreLocal(ConstantString::From(m_varnames->Get(arg))->str(), id, insn_bytecode_offset);
                break;

            // case MAKE_FUNCTION:
            //     b->BindClosure(arg /*Num Default Parameters*/, id, insn_bytecode_offset);
            //     break;

            case CALL_FUNCTION:
                b->Call( arg&0xFF, /* Num Positional Parameters */
                         (arg>>8)&0xFF, id, insn_bytecode_offset); /* Num Keyword Parameters */
                break;

            case POP_TOP:
                b->Pop(id);
                break;

            case RETURN_VALUE:
                b->Return(id, insn_bytecode_offset);
                break;

            case POP_JUMP_IF_FALSE: {
                BasicBlock *bl1 = blocks[arg];
                BasicBlock *bl2 = blocks[i];
                assert(bl1);
                assert(bl2);
                b->ConditionalJump(true, false, bl1, bl2, id, insn_bytecode_offset);

                bl1->AddPredecessor(b, id);
                bl2->AddPredecessor(b, id);

                b = bl2;
                break;
            }

            case POP_JUMP_IF_TRUE: {
                BasicBlock *bl1 = blocks[arg];
                BasicBlock *bl2 = blocks[i];
                assert(bl1);
                assert(bl2);
                b->ConditionalJump(true, true, bl1, bl2, id, insn_bytecode_offset);

                bl1->AddPredecessor(b, id);
                bl2->AddPredecessor(b, id);

                b = bl2;
                break;
            }

            case JUMP_FORWARD: {
                BasicBlock *bl = blocks[arg+i];
                assert(bl);
                b->Jump(bl, id, insn_bytecode_offset);

                bl->AddPredecessor(b, id);
                ignore = true;
                break;
            }

            case BINARY_ADD:
                b->BinaryOp("__add__", id, insn_bytecode_offset);
                break;

            case LOAD_ATTR:
                b->GetAttr(ConstantString::From(m_names->Get(arg))->str(), id, insn_bytecode_offset);
                break;

            case SETUP_EXCEPT: {
                catch_location[i+arg] = true;

                for(std::map<long,BasicBlock*>::iterator it = blocks.begin();
                    it != blocks.end();
                    ++it) {
                    if(it->first >= i && it->first < i+arg) {
                        it->second->SetUnwindBlock(blocks[i+arg]);
                    }
                }
                b->SetUnwindBlock(blocks[i+arg]);
                break;
            }


            case POP_BLOCK:
                break;

            case BUILD_TUPLE:
                b->BuildTuple(arg, id, insn_bytecode_offset);
                break;

            case DUP_TOP:
                b->Dup();
                break;

            case COMPARE_OP:
                b->Compare(arg, id, insn_bytecode_offset);
                break;

            case END_FINALLY:
                b->ReRaise(id, insn_bytecode_offset);
                break;
                
            default:
                std::cerr << "Unhandled opcode: " << s_opcode_names[opcode] << "\n";
        }
    }
}

static void InitOpcodeNames() {
    if(s_inited) {
        return;
    }
    s_inited = true;

    // Generated with: sed -e 's/#define \([A-Z_]*\)[ \t]*\([0-9]*\)/s_opcode_names[\2] = "\1";/'

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
    s_opcode_names[13] = "UNARY_CONVERT";

    s_opcode_names[15] = "UNARY_INVERT";

    s_opcode_names[19] = "BINARY_POWER";

    s_opcode_names[20] = "BINARY_MULTIPLY";
    s_opcode_names[21] = "BINARY_DIVIDE";
    s_opcode_names[22] = "BINARY_MODULO";
    s_opcode_names[23] = "BINARY_ADD";
    s_opcode_names[24] = "BINARY_SUBTRACT";
    s_opcode_names[25] = "BINARY_SUBSCR";
    s_opcode_names[26] = "BINARY_FLOOR_DIVIDE";
    s_opcode_names[27] = "BINARY_TRUE_DIVIDE";
    s_opcode_names[28] = "INPLACE_FLOOR_DIVIDE";
    s_opcode_names[29] = "INPLACE_TRUE_DIVIDE";

    s_opcode_names[30] = "SLICE";
/* Also uses 31-33 */

    s_opcode_names[40] = "STORE_SLICE";
/* Also uses 41-43 */

    s_opcode_names[50] = "DELETE_SLICE";
/* Also uses 51-53 */

    s_opcode_names[54] = "STORE_MAP";
    s_opcode_names[55] = "INPLACE_ADD";
    s_opcode_names[56] = "INPLACE_SUBTRACT";
    s_opcode_names[57] = "INPLACE_MULTIPLY";
    s_opcode_names[58] = "INPLACE_DIVIDE";
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

    s_opcode_names[70] = "PRINT_EXPR";
    s_opcode_names[71] = "PRINT_ITEM";
    s_opcode_names[72] = "PRINT_NEWLINE";
    s_opcode_names[73] = "PRINT_ITEM_TO";
    s_opcode_names[74] = "PRINT_NEWLINE_TO";
    s_opcode_names[75] = "INPLACE_LSHIFT";
    s_opcode_names[76] = "INPLACE_RSHIFT";
    s_opcode_names[77] = "INPLACE_AND";
    s_opcode_names[78] = "INPLACE_XOR";
    s_opcode_names[79] = "INPLACE_OR";
    s_opcode_names[80] = "BREAK_LOOP";
    s_opcode_names[81] = "WITH_CLEANUP";
    s_opcode_names[82] = "LOAD_LOCALS";
    s_opcode_names[83] = "RETURN_VALUE";
    s_opcode_names[84] = "IMPORT_STAR";
    s_opcode_names[85] = "EXEC_STMT";
    s_opcode_names[86] = "YIELD_VALUE";
    s_opcode_names[87] = "POP_BLOCK";
    s_opcode_names[88] = "END_FINALLY";
    s_opcode_names[89] = "BUILD_CLASS";

    s_opcode_names[90] = "HAVE_ARGUMENT";	/* Opcodes from here have an argument: */

    s_opcode_names[90] = "STORE_NAME";	/* Index in name list */
    s_opcode_names[91] = "DELETE_NAME";	/* "" */
    s_opcode_names[92] = "UNPACK_SEQUENCE";	/* Number of sequence items */
    s_opcode_names[93] = "FOR_ITER";
    s_opcode_names[94] = "LIST_APPEND";

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

    s_opcode_names[111] = "JUMP_IF_FALSE_OR_POP"; /* Target byte offset from beginning
                                                     of code */
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
/* CALL_FUNCTION_XXX opcodes defined below depend on this definition */
    s_opcode_names[131] = "CALL_FUNCTION";	/* #args + (#kwargs<<8) */
    s_opcode_names[132] = "MAKE_FUNCTION";	/* #defaults */
    s_opcode_names[133] = "BUILD_SLICE";	/* Number of items */

    s_opcode_names[134] = "MAKE_CLOSURE";     /* #free vars */
    s_opcode_names[135] = "LOAD_CLOSURE";     /* Load free variable from closure */
    s_opcode_names[136] = "LOAD_DEREF";     /* Load and dereference from closure cell */ 
    s_opcode_names[137] = "STORE_DEREF";     /* Store into cell */ 

/* The next 3 opcodes must be contiguous and satisfy
   (CALL_FUNCTION_VAR - CALL_FUNCTION) & 3 == 1  */
    s_opcode_names[140] = "CALL_FUNCTION_VAR";	/* #args + (#kwargs<<8) */
    s_opcode_names[141] = "CALL_FUNCTION_KW";	/* #args + (#kwargs<<8) */
    s_opcode_names[142] = "CALL_FUNCTION_VAR_KW";	/* #args + (#kwargs<<8) */

    s_opcode_names[143] = "SETUP_WITH";

/* Support for opargs more than 16 bits long */
    s_opcode_names[145] = "EXTENDED_ARG";

    s_opcode_names[146] = "SET_ADD";
    s_opcode_names[147] = "MAP_ADD";
}
