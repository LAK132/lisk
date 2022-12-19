#ifndef LISK_CALLABLE_HPP
#	define LISK_CALLABLE_HPP

#	define LISK_ATOM_FORWARD_ONLY
#	include "lisk/atom.hpp"

#	define LISK_SHARED_LIST_FORWARD_ONLY
#	include "lisk/shared_list.hpp"

#	include "lisk/functor.hpp"

#	include <lak/memory.hpp>
#	include <lak/tuple.hpp>
#	include <lak/variant.hpp>

namespace lisk
{
	struct expression;
	struct environment;
	struct lambda;

	struct callable
	{
		using lambda_ptr = lak::shared_ptr<lisk::lambda>;
		using value_type = lak::variant<lambda_ptr, lisk::functor>;
		value_type _value;

		// :TODO: add a way to track the number of arguments that a callable will
		// take, and if it's variadic then make it return how many it read.

		callable()                  = default;
		callable(const callable &c) = default;
		callable(callable &&c)      = default;

		callable &operator=(const callable &c) = default;
		callable &operator=(callable &&c) = default;

		inline callable(const lisk::lambda &l);
		inline callable(const lisk::functor &f);

		inline callable &operator=(const lisk::lambda &l);
		inline callable &operator=(const lisk::functor &f);

		inline bool is_null() const;
		inline bool is_lambda() const;
		inline bool is_functor() const;

		inline bool empty() const { return is_null(); }
		inline operator bool() const { return !is_null(); }

		inline lak::result<lisk::lambda &> get_lambda() &;
		inline lak::result<const lisk::lambda &> get_lambda() const &;
		inline lak::result<lisk::lambda> get_lambda() &&;

		inline lak::result<const lisk::functor &> get_functor() const;

		lak::pair<lisk::expression, size_t> operator()(
		  lisk::basic_shared_list<lisk::expression> l,
		  lisk::environment &e,
		  bool allow_tail_eval) const;
	};

	lisk::string to_string(const lisk::callable &c);
	const lisk::string &type_name(const lisk::callable &);
}

bool operator>>(const lisk::expression &arg, lisk::callable &out);

#	define LISK_CALLABLE_HPP_FINISHED
#endif

#ifdef LISK_CALLABLE_FORWARD_ONLY
#	undef LISK_CALLABLE_FORWARD_ONLY
#else
#	if defined(LISK_CALLABLE_HPP_FINISHED) && !defined(LISK_CALLABLE_HPP_IMPL)
#		define LISK_CALLABLE_HPP_IMPL
#		include "callable.inl"
#	endif
#endif
