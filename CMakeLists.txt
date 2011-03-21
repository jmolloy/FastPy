cmake_minimum_required(VERSION 2.6)

project(FastPy)
set(FastPy_VERSION_MAJOR 0)
set(FastPy_VERSION_MINOR 1)

set(SRCS
  src/Constant.cc
  src/Type.cc
  src/constants.cc
  src/variables.cc
  src/main.cc
  src/Code.cc
  src/Marshal.cc
  src/Function.cc
  src/BasicBlock.cc
  src/OperandStack.cc
  src/Instruction.cc
  src/instructions.cc
  src/Module.cc
  src/LJ_Codegen.cc
  src/PyDict.cc
  src/PyString.cc
  src/PyNone.cc
  src/PyRuntime.cc
  src/PyFunction.cc
)

find_library(LIBJIT jit)

include_directories(include)
add_executable(fastpy ${SRCS})
target_link_libraries(fastpy ${LIBJIT})
