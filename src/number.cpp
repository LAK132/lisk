#include "lisk/number.hpp"

#include "lisk/atom.hpp"
#include "lisk/expression.hpp"

#include <lak/variant.hpp>

lisk::string lisk::to_string(const lisk::number &num)
{
	return lak::visit([](auto &&v) { return to_string(v); }, num._value);
}

const lisk::string &lisk::type_name(const lisk::number &)
{
	const static lisk::string name = "number";
	return name;
}

lisk::string lisk::to_string(lisk::uint_t num)
{
	return std::to_string(num);
}

const lisk::string &lisk::type_name(lisk::uint_t)
{
	const static lisk::string name = "uint";
	return name;
}

lisk::string lisk::to_string(lisk::sint_t num)
{
	return (num >= 0 ? "+" : "") + std::to_string(num);
}

const lisk::string &lisk::type_name(lisk::sint_t)
{
	const static lisk::string name = "sint";
	return name;
}

lisk::string lisk::to_string(lisk::real_t num)
{
	return (num >= 0.0 ? "+" : "") + std::to_string(num);
}

const lisk::string &lisk::type_name(lisk::real_t)
{
	const static lisk::string name = "real";
	return name;
}

bool operator>>(const lisk::expression &arg, lisk::number &out)
{
	if_let_ok (const auto &atom, arg.get_atom())
	{
		if_let_ok (const auto &num, atom.get_number())
		{
			out = num;
			return true;
		}
	}
	return false;
}

bool operator>>(const lisk::expression &arg, lisk::uint_t &out)
{
	if_let_ok (const auto &atom, arg.get_atom())
	{
		if_let_ok (const auto &num, atom.get_number())
		{
			if_let_ok (const auto &n, num.get_uint())
			{
				out = n;
				return true;
			}
		}
	}
	return false;
}

bool operator>>(const lisk::expression &arg, lisk::sint_t &out)
{
	if_let_ok (const auto &atom, arg.get_atom())
	{
		if_let_ok (const auto &num, atom.get_number())
		{
			if_let_ok (const auto &n, num.get_sint())
			{
				out = n;
				return true;
			}
		}
	}
	return false;
}

bool operator>>(const lisk::expression &arg, lisk::real_t &out)
{
	if_let_ok (const auto &atom, arg.get_atom())
	{
		if_let_ok (const auto &num, atom.get_number())
		{
			if_let_ok (const auto &n, num.get_real())
			{
				out = n;
				return true;
			}
		}
	}
	return false;
}
