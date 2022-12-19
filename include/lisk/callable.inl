#include "callable.hpp"

#include "lisk/lambda.hpp"
#include "lisk/shared_list.hpp"

/* --- constructor --- */

inline lisk::callable::callable(const lisk::lambda &l)
: _value(lak::in_place_index<decltype(_value)::index_of<lambda_ptr>>,
         lambda_ptr::make(l))
{
}

inline lisk::callable::callable(const lisk::functor &f)
: _value(lak::in_place_index<decltype(_value)::index_of<lisk::functor>>, f)
{
}

/* --- operator= --- */

lisk::callable &lisk::callable::operator=(const lisk::lambda &l)
{
	_value.template emplace<decltype(_value)::index_of<lambda_ptr>>(
	  lambda_ptr::make(l));
	return *this;
}

lisk::callable &lisk::callable::operator=(const lisk::functor &f)
{
	_value.template emplace<decltype(_value)::index_of<lisk::functor>>(f);
	return *this;
}

inline bool lisk::callable::is_null() const
{
	return !(is_lambda() || is_functor());
}

inline bool lisk::callable::is_lambda() const
{
	return lak::get<lambda_ptr>(_value).map_or(
	  [](const auto &l) -> bool { return l; }, false);
}

inline bool lisk::callable::is_functor() const
{
	return lak::get<lisk::functor>(_value).map_or(
	  [](const auto &l) -> bool { return l; }, false);
}

inline lak::result<lisk::lambda &> lisk::callable::get_lambda() &
{
	return lak::get<lambda_ptr>(_value).and_then(
	  [](lambda_ptr &p) { return lak::result_from_pointer(p.get()); });
}

inline lak::result<const lisk::lambda &> lisk::callable::get_lambda() const &
{
	return lak::get<lambda_ptr>(_value).and_then(
	  [](const lambda_ptr &p)
	  {
		  return lak::result_from_pointer(
		    static_cast<const lisk::lambda *>(p.get()));
	  });
}

inline lak::result<lisk::lambda> lisk::callable::get_lambda() &&
{
	return lak::get<lambda_ptr>(_value).and_then(
	  [](lambda_ptr &p) { return lak::move_result_from_pointer(p.get()); });
}

inline lak::result<const lisk::functor &> lisk::callable::get_functor() const
{
	return lak::get<lisk::functor>(_value);
}
