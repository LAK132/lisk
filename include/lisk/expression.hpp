#ifndef LISK_EXPRESSION_HPP
#	define LISK_EXPRESSION_HPP

#	include <lak/result.hpp>
#	include <lak/variant.hpp>

namespace lisk
{
	struct expression;
}

#	define LISK_SHARED_LIST_FORWARD_ONLY
#	include "lisk/shared_list.hpp"

#	include "lisk/string.hpp"

namespace lisk
{
	using shared_list = lisk::basic_shared_list<lisk::expression>;

	struct eval_shared_list
	{
		lisk::shared_list list;
	};
	struct uneval_shared_list
	{
		lisk::shared_list list;
	};

	struct exception
	{
		lisk::string message;
	};
}

#	define LISK_CALLABLE_FORWARD_ONLY
#	include "lisk/callable.hpp"

#	define LISK_ATOM_FORWARD_ONLY
#	include "lisk/atom.hpp"

namespace lisk
{
	struct callable;

	struct expression
	{
		struct null
		{
		};

		using value_type = lak::variant<null,
		                                lisk::atom,
		                                lisk::eval_shared_list,
		                                lisk::shared_list,
		                                lisk::callable,
		                                lisk::exception>;

		value_type _value;

		expression()                       = default;
		expression(const expression &expr) = default;
		expression(expression &&expr)      = default;

		expression &operator=(const expression &expr) = default;
		expression &operator=(expression &&expr) = default;

		inline expression(null);
		inline expression(const lisk::atom &a);
		inline expression(lisk::atom::nil);
		inline expression(const lisk::shared_list &list);
		inline expression(const lisk::eval_shared_list &list);
		inline expression(const lisk::callable &c);
		inline expression(const lisk::exception &exc);

		inline expression &operator=(null);
		inline expression &operator=(const lisk::atom &a);
		inline expression &operator=(lisk::atom::nil);
		inline expression &operator=(const lisk::shared_list &list);
		inline expression &operator=(const lisk::eval_shared_list &list);
		inline expression &operator=(const lisk::callable &c);
		inline expression &operator=(const lisk::exception &exc);

		inline bool is_null() const;
		inline bool is_atom() const;
		inline bool is_list() const;
		inline bool is_eval_list() const;
		inline bool is_callable() const;
		inline bool is_exception() const;

		inline bool empty() const { return is_null() || is_exception(); }
		inline operator bool() const { return !empty(); }

		inline lak::result<lisk::atom &> get_atom() &;
		inline lak::result<const lisk::atom &> get_atom() const &;
		inline lak::result<lisk::atom> get_atom() &&;

		inline lak::result<lisk::shared_list &> get_list() &;
		inline lak::result<const lisk::shared_list &> get_list() const &;
		inline lak::result<lisk::shared_list> get_list() &&;

		inline lak::result<lisk::eval_shared_list &> get_eval_list() &;
		inline lak::result<const lisk::eval_shared_list &> get_eval_list() const &;
		inline lak::result<lisk::eval_shared_list> get_eval_list() &&;

		inline lak::result<lisk::callable &> get_callable() &;
		inline lak::result<const lisk::callable &> get_callable() const &;
		inline lak::result<lisk::callable> get_callable() &&;

		inline lak::result<lisk::exception &> get_exception() &;
		inline lak::result<const lisk::exception &> get_exception() const &;
		inline lak::result<lisk::exception> get_exception() &&;

		template<typename LAMBDA>
		auto visit(LAMBDA &&lambda) const
		{
			return lak::visit(lambda, _value);
		}

		template<typename LAMBDA>
		auto visit(LAMBDA &&lambda)
		{
			return lak::visit(lambda, _value);
		}
	};

	struct eval_expr
	{
		lisk::expression expr;
	};
	struct uneval_expr
	{
		lisk::expression expr;
	};

	const lisk::string &type_name(const lisk::shared_list &);

	lisk::string to_string(const lisk::eval_shared_list &list);
	const lisk::string &type_name(const lisk::eval_shared_list &);

	lisk::string to_string(const lisk::uneval_shared_list &list);
	const lisk::string &type_name(const lisk::uneval_shared_list &);

	lisk::string to_string(const lisk::exception &exc);
	const lisk::string &type_name(const lisk::exception &);

	lisk::string to_string(lisk::expression::null);
	const lisk::string &type_name(lisk::expression::null);

	lisk::string to_string(const lisk::expression &expr);
	const lisk::string &type_name(const lisk::expression &);

	// string to_string(const eval_expr &expr);
	const lisk::string &type_name(const lisk::eval_expr &);

	// string to_string(const uneval_expr &expr);
	const lisk::string &type_name(const lisk::uneval_expr &);
}

bool operator>>(const lisk::expression &arg, lisk::expression &out);
bool operator>>(const lisk::expression &arg, lisk::exception &out);
bool operator>>(const lisk::expression &arg, lisk::uneval_expr &out);
bool operator>>(const lisk::expression &arg, lisk::shared_list &out);
bool operator>>(const lisk::expression &arg, lisk::eval_shared_list &out);

// These are here instead of in pointer.hpp because of include ordering issues.
template<typename T>
bool operator>>(const lisk::expression &arg, T *&out);
template<typename T>
bool operator>>(const lisk::expression &arg, lak::shared_ptr<T> &out);

#	define LISK_EXPRESSION_HPP_FINISHED
#endif

#ifdef LISK_EXPRESSION_FORWARD_ONLY
#	undef LISK_EXPRESSION_FORWARD_ONLY
#else
#	if defined(LISK_EXPRESSION_HPP_FINISHED) &&                                \
	  !defined(LISK_EXPRESSION_HPP_IMPL)
#		define LISK_EXPRESSION_HPP_IMPL
#		include "expression.inl"
#	endif
#endif
