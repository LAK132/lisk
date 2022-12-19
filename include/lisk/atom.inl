#include "atom.hpp"

#include "lisk/expression.hpp"

#include <lak/result.hpp>

/* --- constructor --- */

inline lisk::atom::atom(nil) : _value(nil{}) {}

inline lisk::atom::atom(const lisk::symbol &sym) : _value(sym) {}

inline lisk::atom::atom(const string &str) : _value(str) {}

inline lisk::atom::atom(const number &num) : _value(num) {}

inline lisk::atom::atom(bool b)
: _value(lak::in_place_index<value_type::index_of<bool>>, b)
{
}

inline lisk::atom::atom(const pointer &ptr) : _value(ptr) {}

/* --- operator= --- */

inline lisk::atom &lisk::atom::operator=(lisk::atom::nil)
{
	_value.emplace<value_type::index_of<lisk::atom::nil>>();
	return *this;
}

inline lisk::atom &lisk::atom::operator=(const lisk::symbol &sym)
{
	_value.emplace<value_type::index_of<lisk::symbol>>(sym);
	return *this;
}

inline lisk::atom &lisk::atom::operator=(const lisk::string &str)
{
	_value.emplace<value_type::index_of<lisk::string>>(str);
	return *this;
}

inline lisk::atom &lisk::atom::operator=(const lisk::number &num)
{
	_value.emplace<value_type::index_of<lisk::number>>(num);
	return *this;
}

inline lisk::atom &lisk::atom::operator=(bool b)
{
	_value.emplace<value_type::index_of<bool>>(b);
	return *this;
}

inline lisk::atom &lisk::atom::operator=(const lisk::pointer &ptr)
{
	_value.emplace<value_type::index_of<lisk::pointer>>(ptr);
	return *this;
}

/* --- is_x --- */

inline bool lisk::atom::is_nil() const
{
	return _value.template holds<lisk::atom::nil>();
}

inline bool lisk::atom::is_symbol() const
{
	return _value.template holds<lisk::symbol>();
}

inline bool lisk::atom::is_string() const
{
	return _value.template holds<lisk::string>();
}

inline bool lisk::atom::is_number() const
{
	return _value.template holds<lisk::number>();
}

inline bool lisk::atom::is_bool() const
{
	return _value.template holds<bool>();
}

inline bool lisk::atom::is_pointer() const
{
	return _value.template holds<lisk::pointer>();
}

/* --- get_x --- */

inline lak::result<lisk::symbol &> lisk::atom::get_symbol() &
{
	return lak::get<lisk::symbol>(_value);
}

inline lak::result<const lisk::symbol &> lisk::atom::get_symbol() const &
{
	return lak::get<lisk::symbol>(_value);
}

inline lak::result<lisk::symbol> lisk::atom::get_symbol() &&
{
	return lak::get<lisk::symbol>(lak::move(_value));
}

inline lak::result<lisk::string &> lisk::atom::get_string() &
{
	return lak::get<lisk::string>(_value);
}

inline lak::result<const lisk::string &> lisk::atom::get_string() const &
{
	return lak::get<lisk::string>(_value);
}

inline lak::result<lisk::string> lisk::atom::get_string() &&
{
	return lak::get<lisk::string>(lak::move(_value));
}

inline lak::result<lisk::number &> lisk::atom::get_number() &
{
	return lak::get<lisk::number>(_value);
}

inline lak::result<const lisk::number &> lisk::atom::get_number() const &
{
	return lak::get<lisk::number>(_value);
}

inline lak::result<lisk::number> lisk::atom::get_number() &&
{
	return lak::get<lisk::number>(lak::move(_value));
}

inline lak::result<bool &> lisk::atom::get_bool() &
{
	return lak::get<bool>(_value);
}

inline lak::result<const bool &> lisk::atom::get_bool() const &
{
	return lak::get<bool>(_value);
}

inline lak::result<bool> lisk::atom::get_bool() &&
{
	return lak::get<bool>(lak::move(_value));
}

inline lak::result<lisk::pointer &> lisk::atom::get_pointer() &
{
	return lak::get<lisk::pointer>(_value);
}

inline lak::result<const lisk::pointer &> lisk::atom::get_pointer() const &
{
	return lak::get<lisk::pointer>(_value);
}

inline lak::result<lisk::pointer> lisk::atom::get_pointer() &&
{
	return lak::get<lisk::pointer>(lak::move(_value));
}
