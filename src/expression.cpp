#include "lisk/expression.hpp"

#include "lisk/eval.hpp"

const lisk::string &lisk::type_name(const lisk::shared_list &)
{
	const static lisk::string name = "<EXPR LIST>";
	return name;
}

lisk::string lisk::to_string(const lisk::eval_shared_list &list)
{
	return "<EVAL " + to_string((const lisk::shared_list &)list) + ">";
}

const lisk::string &lisk::type_name(const lisk::eval_shared_list &)
{
	const static lisk::string name = "<EVAL LIST>";
	return name;
}

lisk::string lisk::to_string(const lisk::uneval_shared_list &list)
{
	return "<UNEVAL " + to_string((const lisk::shared_list &)list) + ">";
}

const lisk::string &lisk::type_name(const lisk::uneval_shared_list &)
{
	const static lisk::string name = "<UNEVAL LIST>";
	return name;
}

lisk::string lisk::to_string(const lisk::exception &exc)
{
	return "<EXCEPTION '" + exc.message + "'>";
}

const lisk::string &lisk::type_name(const lisk::exception &)
{
	const static lisk::string name = "<EXCEPTION>";
	return name;
}

lisk::string lisk::to_string(const lisk::expression &expr)
{
	return expr.visit([](auto &&a) { return to_string(a); });
}

const lisk::string &lisk::type_name(const lisk::expression &)
{
	const static lisk::string name = "expression";
	return name;
}

lisk::string lisk::to_string(lisk::expression::null)
{
	return "null";
}

const lisk::string &lisk::type_name(lisk::expression::null)
{
	const static lisk::string name = "null";
	return name;
}

const lisk::string &lisk::type_name(const lisk::eval_expr &)
{
	const static lisk::string name = "<EVAL EXPRESSION>";
	return name;
}

const lisk::string &lisk::type_name(const lisk::uneval_expr &)
{
	const static lisk::string name = "<UNEVAL EXPRESSION>";
	return name;
}

bool operator>>(const lisk::expression &arg, lisk::expression &out)
{
	out = arg;
	return true;
}

bool operator>>(const lisk::expression &arg, lisk::exception &out)
{
	if_let_ok (const auto &exc, arg.get_exception())
	{
		out = exc;
		return true;
	}
	else
		return false;
}

bool operator>>(const lisk::expression &arg, lisk::uneval_expr &out)
{
	out = {arg};
	return true;
}

bool operator>>(const lisk::expression &arg, lisk::shared_list &out)
{
	if_let_ok (const auto &list, arg.get_list())
	{
		out = list;
		return true;
	}
	else
		return false;
}

bool operator>>(const lisk::expression &arg, lisk::eval_shared_list &out)
{
	if_let_ok (const auto &list, arg.get_eval_list())
	{
		out = list;
		return true;
	}
	else
		return false;
}
