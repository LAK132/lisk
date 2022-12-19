#include "expression.hpp"

#include "lisk/callable.hpp"

/* --- constructor --- */

lisk::expression::expression(null)
: _value(lak::in_place_index<value_type::index_of<null>>)
{
}

lisk::expression::expression(const lisk::atom &a) : _value(a) {}

lisk::expression::expression(lisk::atom::nil)
: _value(lak::in_place_index<value_type::index_of<lisk::atom>>,
         lisk::atom::nil{})
{
}

lisk::expression::expression(const lisk::shared_list &list) : _value(list) {}

lisk::expression::expression(const lisk::eval_shared_list &list) : _value(list)
{
}

lisk::expression::expression(const lisk::callable &c) : _value(c) {}

lisk::expression::expression(const lisk::exception &exc) : _value(exc) {}

/* --- operator= --- */

lisk::expression &lisk::expression::operator=(null)
{
	_value.emplace<decltype(_value)::index_of<null>>();
	return *this;
}

lisk::expression &lisk::expression::operator=(const lisk::atom &a)
{
	_value.emplace<decltype(_value)::index_of<atom>>(a);
	return *this;
}

lisk::expression &lisk::expression::operator=(lisk::atom::nil)
{
	_value.emplace<decltype(_value)::index_of<atom>>(atom::nil{});
	return *this;
}

lisk::expression &lisk::expression::operator=(const lisk::shared_list &list)
{
	_value.emplace<decltype(_value)::index_of<shared_list>>(list);
	return *this;
}

lisk::expression &lisk::expression::operator=(
  const lisk::eval_shared_list &list)
{
	_value.emplace<decltype(_value)::index_of<eval_shared_list>>(list);
	return *this;
}

lisk::expression &lisk::expression::operator=(const lisk::callable &c)
{
	_value.emplace<decltype(_value)::index_of<callable>>(c);
	return *this;
}

lisk::expression &lisk::expression::operator=(const lisk::exception &exc)
{
	_value.emplace<decltype(_value)::index_of<exception>>(exc);
	return *this;
}

bool lisk::expression::is_null() const
{
	return _value.template holds<null>();
}

bool lisk::expression::is_atom() const
{
	return _value.template holds<lisk::atom>();
}

bool lisk::expression::is_list() const
{
	return _value.template holds<lisk::shared_list>();
}

bool lisk::expression::is_eval_list() const
{
	return _value.template holds<lisk::eval_shared_list>();
}

bool lisk::expression::is_callable() const
{
	return _value.template holds<lisk::callable>();
}

bool lisk::expression::is_exception() const
{
	return _value.template holds<lisk::exception>();
}

inline lak::result<lisk::atom &> lisk::expression::get_atom() &
{
	return lak::get<lisk::atom>(_value);
}

inline lak::result<const lisk::atom &> lisk::expression::get_atom() const &
{
	return lak::get<lisk::atom>(_value);
}

inline lak::result<lisk::atom> lisk::expression::get_atom() &&
{
	return lak::get<lisk::atom>(lak::move(_value));
}

inline lak::result<lisk::shared_list &> lisk::expression::get_list() &
{
	return lak::get<lisk::shared_list>(_value);
}

inline lak::result<const lisk::shared_list &> lisk::expression::get_list()
  const &
{
	return lak::get<lisk::shared_list>(_value);
}

inline lak::result<lisk::shared_list> lisk::expression::get_list() &&
{
	return lak::get<lisk::shared_list>(lak::move(_value));
}

inline lak::result<lisk::eval_shared_list &> lisk::expression::get_eval_list()
  &
{
	return lak::get<lisk::eval_shared_list>(_value);
}

inline lak::result<const lisk::eval_shared_list &>
lisk::expression::get_eval_list() const &
{
	return lak::get<lisk::eval_shared_list>(_value);
}

inline lak::result<lisk::eval_shared_list> lisk::expression::get_eval_list() &&
{
	return lak::get<lisk::eval_shared_list>(lak::move(_value));
}

inline lak::result<lisk::callable &> lisk::expression::get_callable() &
{
	return lak::get<lisk::callable>(_value);
}

inline lak::result<const lisk::callable &> lisk::expression::get_callable()
  const &
{
	return lak::get<lisk::callable>(_value);
}

inline lak::result<lisk::callable> lisk::expression::get_callable() &&
{
	return lak::get<lisk::callable>(lak::move(_value));
}

inline lak::result<lisk::exception &> lisk::expression::get_exception() &
{
	return lak::get<lisk::exception>(_value);
}

inline lak::result<const lisk::exception &> lisk::expression::get_exception()
  const &
{
	return lak::get<lisk::exception>(_value);
}

inline lak::result<lisk::exception> lisk::expression::get_exception() &&
{
	return lak::get<lisk::exception>(lak::move(_value));
}

template<typename T>
bool operator>>(const lisk::expression &arg, T *&out)
{
	if_let_ok (auto &&atom, arg.get_atom())
	{
		if_let_ok (auto &&ptr, atom.get_pointer())
		{
			if_let_ok (auto &&p, ptr.template get<T>())
			{
				out = p;
				return true;
			}
		}
	}
	return false;
}

template<typename T>
bool operator>>(const lisk::expression &arg, lak::shared_ptr<T> &out)
{
	if_let_ok (auto &&atom, arg.get_atom())
	{
		if_let_ok (auto &&ptr, atom.get_pointer())
		{
			if_let_ok (auto &&p, ptr.template get_shared<T>())
			{
				out = p;
				return true;
			}
		}
	}
	return false;
}
