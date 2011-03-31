#include <readline/readline.h>
#include <readline/history.h>

#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cassert>

std::string get_statement() {
    /* Rebind TAB to insert itself instead of completing. */
    rl_bind_key('\t', rl_insert);

    char *line = readline(">>> ");

    if(!line) {
        exit(0);
    }

    if(*line) {
        add_history(line);
    }

    /* Scan the line for ending with a colon.
       
       If the line ends with a colon then optional whitespace, enter continuation mode. */
    char last_non_whitespace = '\0';
    for(int i = 0; i < strlen(line); i++) {
        if(!isspace(line[i])) {
            last_non_whitespace = line[i];
        }
    }
    if(last_non_whitespace != ':') {
        std::string s(line);
        free(line);
        return s;
    }

    std::stringstream ss;
    ss << line;
    free(line);

    /* Continuation mode. */
    do {
        line = readline("... ");
        if(!line) {
            exit(0);
        }

        if(strlen(line) == 0) {
            free(line);
            return ss.str();
        }

        ss << '\n' << line;
    } while(1);
}

extern "C" char *tempnam(const char *a, const char *b) {
    assert(0 && "tempnam overridden but not implemented (repl.cc)");
    abort();
}
extern "C" char *tmpnam_r(char *a) {
    assert(0 && "tempnam overridden but not implemented (repl.cc)");
    abort();
}
