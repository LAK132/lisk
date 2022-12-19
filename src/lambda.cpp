#include "lisk/lambda.hpp"

#include "lisk/shared_list.hpp"

lisk::lambda::lambda(lisk::shared_list l,
                     lisk::environment &e,
                     bool allow_tail_eval)
: captured_env(lisk::environment::extends(e))
{
	if (lisk::shared_list arg1, arg2;
	    l.value() >> arg1 && l.next().value() >> arg2)
	{
		params = arg1.clone();

		list_reader reader(params, e, allow_tail_eval);

		size_t param_index = 0;
		for (auto &node : params)
		{
			if (symbol s; reader >> s)
			{
				node.value = s;
				++param_index;
			}
			else
			{
				// :TODO: This really should throw an exception. Maybe this should
				// now evaluate to a lambda that returns an exception on call?
				params.clear();
				return;
			}
		}

		exp = arg2;
	}
}

lak::pair<lisk::expression, size_t> lisk::lambda::operator()(
  lisk::shared_list l, lisk::environment &e, bool allow_tail_eval) const
{
	auto new_env = lisk::environment::extends(captured_env);

	lisk::list_reader reader(params, e, allow_tail_eval);

	size_t param_index = 0;
	for (const auto &node : l)
	{
		auto evaled = reader.list;
		if (!reader)
		{
			if (param_index == 0)
			{
				return {
				  lisk::exception{"Too many arguments to call lambda, expected none"},
				  0};
			}
			else
			{
				return {lisk::exception{
				          "Too many arguments to call lambda, expected params are '" +
				          to_string(params) + "'"},
				        0};
			}
		}
		else if (lisk::symbol s; reader >> s)
		{
			new_env.define_expr(s, lisk::eval(node.value, e, allow_tail_eval));
			++param_index;
		}
		else
		{
			return {lisk::exception{"Failed to get symbol " +
			                        std::to_string(param_index) + " from '" +
			                        to_string(node.value) + "' for '" +
			                        to_string(evaled) + "'"},
			        0};
		}
	}

	if (reader)
	{
		return {lisk::exception{"Too few parameters in '" + to_string(l) +
		                        "' to call lambda, expected parameters are '" +
		                        to_string(params) + "'"},
		        0};
	}

	return {lisk::eval(exp, new_env, allow_tail_eval), param_index};
}

lisk::string lisk::to_string(const lisk::lambda &l)
{
	return "(lambda " + to_string(l.params) + " " + to_string(l.exp) + ")";
}

const lisk::string &type_name(const lisk::lambda &)
{
	const static lisk::string name = "lambda";
	return name;
}

bool operator>>(const lisk::expression &arg, lisk::lambda &out)
{
	if_let_ok (const auto &callable, arg.get_callable())
	{
		if_let_ok (const auto &lambda, callable.get_lambda())
		{
			out = lambda;
			return true;
		}
	}
	return false;
}
