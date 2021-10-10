#include "lisk/functor.hpp"
#include "lisk/eval.hpp"

namespace lisk
{
	string to_string(functor f)
	{
		return "<builtin " +
		       std::to_string(
		         reinterpret_cast<uintptr_t>(static_cast<void *>(f))) +
		       ">";
	}

	const string &type_name(const functor &)
	{
		const static string name = "functor";
		return name;
	}
}

bool operator>>(const lisk::expression &arg, lisk::functor &out)
{
	if (!arg.is_callable() || !arg.as_callable().is_functor()) return false;

	out = arg.as_callable().as_functor();
	return true;
}
