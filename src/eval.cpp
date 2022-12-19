#include "lisk/eval.hpp"

#include "lisk/atom.hpp"
#include "lisk/expression.hpp"
#include "lisk/functor.hpp"
#include "lisk/lambda.hpp"

lak::pair<lisk::shared_list, size_t> lisk::eval_all(lisk::shared_list l,
                                                    lisk::environment &e,
                                                    bool allow_tail_eval)
{
	auto result  = lisk::shared_list::create();
	auto end     = result;
	size_t count = 0;
	for (const auto &node : l)
	{
		++count;
		end.set_next(lisk::shared_list::create());
		++end;
		end.value() = lisk::eval(node.value, e, allow_tail_eval);
	}
	return {++result, count};
}

lisk::expression lisk::eval(const lisk::expression &exp,
                            lisk::environment &e,
                            bool allow_tail_eval)
{
	if (exp.is_null())
	{
		// Expr was the empty list, which evaluates to nil.
		return lisk::atom::nil{};
	}
	else if (exp.is_eval_list())
	{
		if (!allow_tail_eval) return exp;

		auto result = exp;

		for (lisk::callable c; result.get_eval_list().map_or(
		       [&](const auto &el) { return el >> c; }, false);
		     result = lisk::eval(c({}, e, false).first, e, false))
			;

		return result;
	}
	else if_let_ok (lisk::atom a, exp.get_atom())
	{
		if_let_ok (lisk::symbol sym, a.get_symbol())
			return e[sym];
		else
			return a;
	}
	else if_let_ok (lisk::shared_list l, exp.get_list())
	{
		// If we're about do do a function call, this should evalutate the symbol
		// to the relevant function pointer.
		auto subexp = lisk::eval(l.value(), e, allow_tail_eval);

		// This is the empty list or nil atom.
		if (subexp.get_list().map_or([](auto &&l) { return l.value().is_null(); },
		                             false) ||
		    subexp.get_atom().map_or([](auto &&a) { return a.is_nil(); }, false))
		{
			return lisk::atom::nil{};
		}
		else if_let_ok (lisk::atom a, subexp.get_atom())
		{
			if_let_ok (lisk::symbol sym, a.get_symbol())
				return e[sym];
			else
				return a;
		}
		else if_let_ok (lisk::shared_list l2, subexp.get_list())
		{
			// :TODO: Comment on why we would ever end up here?
			return l2;
		}
		else if_let_ok (lisk::callable c, subexp.get_callable())
		{
			return c(l.next(), e, allow_tail_eval).first;
		}
		else if_let_ok (lisk::exception exc, subexp.get_exception())
		{
			return exc;
		}
		else
		{
			return lisk::exception{"Failed to eval sub-expression '" +
			                       to_string(l.value()) + "' of '" + to_string(exp) +
			                       "', got '" + to_string(subexp) +
			                       "', expected a symbol, atom or callable"};
		}
	}
	else if_let_ok (lisk::exception exc, exp.get_exception())
	{
		return exc;
	}
	else
	{
		return lisk::exception{
		  "Failed to eval expression '" + to_string(exp) + "' type '" +
		  exp.visit([](auto &&a) { return type_name(a); }) + "'"};
	}
}
