#pragma once

#include <unordered_map>

#include "ul/maybe.h"

#include "util/arena.h"
#include "util/either.h"

#include "ast.h"

namespace forrest {

using std::unordered_map;

struct Shell
{
    struct EvalErroriik maybe<Node*> resolveSymbol(const string& name);
    EvalResult eval(Node* expr);
    EvalResult eval(Node* expr, Arena& storage);

    unordered_map<string, Node*> symbols;

private:
    EvalResult eval_fn(const vector<Node*>& evald_args);
    EvalResult eval_def(const vector<Node*>& evald_args);
};

}  // namespace forrest
