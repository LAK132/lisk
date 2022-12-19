#include "lisk/callable.hpp"

#include "lisk/expression.hpp"
#include "lisk/functor.hpp"
#include "lisk/lambda.hpp"

lak::pair<lisk::expression, size_t> lisk::callable::operator()(
  lisk::shared_list l, lisk::environment &e, bool allow_tail_eval) const
{
	if (is_null()) return {lisk::expression::null{}, 0};

	lak::pair<lisk::expression, size_t> result = lak::visit(
	  [&](auto &&func) { return (*func)(l, e, allow_tail_eval); }, _value);

	if (allow_tail_eval && result.first.is_eval_list())
		result.first = eval(result.first, e, allow_tail_eval);

	return result;
}

lisk::string lisk::to_string(const lisk::callable &c)
{
	return lak::visit(
	  [](auto &&func)
	  { return func ? to_string(*func) : to_string(expression::null{}); },
	  c._value);
}

const lisk::string &lisk::type_name(const lisk::callable &)
{
	const static lisk::string name = "callable";
	return name;
}

bool operator>>(const lisk::expression &arg, lisk::callable &out)
{
	if_let_ok (const auto &callable, arg.get_callable())
	{
		out = callable;
		return true;
	}
	else
		return false;
}
