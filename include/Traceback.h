#ifndef TRACEBACK_H
#define TRACEBACK_H

#include <Constant.h>
#include <list>
#include <jit/jit.h>

#define TRACEBACK_SHOW_BUILTINS 1

class Traceback : public Constant {
public:
    const std::string Repr();

    void AddTraceFromHere();

private:
    jit_stack_trace_t m_stack_trace;
};

#endif
