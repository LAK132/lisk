#include "lisk/string.hpp"

#include "lisk/expression.hpp"

namespace lisk
{
	string to_string(const symbol &sym) { return sym; }

	const string &type_name(const symbol &)
	{
		const static string name = "symbol";
		return name;
	}

	string to_string(const string &str)
	{
		auto result  = str;
		auto replace = [](string &str, const char c, const char *cstr)
		{
			for (auto i = str.find(c); i != string::npos; i = str.find(c))
				str.replace(i, 1, cstr);
		};
		replace(result, '\n', "\\n");
		replace(result, '\r', "\\r");
		replace(result, '\t', "\\t");
		replace(result, '\0', "\\0");
		replace(result, '\"', "\\\"");

		return "\"" + result + "\"";
	}

	const string &type_name(const string &)
	{
		const static string name = "string";
		return name;
	}
}

bool operator>>(const lisk::expression &arg, lisk::symbol &out)
{
	if (!arg.is_atom() || !arg.as_atom().is_symbol()) return false;

	out = arg.as_atom().as_symbol();
	return true;
}

bool operator>>(const lisk::expression &arg, lisk::string &out)
{
	if (!arg.is_atom() || !arg.as_atom().is_string()) return false;

	out = arg.as_atom().as_string();
	return true;
}