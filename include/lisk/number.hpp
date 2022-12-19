#ifndef LISK_NUMBER_HPP
#	define LISK_NUMBER_HPP

#	include "lisk/string.hpp"

#	include <lak/result.hpp>
#	include <lak/variant.hpp>

namespace lisk
{
	using uint_t = unsigned long long;
	using sint_t = signed long long;
	using real_t = long double;

	struct number
	{
		using value_type = lak::variant<lisk::uint_t, lisk::sint_t, lisk::real_t>;
		value_type _value;

		number()                = default;
		number(const number &n) = default;

		number &operator=(const number &n) = default;

		inline number(lisk::uint_t u);
		inline number(lisk::sint_t s);
		inline number(lisk::real_t r);

		inline number &operator=(lisk::uint_t u);
		inline number &operator=(lisk::sint_t s);
		inline number &operator=(lisk::real_t r);

		inline bool is_uint() const;
		inline bool is_sint() const;
		inline bool is_real() const;

		inline lak::result<lisk::uint_t &> get_uint() &;
		inline lak::result<const lisk::uint_t &> get_uint() const &;
		inline lak::result<lisk::uint_t> get_uint() &&;

		inline lak::result<lisk::sint_t &> get_sint() &;
		inline lak::result<const lisk::sint_t &> get_sint() const &;
		inline lak::result<lisk::sint_t> get_sint() &&;

		inline lak::result<lisk::real_t &> get_real() &;
		inline lak::result<const lisk::real_t &> get_real() const &;
		inline lak::result<lisk::real_t> get_real() &&;

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

	lisk::string to_string(const lisk::number &num);
	const lisk::string &type_name(const lisk::number &);

	lisk::string to_string(lisk::uint_t num);
	const lisk::string &type_name(lisk::uint_t);

	lisk::string to_string(lisk::sint_t num);
	const lisk::string &type_name(lisk::sint_t);

	lisk::string to_string(lisk::real_t num);
	const lisk::string &type_name(lisk::real_t);

	struct expression;
}

bool operator>>(const lisk::expression &arg, lisk::number &out);
bool operator>>(const lisk::expression &arg, lisk::uint_t &out);
bool operator>>(const lisk::expression &arg, lisk::sint_t &out);
bool operator>>(const lisk::expression &arg, lisk::real_t &out);

inline lisk::number operator+(lisk::number A, lisk::number B);
inline lisk::number operator-(lisk::number A, lisk::number B);
inline lisk::number operator*(lisk::number A, lisk::number B);
inline lisk::number operator/(lisk::number A, lisk::number B);
inline lisk::number &operator+=(lisk::number &A, lisk::number B);
inline lisk::number &operator-=(lisk::number &A, lisk::number B);
inline lisk::number &operator*=(lisk::number &A, lisk::number B);
inline lisk::number &operator/=(lisk::number &A, lisk::number B);

#	define LISK_NUMBER_HPP_FINISHED
#endif

#ifdef LISK_NUMBER_FORWARD_ONLY
#	undef LISK_NUMBER_FORWARD_ONLY
#else
#	if defined(LISK_NUMBER_HPP_FINISHED) && !defined(LISK_NUMBER_HPP_IMPL)
#		define LISK_NUMBER_HPP_IMPL
#		include "number.inl"
#	endif
#endif
