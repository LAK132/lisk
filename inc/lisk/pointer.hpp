#ifndef LISK_POINTER_HPP
#define LISK_POINTER_HPP

#include "string.hpp"

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <variant>

namespace lisk
{
	struct expression;

	struct pointer
	{
		std::variant<void *, const void *, std::shared_ptr<void>> _value;
		std::type_index _type;

		pointer() : _value((const void *)nullptr), _type(typeid(void)) {}
		pointer(const pointer &ptr) = default;
		pointer(pointer &&ptr)      = default;

		pointer &operator=(const pointer &ptr) = default;
		pointer &operator=(pointer &&ptr) = default;

		template<typename T>
		pointer(T *ptr);
		template<typename T>
		pointer(const T *ptr);
		template<typename T>
		pointer(std::shared_ptr<T> ptr);

		template<typename T>
		pointer &operator=(T *ptr);
		template<typename T>
		pointer &operator=(const T *ptr);
		template<typename T>
		pointer &operator=(std::shared_ptr<T> ptr);

		template<typename T>
		bool is_raw_ptr() const;
		template<typename T>
		bool is_raw_const_ptr() const;
		template<typename T>
		bool is_shared_ptr() const;

		template<typename T>
		T *as_raw_ptr() const;
		template<typename T>
		const T *as_raw_const_ptr() const;
		template<typename T>
		std::shared_ptr<T> as_shared_ptr() const;
	};

	string to_string(const pointer &);
	const string &type_name(const pointer &);

	template<typename T>
	string to_string(T *);
	template<typename T>
	const string &type_name(T *);

	template<typename T>
	string to_string(const T *);
	template<typename T>
	const string &type_name(const T *);

	template<typename T>
	string to_string(const std::shared_ptr<T> &);
	template<typename T>
	const string &type_name(const std::shared_ptr<T> &);
}

bool operator>>(const lisk::expression &arg, lisk::pointer &out);

#include "pointer.inl"

#endif