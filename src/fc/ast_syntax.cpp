#include "ast_syntax.h"

namespace forrest {

static const char SPECIAL_CHARS[] = {OPEN_TUPLE_CHAR,
                                     CLOSE_TUPLE_CHAR,
                                     STRING_QUOTE_CHAR,
                                     STRING_ESCAPE_CHAR,
                                     OPEN_APPLY_CHAR,
                                     CLOSE_APPLY_CHAR,
                                     0};

bool is_symbol_char(Utf8Char x)
{
    auto p = SPECIAL_CHARS;
    do {
        if (*p == x.front())
            return false;
    } while (*(p++) != 0);
    return !iscntrl(x.front()) && !isspace(x.front());
}

}  // namespace forrest
