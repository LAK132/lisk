#ifndef LISK_ENVIRONMENT_HPP
#define LISK_ENVIRONMENT_HPP

#include "callable.hpp"
#include "expression.hpp"
#include "shared_list.hpp"

#include <unordered_map>

namespace lisk
{
	struct environment
	{
		using value_type =
		  basic_shared_list<std::unordered_map<symbol, expression>>;
		value_type _map = {};

		environment()                    = default;
		environment(const environment &) = default;
		environment(environment &&)      = default;

		environment &operator=(const environment &) = default;
		environment &operator=(environment &&) = default;

		static environment extends(const environment &other);

		void define_expr(const symbol &sym, const expression &expr);
		void define_atom(const symbol &sym, const atom &a);
		void define_list(const symbol &sym, const shared_list &list);
		void define_callable(const symbol &sym, const callable &c);
		void define_functor(const symbol &sym, const functor &f);

		expression operator[](const symbol &sym) const;

		environment clone(size_t depth = 0) const;
		environment &squash(size_t depth);
	};

	string to_string(const environment &env);
}

#endif
