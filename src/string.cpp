#include "lisk/string.hpp"

#include "lisk/atom.hpp"
#include "lisk/expression.hpp"

lisk::string lisk::to_string(const lisk::symbol &sym)
{
	return sym;
}

const lisk::string &lisk::type_name(const lisk::symbol &)
{
	const static lisk::string name = "symbol";
	return name;
}

lisk::string lisk::to_string(const lisk::string &str)
{
	auto result  = str;
	auto replace = [](lisk::string &str, const char c, const char *cstr)
	{
		for (auto i = str.find(c); i != lisk::string::npos; i = str.find(c))
			str.replace(i, 1, cstr);
	};
	replace(result, '\n', "\\n");
	replace(result, '\r', "\\r");
	replace(result, '\t', "\\t");
	replace(result, '\0', "\\0");
	replace(result, '\"', "\\\"");

	return "\"" + result + "\"";
}

const lisk::string &lisk::type_name(const lisk::string &)
{
	const static lisk::string name = "string";
	return name;
}

bool operator>>(const lisk::expression &arg, lisk::symbol &out)
{
	if_let_ok (const auto &atom, arg.get_atom())
	{
		if_let_ok (const auto &sym, atom.get_symbol())
		{
			out = sym;
			return true;
		}
	}
	return false;
}

bool operator>>(const lisk::expression &arg, lisk::string &out)
{
	if_let_ok (const auto &atom, arg.get_atom())
	{
		if_let_ok (const auto &str, atom.get_string())
		{
			out = str;
			return true;
		}
	}
	return false;
}