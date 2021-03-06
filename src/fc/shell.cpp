#include "shell.h"

#include <span>
#include <variant>

#include "builtinnames.h"
#include "ul/usual.h"
#include "util/arena.h"

namespace forrest {

using std::get_if;

struct Lambda
{
    vector<const char*> pars;
    Node* body;
};

using std::array;
using std::unique_ptr;
using std::visit;

maybe<Lambda> try_get_fn_expr(Node* e)
{
    // e must be ApplyNode.
    auto apply_node = e->try_cast<tag::Apply>();
    if (!apply_node)
        return {};
    // Lambda must be the builtin FN SymLeaf.
    auto symleaf = apply_node->lambda->try_cast<tag::Sym>();
    if (!symleaf)
        return {};
    if (symleaf != BuiltinNames::g->id_to_symleaf(BuiltinNames::FN)) {
        return {};
    }
    // xs should hold args and body
    auto& xs = apply_node->args->xs;
    if (xs.size() != 2)
        return {};
    auto args = xs[0]->try_cast<tag::Tuple>();
    if (!args)
        return {};
    // args should be list of symbols
    vector<const char*> lambda_args;
    lambda_args.reserve(args->xs.size());
    for (auto& arg : args->xs) {
        auto p_arg = arg->try_cast<tag::Sym>();
        if (!p_arg)
            return {};
        lambda_args.emplace_back(p_arg->name.c_str());
    }
    return Lambda{move(lambda_args), xs[1]};
};

maybe<Node*> Shell::resolveSymbol(const string& name)
{
    auto m_id = BuiltinNames::g->string_to_id(name);
    if (m_id) {
        return BuiltinNames::g->id_to_symleaf(*m_id);
    }
    auto it = symbols.find(name);
    if (it == symbols.end())
        return {};
    return it->second;
}

Shell::EvalResult Shell::eval(Node* expr)
{
    Arena tmp_storage;
    return eval(expr, tmp_storage);
}

Shell::EvalResult beta_reduction(const Lambda& lambda, const vector<Node*> evald_args)
{
    // todo
}

Shell::EvalResult Shell::eval_fn(const vector<Node*>& evald_args)
{
    // @1 must be list of symbols
    // @2 must be body
}

Shell::EvalResult Shell::eval_def(const vector<Node*>& evald_args)
{
    // @1 must be unset symbol
    if (evald_args.size() != 2) {
        return EvalError{"def needs 2 args"};
    }
    auto sym = evald_args[0]->try_cast<tag::Sym>();
    if (!sym) {
        return EvalError{"def first arg must be sym"};
    }
    if (symbols.find(sym->name) != symbols.end()) {
        return EvalError{"def first arg must be unbound sym"};
    }
    // @2 must be value to set
    symbols[sym->name] = evald_args[1];
    return sym;
}

Shell::EvalResult Shell::eval(Node* expr, Arena& storage)
{
    struct Visitor
    {
        Shell& shell;
        Arena& storage;
        explicit Visitor(Shell& shell, Arena& storage) : shell(shell), storage(storage) {}

        EvalResult operator()(TupleNode* p)
        {
            vector<Node*> evald_xs;
            for (auto x : p->xs) {
                auto er = visit(*this, x->thisv());
                if (is_left(er))
                    return left(er);
                evald_xs.emplace_back(right(er));
            }
            // Here we have an evaluated tuple
            return storage.new_<TupleNode>(BE(evald_xs));
        }
        EvalResult operator()(StrNode* p) { return p; }
        EvalResult operator()(SymLeaf* p)
        {
            auto me = shell.resolveSymbol(p->name);
            if (me) {
                return *me;
            }
            return EvalError{string("Unknown symbol: ") + p->name};
        }
        EvalResult operator()(NumLeaf* p) { return p; }
        EvalResult operator()(CharLeaf* p) { return p; }
        EvalResult operator()(ApplyNode* p)
        {
            vector<Node*> evald_args;
            auto args_xs = p->args->xs;
            evald_args.reserve(args_xs.size());
            for (auto e : p->args->xs) {
                auto er = visit(*this, e->thisv());
                if (is_left(er)) {
                    return er;
                }
                evald_args.emplace_back(right(er));
            }

            auto er_lambda = visit(*this, p->lambda->thisv());
            if (is_left(er_lambda))
                return er_lambda;
            // Check if builtin.
            auto evald_lambda = right(er_lambda);
            auto symleaf = evald_lambda->try_cast<tag::Sym>();
            if (symleaf) {
                if (symleaf == BuiltinNames::g->id_to_symleaf(BuiltinNames::FN)) {
                    return shell.eval_fn(evald_args);
                } else if (symleaf == BuiltinNames::g->id_to_symleaf(BuiltinNames::DEF)) {
                    return shell.eval_def(evald_args);
                }
            }
            auto m_lambda = try_get_fn_expr(evald_lambda);
            if (!m_lambda) {
                return EvalError{"First element is not a lambda."};
            };

            // Bind evald_args to lambda.pars in lambda.body.
            // The can be less args then pars, then it's currying.
            return beta_reduction(*m_lambda, evald_args);
        }
        EvalResult operator()(QuoteNode* p) { return p->expr; }
    };
    return visit(Visitor{*this, storage}, expr->thisv());
}

}  // namespace forrest