#include "lisk/environment.hpp"

namespace lisk
{
	environment environment::extends(const environment &other)
	{
		environment result;
		result._map = value_type::extends(other._map);
		return result;
	}

	void environment::define_expr(const symbol &sym, const expression &expr)
	{
		_map.value()[sym] = expr;
	}

	void environment::define_atom(const symbol &sym, const atom &a)
	{
		_map.value()[sym] = a;
	}

	void environment::define_list(const symbol &sym, const shared_list &list)
	{
		_map.value()[sym] = list;
	}

	void environment::define_callable(const symbol &sym, const callable &c)
	{
		_map.value()[sym] = c;
	}

	void environment::define_functor(const symbol &sym, const functor &f)
	{
		define_callable(sym, f);
	}

	expression environment::operator[](const symbol &sym) const
	{
		for (const auto &node : _map)
			if (const auto it = node.value.find(sym); it != node.value.end())
				return it->second;

		return exception{"Environment lookup failed, couldn't find '" + sym +
		                 "' in '" + to_string(*this) + "'"};
	}

	environment environment::clone(size_t depth) const
	{
		environment result;
		result._map = _map.clone(depth);
		return result;
	}

	environment &environment::squash(size_t depth)
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

	string to_string(const environment &env)
	{
		string result;
		result += "(";
		for (const auto &node : env._map)
			for (const auto &[key, value] : node.value)
				result += "(" + to_string(key) + " " + to_string(value) + ") ";
		if (result.back() == ' ') result.pop_back();
		result += ")";
		return result;
	}
}
