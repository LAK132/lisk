#include "lisk/callable.hpp"

#include "lisk/expression.hpp"
#include "lisk/functor.hpp"
#include "lisk/lambda.hpp"

namespace lisk
{
	callable::callable(const lambda &l)
	{
		_value.emplace<std::shared_ptr<lambda>>(std::make_shared<lambda>(l));
	}

	callable::callable(const functor &f)
	{
		_value.emplace<std::shared_ptr<functor>>(std::make_shared<functor>(f));
	}

	callable &callable::operator=(const lambda &l)
	{
		_value.emplace<std::shared_ptr<lambda>>(std::make_shared<lambda>(l));
		return *this;
	}

	callable &callable::operator=(const functor &f)
	{
		_value.emplace<std::shared_ptr<functor>>(std::make_shared<functor>(f));
		return *this;
	}

	bool callable::is_null() const { return !is_lambda() && !is_functor(); }

	bool callable::is_lambda() const { return get_lambda() != nullptr; }

	bool callable::is_functor() const { return get_functor() != nullptr; }

	const lambda *callable::get_lambda() const
	{
		if (std::holds_alternative<std::shared_ptr<lambda>>(_value))
			return std::get<std::shared_ptr<lambda>>(_value).get();
		else
			return nullptr;
	}

	lambda *callable::get_lambda()
	{
		if (std::holds_alternative<std::shared_ptr<lambda>>(_value))
			return std::get<std::shared_ptr<lambda>>(_value).get();
		else
			return nullptr;
	}

	functor *callable::get_functor() const
	{
		if (std::holds_alternative<std::shared_ptr<functor>>(_value))
			return std::get<std::shared_ptr<functor>>(_value).get();
		else
			return nullptr;
	}

	const lambda &callable::as_lambda() const
	{
		return *std::get<std::shared_ptr<lambda>>(_value);
	}

	lambda &callable::as_lambda()
	{
		return *std::get<std::shared_ptr<lambda>>(_value);
	}

	functor callable::as_functor() const
	{
		return *std::get<std::shared_ptr<functor>>(_value);
	}

	std::pair<expression, size_t> callable::operator()(
	  shared_list l, environment &e, bool allow_tail_eval) const
	{
		if (is_null()) return {expression::null{}, 0};

		std::pair<expression, size_t> result = std::visit(
		  [&](auto &&func) { return (*func)(l, e, allow_tail_eval); }, _value);

		if (allow_tail_eval && result.first.is_eval_list())
			result.first = eval(result.first, e, allow_tail_eval);

		return result;
	}

	string to_string(const callable &c)
	{
		return std::visit(
		  [](auto &&func)
		  { return func ? to_string(*func) : to_string(expression::null{}); },
		  c._value);
	}

	const string &type_name(const callable &)
	{
		const static string name = "callable";
		return name;
	}
}

bool operator>>(const lisk::expression &arg, lisk::callable &out)
{
	if (!arg.is_callable()) return false;

	out = arg.as_callable();
	return true;
}
