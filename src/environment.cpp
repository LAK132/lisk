#include "lisk/environment.hpp"

#include "lisk/shared_list.hpp"

lisk::environment lisk::environment::extends(const lisk::environment &other)
{
	lisk::environment result;
	result._map = value_type::extends(other._map);
	return result;
}

void lisk::environment::define_expr(const lisk::symbol &sym,
                                    const lisk::expression &expr)
{
	_map.value()[sym] = expr;
}

void lisk::environment::define_atom(const lisk::symbol &sym,
                                    const lisk::atom &a)
{
	_map.value()[sym] = a;
}

void lisk::environment::define_list(const lisk::symbol &sym,
                                    const lisk::shared_list &list)
{
	_map.value()[sym] = list;
}

void lisk::environment::define_callable(const lisk::symbol &sym,
                                        const lisk::callable &c)
{
	_map.value()[sym] = c;
}

void lisk::environment::define_functor(const lisk::symbol &sym,
                                       const lisk::functor &f)
{
	define_callable(sym, f);
}

lisk::expression lisk::environment::operator[](const lisk::symbol &sym) const
{
	for (const auto &node : _map)
		if (const auto it = node.value.find(sym); it != node.value.end())
			return it->second;

	return lisk::exception{"Environment lookup failed, couldn't find '" + sym +
	                       "' in '" + to_string(*this) + "'"};
}

lisk::environment lisk::environment::clone(size_t depth) const
{
	lisk::environment result;
	result._map = _map.clone(depth);
	return result;
}

lisk::environment &lisk::environment::squash(size_t depth)
{
	if (depth == 0)
	{
		*this = clone(0);
		while (_map._node->next)
		{
			_map.next_value().merge(_map.value());
			_map++;
		}
	}
	else
	{
		*this = clone(depth + 1);
		while (depth-- > 0 && _map._node->next)
		{
			_map.next_value().merge(_map.value());
			_map++;
		}
	}
	return *this;
}

lisk::string lisk::to_string(const lisk::environment &env)
{
	lisk::string result;
	result += "(";
	for (const auto &node : env._map)
		for (const auto &[key, value] : node.value)
			result += "(" + to_string(key) + " " + to_string(value) + ") ";
	if (result.back() == ' ') result.pop_back();
	result += ")";
	return result;
}
