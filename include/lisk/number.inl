#include "number.hpp"

#include "lisk/expression.hpp"

/* --- constructor --- */

inline lisk::number::number(lisk::uint_t u)
: _value(lak::in_place_index<value_type::index_of<lisk::uint_t>>, u)
{
}

inline lisk::number::number(lisk::sint_t s)
: _value(lak::in_place_index<value_type::index_of<lisk::sint_t>>, s)
{
}

inline lisk::number::number(lisk::real_t r)
: _value(lak::in_place_index<value_type::index_of<lisk::real_t>>, r)
{
}

/* --- operator= --- */

inline lisk::number &lisk::number::operator=(lisk::uint_t u)
{
	_value.emplace<value_type::index_of<lisk::uint_t>>(u);
	return *this;
}

inline lisk::number &lisk::number::operator=(lisk::sint_t s)
{
	_value.emplace<value_type::index_of<lisk::sint_t>>(s);
	return *this;
}

inline lisk::number &lisk::number::operator=(lisk::real_t r)
{
	_value.emplace<value_type::index_of<lisk::real_t>>(r);
	return *this;
}

inline bool lisk::number::is_uint() const
{
	return _value.holds<value_type::index_of<lisk::uint_t>>();
}

inline bool lisk::number::is_sint() const
{
	return _value.holds<value_type::index_of<lisk::sint_t>>();
}

inline bool lisk::number::is_real() const
{
	return _value.holds<value_type::index_of<lisk::real_t>>();
}

inline lak::result<lisk::uint_t &> lisk::number::get_uint() &
{
	return lak::get<lisk::uint_t>(_value);
}

inline lak::result<const lisk::uint_t &> lisk::number::get_uint() const &
{
	return lak::get<lisk::uint_t>(_value);
}

inline lak::result<lisk::uint_t> lisk::number::get_uint() &&
{
	return lak::get<lisk::uint_t>(lak::move(_value));
}

inline lak::result<lisk::sint_t &> lisk::number::get_sint() &
{
	return lak::get<lisk::sint_t>(_value);
}

inline lak::result<const lisk::sint_t &> lisk::number::get_sint() const &
{
	return lak::get<lisk::sint_t>(_value);
}

inline lak::result<lisk::sint_t> lisk::number::get_sint() &&
{
	return lak::get<lisk::sint_t>(lak::move(_value));
}

inline lak::result<lisk::real_t &> lisk::number::get_real() &
{
	return lak::get<lisk::real_t>(_value);
}

inline lak::result<const lisk::real_t &> lisk::number::get_real() const &
{
	return lak::get<lisk::real_t>(_value);
}

inline lak::result<lisk::real_t> lisk::number::get_real() &&
{
	return lak::get<lisk::real_t>(lak::move(_value));
}

lisk::number operator+(lisk::number A, lisk::number B)
{
	return lak::visit([](auto &&A, auto &&B) -> lisk::number
	                  { return lisk::number(A + B); },
	                  A._value,
	                  B._value);
}

lisk::number operator-(lisk::number A, lisk::number B)
{
	return lak::visit([](auto &&A, auto &&B) -> lisk::number
	                  { return lisk::number(A - B); },
	                  A._value,
	                  B._value);
}

lisk::number operator*(lisk::number A, lisk::number B)
{
	return lak::visit([](auto &&A, auto &&B) -> lisk::number
	                  { return lisk::number(A * B); },
	                  A._value,
	                  B._value);
}

lisk::number operator/(lisk::number A, lisk::number B)
{
	return lak::visit([](auto &&A, auto &&B) -> lisk::number
	                  { return lisk::number(A / B); },
	                  A._value,
	                  B._value);
}

lisk::number &operator+=(lisk::number &A, lisk::number B)
{
	return A = A + B;
}

lisk::number &operator-=(lisk::number &A, lisk::number B)
{
	return A = A - B;
}

lisk::number &operator*=(lisk::number &A, lisk::number B)
{
	return A = A * B;
}

lisk::number &operator/=(lisk::number &A, lisk::number B)
{
	return A = A / B;
}
