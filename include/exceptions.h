#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <Exception.h>

class StandardError : public Exception {
public:
    StandardError(const std::string &desc) :
        Exception("StandardError", desc) {
        m_traceback->AddTraceFromHere();
    }
    StandardError(const std::string &name, const std::string &desc) :
        Exception(name, desc) {
    }

};

class RuntimeError : public StandardError {
public:
    RuntimeError(const std::string &desc) :
        StandardError("RuntimeError", desc) {
        m_traceback->AddTraceFromHere();
    }
    RuntimeError(const std::string &name, const std::string &desc) :
        StandardError(name, desc) {
    }

};

class NotImplementedError : public RuntimeError {
public:
    NotImplementedError(const std::string &desc) :
        RuntimeError("NotImplementedError", desc) {
        m_traceback->AddTraceFromHere();
    }

};

class TypeError : public RuntimeError {
public:
    TypeError(const std::string &desc) :
        RuntimeError("NotImplementedError", desc) {
        m_traceback->AddTraceFromHere();
    }

};


#endif
