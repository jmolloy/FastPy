#include <Instruction.h>
#include <sstream>

Instruction::Instruction(const char *mnemonic, int id) :
    m_mnemonic(mnemonic) {
    SetId(id);
}

const std::string Instruction::Repr() {
    std::stringstream ss;
    ss << "    " << RefRepr() << " = " << m_mnemonic << " ";
    bool first = true;
    for(std::vector<Value*>::iterator it = m_args.begin();
        it != m_args.end();
        ++it) {
        if(first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << (*it)->RefRepr();
    }
    return ss.str();
}
