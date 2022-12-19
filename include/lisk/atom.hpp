#ifndef LISK_ATOM_HPP
#	define LISK_ATOM_HPP

#	define LISK_NUMBER_FORWARD_ONLY
#	include "lisk/number.hpp"

#	define LISK_POINTER_FORWARD_ONLY
#	include "lisk/pointer.hpp"

#	include "lisk/string.hpp"

#	include <lak/result.hpp>
#	include <lak/variant.hpp>

namespace lisk
{
	struct atom
	{
		struct nil
		{
		};

		using value_type = lak::variant<nil,
		                                lisk::symbol,
		                                lisk::string,
		                                lisk::number,
		                                bool,
		                                lisk::pointer>;
		value_type _value;

		atom()              = default;
		atom(const atom &a) = default;
		atom(atom &&a)      = default;

		atom &operator=(const atom &expr) = default;
		atom &operator=(atom &&expr) = default;

		inline atom(nil);
		inline atom(const lisk::symbol &sym);
		inline atom(const lisk::string &str);
		inline atom(const lisk::number &num);
		inline atom(bool b);
		inline atom(const lisk::pointer &ptr);

		inline atom &operator=(nil);
		inline atom &operator=(const lisk::symbol &sym);
		inline atom &operator=(const lisk::string &str);
		inline atom &operator=(const lisk::number &num);
		inline atom &operator=(bool b);
		inline atom &operator=(const lisk::pointer &ptr);

		inline bool is_nil() const;
		inline bool is_symbol() const;
		inline bool is_string() const;
		inline bool is_number() const;
		inline bool is_bool() const;
		inline bool is_pointer() const;

		inline lak::result<lisk::symbol &> get_symbol() &;
		inline lak::result<const lisk::symbol &> get_symbol() const &;
		inline lak::result<lisk::symbol> get_symbol() &&;

		inline lak::result<lisk::string &> get_string() &;
		inline lak::result<const lisk::string &> get_string() const &;
		inline lak::result<lisk::string> get_string() &&;

		inline lak::result<lisk::number &> get_number() &;
		inline lak::result<const lisk::number &> get_number() const &;
		inline lak::result<lisk::number> get_number() &&;

		inline lak::result<bool &> get_bool() &;
		inline lak::result<const bool &> get_bool() const &;
		inline lak::result<bool> get_bool() &&;

		inline lak::result<lisk::pointer &> get_pointer() &;
		inline lak::result<const lisk::pointer &> get_pointer() const &;
		inline lak::result<lisk::pointer> get_pointer() &&;

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

	lisk::string to_string(lisk::atom::nil);
	const string &type_name(lisk::atom::nil);

	lisk::string to_string(bool b);
	const lisk::string &type_name(bool);

	lisk::string to_string(const lisk::atom &a);
	const lisk::string &type_name(const lisk::atom &);

	struct expression;
}

bool operator>>(const lisk::expression &arg, lisk::atom::nil);
bool operator>>(const lisk::expression &arg, bool &out);
bool operator>>(const lisk::expression &arg, lisk::atom &out);

#	define LISK_ATOM_HPP_FINISHED
#endif

#ifdef LISK_ATOM_FORWARD_ONLY
#	undef LISK_ATOM_FORWARD_ONLY
#else
#	if defined(LISK_ATOM_HPP_FINISHED) && !defined(LISK_ATOM_HPP_IMPL)
#		define LISK_ATOM_HPP_IMPL
#		include "atom.inl"
#	endif
#endif
