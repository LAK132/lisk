#include "shared_list.hpp"

namespace lisk
{
	template<typename T>
	typename basic_shared_list_node<T>::pointer_type
	basic_shared_list_node<T>::create()
	{
		return std::make_shared<basic_shared_list_node<T>>();
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::create()
	{
		return {basic_shared_list_node<T>::create()};
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::extends(
	  const basic_shared_list &other)
	{
		auto result = create();
		result.set_next(other);
		return result;
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::append(basic_shared_list &list,
	                                                  const T &v)
	{
		basic_shared_list result = _node ? next() : *this;
		result.value()           = v;
		return result;
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::append(basic_shared_list &list,
	                                                  T &&v)
	{
		basic_shared_list result = _node ? next() : *this;
		result.value()           = std::move(v);
		return result;
	}

	template<typename T>
	basic_shared_list<T> &basic_shared_list<T>::operator++()
	{
		if (_node) _node = _node->next;
		return *this;
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::operator++(int)
	{
		auto result = *this;
		operator++();
		return result;
	}

	template<typename T>
	const T &basic_shared_list<T>::value() const
	{
		if (!_node) _node = basic_shared_list_node<T>::create();
		return _node->value;
	}

	template<typename T>
	T &basic_shared_list<T>::value()
	{
		if (!_node) _node = basic_shared_list_node<T>::create();
		return _node->value;
	}

	template<typename T>
	const T &basic_shared_list<T>::next_value() const
	{
		if (!_node) _node = basic_shared_list_node<T>::create();
		if (!_node->next) _node->next = basic_shared_list_node<T>::create();
		return _node->next->value;
	}

	template<typename T>
	T &basic_shared_list<T>::next_value()
	{
		if (!_node) _node = basic_shared_list_node<T>::create();
		if (!_node->next) _node->next = basic_shared_list_node<T>::create();
		return _node->next->value;
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::next(size_t n) const
	{
		auto result = _node;
		while (result && n-- > 0) result = result->next;
		return {result};
	}

	template<typename T>
	void basic_shared_list<T>::set_next(basic_shared_list<T> l)
	{
		if (!_node) _node = basic_shared_list_node<T>::create();
		_node->next = l._node;
	}

	template<typename T>
	void basic_shared_list<T>::clear()
	{
		_node.reset();
	}

	template<typename T>
	void basic_shared_list<T>::clear_next()
	{
		if (_node) _node->next.reset();
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::begin() const
	{
		// :TODO: .empty doesn't work here if we're dealing with unoreded_maps
		// where the beginning map might be empty!!!
		if (_node && !_node->next && _node->value.empty())
			return end();
		else
			return *this;
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::last() const
	{
		auto result = *this;
		while (result._node && result._node->next) ++result;
		return result;
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::end() const
	{
		return {nullptr};
	}

	template<typename T>
	basic_shared_list<T> basic_shared_list<T>::clone(size_t depth) const
	{
		auto result = create();
		auto last   = result;
		auto end    = last;
		if (depth == 0)
		{
			for (const auto &node : *this)
			{
				last.value() = node.value;
				last.set_next(create());
				end = last;
				++last;
			}
			end.clear_next();
		}
		else
		{
			auto iter = *this;
			while (depth-- > 0 && iter)
			{
				last.value() = iter.value();
				last.set_next(create());
				end = last;
				++last;
				++iter;
			}
			if (iter)
				end.set_next(iter);
			else
				end.clear_next();
		}
		return result;
	}

	template<typename T>
	bool basic_shared_list<T>::operator==(
	  const basic_shared_list<T> &other) const
	{
		return _node.get() == other._node.get();
	}

	template<typename T>
	bool basic_shared_list<T>::operator!=(
	  const basic_shared_list<T> &other) const
	{
		return !operator==(other);
	}

	template<typename T>
	basic_shared_list<T>::operator bool() const
	{
		return _node && (_node->next || !_node->value.empty());
	}

	template<typename T>
	const basic_shared_list_node<T> &basic_shared_list<T>::operator*() const
	{
		if (!_node) _node = basic_shared_list_node<T>::create();
		return *_node;
	}

	template<typename T>
	basic_shared_list_node<T> &basic_shared_list<T>::operator*()
	{
		if (!_node) _node = basic_shared_list_node<T>::create();
		return *_node;
	}

	template<typename T>
	const basic_shared_list_node<T> *basic_shared_list<T>::operator->() const
	{
		if (!_node) _node = basic_shared_list_node<T>::create();
		return _node.get();
	}

	template<typename T>
	basic_shared_list_node<T> *basic_shared_list<T>::operator->()
	{
		if (!_node) _node = basic_shared_list_node<T>::create();
		return _node.get();
	}

	template<typename T>
	string to_string(basic_shared_list<T> list)
	{
		if (!list._node) return "";
		if (list.value().empty()) return "()";

		string result;

		result += '(';
		do
		{
			result += to_string(list.value());
			++list;
			if (list) result += ' ';
		} while (list);
		result += ')';

		return result;
	}

	template<typename T>
	const string &type_name(const basic_shared_list<T> &)
	{
		const static string name = "basic_shared_list<" + type_name(T{}) + ">";
		return name;
	}
}
