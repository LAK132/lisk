#include "lisk/atom.hpp"

#include "lisk/expression.hpp"

lisk::string lisk::to_string(lisk::atom::nil)
{
	return "nil";
}

const lisk::string &lisk::type_name(lisk::atom::nil)
{
	const static lisk::string name = "nil";
	return name;
}

lisk::string lisk::to_string(bool b)
{
	return b ? "true" : "false";
}

const lisk::string &lisk::type_name(bool)
{
	const static lisk::string name = "bool";
	return name;
}

lisk::string lisk::to_string(const lisk::atom &a)
{
	return a.visit([](auto &&a) { return to_string(a); });
}

const lisk::string &lisk::type_name(const lisk::atom &)
{
	const static lisk::string name = "atom";
	return name;
}

bool operator>>(const lisk::expression &arg, lisk::atom::nil)
{
	if_let_ok (const auto &atom, arg.get_atom())
		return atom.is_nil();
	else
		return false;
}

bool operator>>(const lisk::expression &arg, bool &out)
{
	if_let_ok (const auto &list, arg.get_list())
	{
		if (!list._node || (list._node->value.is_null() && !list._node->next))
		{
			// arg was the empty list, which trivially casts to false.
			out = false;
			return true;
		}
		else
			return false;
	}
	else if_let_ok (const auto &atom, arg.get_atom())
	{
		if_let_ok (bool b, atom.get_bool())
		{
			out = b;
			return true;
		}
		else if (atom.is_nil())
		{
			// arg was the nil atom, which trivially casts to false.
			out = false;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool operator>>(const lisk::expression &arg, lisk::atom &out)
{
	if_let_ok (const auto &atom, arg.get_atom())
	{
		out = atom;
		return true;
	}
	else
		return false;
}
