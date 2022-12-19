#include "lisk/functor.hpp"

#include "lisk/eval.hpp"
#include "lisk/expression.hpp"

lisk::string lisk::to_string(lisk::functor f)
{
	return "<builtin " +
	       std::to_string(reinterpret_cast<uintptr_t>(static_cast<void *>(f))) +
	       ">";
}

const lisk::string &lisk::type_name(const lisk::functor &)
{
	const static lisk::string name = "functor";
	return name;
}

bool operator>>(const lisk::expression &arg, lisk::functor &out)
{
	if_let_ok (const auto &callable, arg.get_callable())
	{
		if_let_ok (const auto &func, callable.get_functor())
		{
			out = func;
			return true;
		}
	}
	return false;
}
