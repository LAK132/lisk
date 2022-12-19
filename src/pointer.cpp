#include "lisk/pointer.hpp"

#include "lisk/expression.hpp"

lisk::string lisk::to_string(const lisk::pointer &)
{
	// :TODO: actually return the value.
	return "<POINTER>";
}

const lisk::string &lisk::type_name(const lisk::pointer &)
{
	const static lisk::string name = "pointer";
	return name;
}

bool operator>>(const lisk::expression &arg, lisk::pointer &out)
{
	if_let_ok (const auto &atom, arg.get_atom())
	{
		if_let_ok (const auto &ptr, atom.get_pointer())
		{
			out = ptr;
			return true;
		}
	}
	return false;
}
