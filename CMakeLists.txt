cmake_minimum_required(VERSION 2.6)

project(FastPy)
set(FastPy_VERSION_MAJOR 0)
set(FastPy_VERSION_MINOR 1)

set(SRCS
  src/Constant.cc
  src/Type.cc
  src/constants.cc
  src/variables.cc
  src/Code.cc
  src/Marshal.cc
  src/Function.cc
  src/BasicBlock.cc
  src/OperandStack.cc
  src/Instruction.cc
  src/instructions.cc
  src/Module.cc
  src/LJ_Codegen.cc
  src/LLVM_Codegen.cc
  src/LLVM_Support.cc
  src/PyRuntime.cc
  src/Object.cc
  src/Exception.cc
  src/Traceback.cc
  src/db.cc
)

find_library(LIBJIT jit)
find_path(LIBJIT_INC jit/jit.h)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(Python)
find_package(Readline)
find_package(LLVM)

include_directories(${LIBJIT_INC})
include_directories(include)
if(READLINE_FOUND)
  include_directories(${READLINE_INCLUDE_DIR})
endif()
if(PYTHONLIBS_FOUND)
  include_directories(${PYTHON_INCLUDE_DIR})
endif()

if(LLVM_FOUND)
  add_definitions(-DWITH_LLVM=1 ${LLVM_CFLAGS})
endif()
if(READLINE_FOUND AND PYTHONLIBS_FOUND)
  add_definitions(-DREPL=1)
  set(SRCS ${SRCS} src/repl.cc src/repl_readline.cc)
endif()


add_executable(fastpy src/main.cc ${SRCS})
target_link_libraries(fastpy ${LIBJIT} ${LLVM_JIT_LIBS} pthread dl util)


if(LLVM_FOUND)
  target_link_libraries(fastpy ${LLVM_LFLAGS})
endif()
if(READLINE_FOUND AND PYTHONLIBS_FOUND)
  target_link_libraries(fastpy ${PYTHON_LIBRARY} ${READLINE_LIBRARY})
endif()
