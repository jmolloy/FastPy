#ifndef DB_H
#define DB_H

#define DB_PRINT_BYTECODE 1
#define DB_PRINT_IR       2
#define DB_PRINT_LJ       4
#define DB_PRINT_LLVM     8
#define DB_PRINT_ASM      16
#define DB_PRINT_C_CALLS  32

bool db_print(Function *fn, int type);

#endif