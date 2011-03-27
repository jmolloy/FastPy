#include <Exception.h>
#include <sstream>
#include <Traceback.h>

const std::string Exception::Repr() {
    std::stringstream ss;
    ss << "Traceback (most recent call last):\n";

    if(m_traceback) {
        ss << m_traceback->Repr();
    }

    ss << m_name << ": " << m_desc;
    return ss.str();
}
