#ifndef LISK_SHARED_LIST_HPP
#	define LISK_SHARED_LIST_HPP

#	include "lisk/string.hpp"

#	include <lak/memory.hpp>

namespace lisk
{
	template<typename T>
	struct basic_shared_list_node
	{
		using pointer_type = lak::shared_ptr<basic_shared_list_node>;

		T value;
		pointer_type next;

		static pointer_type create();
	};

	template<typename T>
	struct basic_shared_list
	{
		mutable lak::shared_ptr<lisk::basic_shared_list_node<T>> _node = {};

		static basic_shared_list create();

		// prepend
		static basic_shared_list extends(const basic_shared_list &other);
		static basic_shared_list append(basic_shared_list &list, const T &v);
		static basic_shared_list append(basic_shared_list &list, T &&v);

		basic_shared_list &operator++();
		basic_shared_list operator++(int);

		const T &value() const;
		T &value();
		const T &next_value() const;
		T &next_value();

		basic_shared_list next(size_t n = 1) const;
		void set_next(basic_shared_list l);

		void clear();
		void clear_next();

		basic_shared_list begin() const;
		basic_shared_list last() const;
		basic_shared_list end() const;

		// 0 for full clone
		basic_shared_list clone(size_t depth = 0) const;

		bool operator==(const basic_shared_list &other) const;
		bool operator!=(const basic_shared_list &other) const;

		operator bool() const;

		const lisk::basic_shared_list_node<T> &operator*() const;
		lisk::basic_shared_list_node<T> &operator*();

		const lisk::basic_shared_list_node<T> *operator->() const;
		lisk::basic_shared_list_node<T> *operator->();
	};

	template<typename T>
	lisk::string to_string(lisk::basic_shared_list<T> list);

	template<typename T>
	const lisk::string &type_name(const lisk::basic_shared_list<T> &);
}

#	define LISK_SHARED_LIST_HPP_FINISHED
#endif

#ifdef LISK_SHARED_LIST_FORWARD_ONLY
#	undef LISK_SHARED_LIST_FORWARD_ONLY
#else
#	if defined(LISK_SHARED_LIST_HPP_FINISHED) &&                               \
	  !defined(LISK_SHARED_LIST_HPP_IMPL)
#		define LISK_SHARED_LIST_HPP_IMPL
#		include "shared_list.inl"
#	endif
#endif
