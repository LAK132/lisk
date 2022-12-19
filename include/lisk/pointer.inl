#include "pointer.hpp"

/* --- constructor --- */

template<typename T>
lisk::pointer::pointer(T *ptr)
: _value(lak::in_place_index<value_type::index_of<void *>>, ptr),
  _type(typeid(T))
{
}

template<typename T>
lisk::pointer::pointer(const T *ptr)
: _value(lak::in_place_index<value_type::index_of<const void *>>, ptr),
  _type(typeid(T))
{
}

template<typename T>
lisk::pointer::pointer(const lak::shared_ptr<T> &ptr)
: _value(lak::in_place_index<value_type::index_of<lak::shared_ptr<void>>>,
         ptr),
  _type(typeid(T))
{
}

/* --- operator= --- */

template<typename T>
lisk::pointer &lisk::pointer::operator=(T *ptr)
{
	_value.emplace<value_type::index_of<void *>>(ptr);
	_type = std::type_index(typeid(T));
	return *this;
}

template<typename T>
lisk::pointer &lisk::pointer::operator=(const T *ptr)
{
	_value.emplace<value_type::index_of<const void *>>(ptr);
	_type = std::type_index(typeid(T));
	return *this;
}

template<typename T>
lisk::pointer &lisk::pointer::operator=(const lak::shared_ptr<T> &ptr)
{
	_value.emplace<value_type::index_of<lak::shared_ptr<void>>>(ptr);
	_type = std::type_index(typeid(T));
	return *this;
}

template<typename T>
bool lisk::pointer::is_raw_ptr() const
{
	return _type == std::type_index(typeid(T)) &&
	       _value.holds<value_type::index_of<void *>>();
}

template<typename T>
bool lisk::pointer::is_raw_const_ptr() const
{
	return _type == std::type_index(typeid(T)) &&
	       _value.holds<value_type::index_of<const void *>>();
}

template<typename T>
bool lisk::pointer::is_shared_ptr() const
{
	return _type == std::type_index(typeid(T)) &&
	       _value.holds<value_type::index_of<lak::shared_ptr<void>>>();
}

template<typename T>
T *lisk::pointer::as_raw_ptr() const
{
	return static_cast<T *>(lak::get<void *>(_value));
}

template<typename T>
const T *lisk::pointer::as_raw_const_ptr() const
{
	return static_cast<const T *>(lak::get<const void *>(_value));
}

template<typename T>
lak::shared_ptr<T> lisk::pointer::as_shared_ptr() const
{
	return std::static_pointer_cast<T>(lak::get<lak::shared_ptr<void>>(_value));
}

template<typename T>
lisk::string lisk::to_string(T *t)
{
	// :TODO:
	return "<POINTER>";
}

template<typename T>
const lisk::string &lisk::type_name(T *t)
{
	const static lisk::string name = "<" + type_name(*t) + "*>";
	return name;
}

template<typename T>
lisk::string lisk::to_string(const T *)
{
	// :TODO:
	return "<POINTER>";
}

template<typename T>
const lisk::string &lisk::type_name(const T *)
{
	const static lisk::string name = "<const " + type_name(T{}) + "*>";
	return name;
}

template<typename T>
lisk::string lisk::to_string(const lak::shared_ptr<T> &)
{
	// :TODO:
	return "<POINTER>";
}

template<typename T>
const lisk::string &lisk::type_name(const lak::shared_ptr<T> &t)
{
	const static string name = "<managed " + type_name(*t.get()) + "*>";
	return name;
}
