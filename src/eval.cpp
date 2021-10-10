#include "lisk/eval.hpp"

#include "lisk/functor.hpp"
#include "lisk/lambda.hpp"

namespace lisk
{
	std::pair<shared_list, size_t> eval_all(shared_list l,
	                                        environment &e,
	                                        bool allow_tail_eval)
	{
		auto result  = shared_list::create();
		auto end     = result;
		size_t count = 0;
		for (const auto &node : l)
		{
			++count;
			end.set_next(shared_list::create());
			++end;
			end.value() = eval(node.value, e, allow_tail_eval);
		}
		return {++result, count};
	}

	expression eval(const expression &exp, environment &e, bool allow_tail_eval)
	{
		if (exp.is_null())
		{
			// Expr was the empty list, which evaluates to nil.
			return atom::nil{};
		}
		else if (exp.is_eval_list())
		{
			if (!allow_tail_eval) return exp;

			auto result = exp;

			for (callable c;
			     result.is_eval_list() && result.as_eval_list().list.value() >> c;
			     result = eval(c({}, e, false).first, e, false))
				;

			return result;
		}
		else if (symbol sym; exp >> sym)
		{
			return e[sym];
		}
		else if (atom a; exp >> a)
		{
			return a;
		}
		else if (shared_list l; exp >> l)
		{
			// If we're about do do a function call, this should evalutate the symbol
			// to the relevant function pointer.
			auto subexp = eval(l.value(), e, allow_tail_eval);

			// This is the empty list or nil atom.
			if ((subexp.is_list() && subexp.as_list().value().is_null()) ||
			    (subexp.is_atom() && subexp.as_atom().is_nil()))
			{
				return atom::nil{};
			}
			else if (subexp >> sym)
			{
				return e[sym];
			}
			else if (subexp >> a)
			{
				return a;
			}
			else if (shared_list l2; subexp >> l2)
			{
				// :TODO: Comment on why we would ever end up here?
				return l2;
			}
			else if (callable c; subexp >> c)
			{
				return c(l.next(), e, allow_tail_eval).first;
			}
			else if (exception exc; subexp >> exc)
			{
				return exc;
			}
			else
			{
				return exception{"Failed to eval sub-expression '" +
				                 to_string(l.value()) + "' of '" + to_string(exp) +
				                 "', got '" + to_string(subexp) +
				                 "', expected a symbol, atom or callable"};
			}
		}
		else if (exception exc; exp >> exc)
		{
			return exc;
		}
		else
		{
			return exception{"Failed to eval expression '" + to_string(exp) +
			                 "' type '" +
			                 exp.visit([](auto &&a) { return type_name(a); }) + "'"};
		}
	}
}
