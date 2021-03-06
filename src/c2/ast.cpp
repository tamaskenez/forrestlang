#include "ast.h"

#include "absl/strings/str_format.h"
#include "ast_syntax.h"
#include "ul/usual.h"
#include "util/utf.h"

namespace forrest {

using absl::PrintF;
using absl::StrFormat;
using namespace ul;

void dump(ast::Expr* expr)
{
    struct Visitor
    {
        string ind;
        void indent() { ind += " "; }
        void dedent() { ind.pop_back(); }
        void operator()(const ast::List& x)
        {
            char oc = x.fnapp ? '(' : '[';
            char cc = x.fnapp ? ')' : ']';
            if (x.xs.empty()) {
                PrintF("%s%c%c\n", ind, oc, cc);
            } else {
                PrintF("%s%c\n", ind, oc);
                indent();
                for (auto expr_ptr : x.xs) {
                    visit(*this, *expr_ptr);
                }
                dedent();
                PrintF("%s%c\n", ind, cc);
            }
        }
        void operator()(const ast::Token& x)
        {
            PrintF("%s", ind);
            switch (x.kind) {
                case ast::Token::STRING:
                    break;
                case ast::Token::QUOTED_STRING:
                    PrintF("<QUOTED>: ");
                    break;
                case ast::Token::NUMBER:
                    PrintF("<NUMBER>: ");
                    break;
                default:
                    UL_UNREACHABLE;
            }
            string s;
            for (auto c : x.x) {
                if (is_ascii_utf8_byte(c) && (c == ' ' || isgraph(c))) {
                    s += c;
                } else {
                    s += StrFormat("\\U+%02X;", c);
                }
            }
            PrintF("%s\n", s);
        }
    };

    visit(Visitor{}, *expr);
}

}  // namespace forrest
