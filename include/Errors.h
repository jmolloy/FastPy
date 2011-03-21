/**@file   Errors.h
 * @author James Molloy <james.molloy@arm.com>
 * @date   Sat Dec  4 13:21:27 2010
 * @brief  Defines the general "Error" class used for passing errors around. */

#ifndef ERRORS_H
#define ERRORS_H

#include <string>
#include <iostream>

#include <stdlib.h>

class Errors {
public:
    Errors() :
        m_error(false), m_descr() {
    }

    /** Report an error - this does not get printed, just stored in the object. */
    void Error(std::string descr) {
        m_error = true;
        m_descr = descr;
    }

    /** Return true if an error has occurred. */
    bool WasError() {
        return m_error;
    }

    /** Print error to stderr and exit. */
    void Garble() {
        std::cerr << "Error: " << m_descr << std::endl;
        exit(1);
    }

private:
    bool        m_error;
    std::string m_descr;
};

#endif
