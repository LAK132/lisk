#ifndef LISK_POINTER_HPP
#	define LISK_POINTER_HPP

#	include "lisk/string.hpp"

#	include <lak/memory.hpp>
#	include <lak/variant.hpp>

#	include <typeindex>
#	include <typeinfo>

namespace lisk
{
	struct pointer
	{
		using value_type =
		  lak::variant<void *, const void *, lak::shared_ptr<void>>;
		value_type _value;
		std::type_index _type;

		inline pointer() : _value((const void *)nullptr), _type(typeid(void)) {}
		pointer(const pointer &ptr) = default;
		pointer(pointer &&ptr)      = default;

		pointer &operator=(const pointer &ptr) = default;
		pointer &operator=(pointer &&ptr) = default;

		template<typename T>
		pointer(T *ptr);
		template<typename T>
		pointer(const T *ptr);
		template<typename T>
		pointer(const lak::shared_ptr<T> &ptr);

		template<typename T>
		pointer &operator=(T *ptr);
		template<typename T>
		pointer &operator=(const T *ptr);
		template<typename T>
		pointer &operator=(const lak::shared_ptr<T> &ptr);

		template<typename T>
		lak::result<T *> get() const
		{
			if (_type != std::type_index(typeid(T))) return lak::err_t{};

			if_let_ok (void *p, lak::get<void *>(_value)) return static_cast<T *>(p);

			if constexpr (lak::is_const_v<T>)
				if_let_ok (const void *p, lak::get<const void *>(_value))
					return static_cast<T *>(p);

			if_let_ok (const auto &p, lak::get<lak::shared_ptr<void>>(_value))
				return static_cast<T *>(p.get());

			return lak::err_t{};
		}

		template<typename T>
		lak::result<lak::shared_ptr<T>> get_shared() const
		{
			if (_type != std::type_index(typeid(T))) return lak::err_t{};

			if_let_ok (const auto &p, lak::get<lak::shared_ptr<void>>(_value))
				return lak::ok_t{static_cast<lak::shared_ptr<T>>(p)};

			return lak::err_t{};
		}

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
		lak::shared_ptr<T> as_shared_ptr() const;
	};

	lisk::string to_string(const lisk::pointer &);
	const lisk::string &type_name(const lisk::pointer &);

	template<typename T>
	lisk::string to_string(T *);
	template<typename T>
	const lisk::string &type_name(T *);

	template<typename T>
	lisk::string to_string(const T *);
	template<typename T>
	const lisk::string &type_name(const T *);

	template<typename T>
	lisk::string to_string(const lak::shared_ptr<T> &);
	template<typename T>
	const lisk::string &type_name(const lak::shared_ptr<T> &);

	struct expression;
}

bool operator>>(const lisk::expression &arg, lisk::pointer &out);

#	define LISK_POINTER_HPP_FINISHED
#endif

#ifdef LISK_POINTER_FORWARD_ONLY
#	undef LISK_POINTER_FORWARD_ONLY
#else
#	if defined(LISK_POINTER_HPP_FINISHED) && !defined(LISK_POINTER_HPP_IMPL)
#		define LISK_POINTER_HPP_IMPL
#		include "pointer.inl"
#	endif
#endif
