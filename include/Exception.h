#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <Constant.h>
#include <Traceback.h>

class Exception : public Constant {
public:

    /** Let the traceback function access the traceback. */
    friend class BeginCatch_GetTraceback;

    Exception(const std::string &name, const std::string &desc) :
        m_name(name), m_desc(desc), m_traceback(new Traceback()) {
    }

    const std::string &GetDesc() {
        return m_desc;
    }
    void SetDesc(std::string &d) {
        m_desc = d;
    }
    const char *what() {
        return m_desc.c_str();
    }

    void SetTraceback(Traceback *t) {
        m_traceback = t;
    }
    Traceback *GetTraceback() {
        return m_traceback;
    }

    virtual const std::string Repr();
    virtual Object *__Repr__();
    
protected:
    std::string m_name;
    std::string m_desc;
    Traceback *m_traceback;
};

#endif
