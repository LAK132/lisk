#ifndef LISK_ENVIRONMENT_HPP
#define LISK_ENVIRONMENT_HPP

#define LISK_EXPRESSION_FORWARD_ONLY
#include "lisk/expression.hpp"

#define LISK_SHARED_LIST_FORWARD_ONLY
#include "lisk/shared_list.hpp"

#include <unordered_map>

namespace lisk
{
	struct callable;

	struct environment
	{
		using value_type = lisk::basic_shared_list<
		  std::unordered_map<lisk::symbol, lisk::expression>>;
		value_type _map = {};

		environment()                    = default;
		environment(const environment &) = default;
		environment(environment &&)      = default;

		environment &operator=(const environment &) = default;
		environment &operator=(environment &&) = default;

		static environment extends(const environment &other);

		void define_expr(const lisk::symbol &sym, const lisk::expression &expr);
		void define_atom(const lisk::symbol &sym, const lisk::atom &a);
		void define_list(const lisk::symbol &sym, const lisk::shared_list &list);
		void define_callable(const lisk::symbol &sym, const lisk::callable &c);
		void define_functor(const lisk::symbol &sym, const lisk::functor &f);

		lisk::expression operator[](const lisk::symbol &sym) const;

		environment clone(size_t depth = 0) const;
		environment &squash(size_t depth);
	};

	lisk::string to_string(const lisk::environment &env);
}

#endif
