/*
MIT License

Copyright (c) 2020 LAK132

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "shared_list.hpp"

namespace lisk
{
  template<typename T>
  typename shared_list_node<T>::pointer_type shared_list_node<T>::create()
  {
    return std::make_shared<shared_list_node<T>>();
  }

  template<typename T>
  shared_list<T> shared_list<T>::create()
  {
    return {shared_list_node<T>::create()};
  }

  template<typename T>
  shared_list<T> shared_list<T>::extends(const shared_list &other)
  {
    auto result = create();
    result.set_next(other);
    return result;
  }

  template<typename T>
  shared_list<T> &shared_list<T>::operator++()
  {
    if (_node) _node = _node->next;
    return *this;
  }

  template<typename T>
  shared_list<T> shared_list<T>::operator++(int)
  {
    auto result = *this;
    operator++();
    return result;
  }

  template<typename T>
  const T &shared_list<T>::value() const
  {
    if (!_node) _node = shared_list_node<T>::create();
    return _node->value;
  }

  template<typename T>
  T &shared_list<T>::value()
  {
    if (!_node) _node = shared_list_node<T>::create();
    return _node->value;
  }

  template<typename T>
  const T &shared_list<T>::next_value() const
  {
    if (!_node) _node = shared_list_node<T>::create();
    if (!_node->next) _node->next = shared_list_node<T>::create();
    return _node->next->value;
  }

  template<typename T>
  T &shared_list<T>::next_value()
  {
    if (!_node) _node = shared_list_node<T>::create();
    if (!_node->next) _node->next = shared_list_node<T>::create();
    return _node->next->value;
  }

  template<typename T>
  shared_list<T> shared_list<T>::next(size_t n) const
  {
    auto result = _node;
    while (result && n --> 0) result = result->next;
    return {result};
  }

  template<typename T>
  void shared_list<T>::set_next(shared_list<T> l)
  {
    if (!_node) _node = shared_list_node<T>::create();
    _node->next = l._node;
  }

  template<typename T>
  void shared_list<T>::clear()
  {
    _node.reset();
  }

  template<typename T>
  void shared_list<T>::clear_next()
  {
    if (_node) _node->next.reset();
  }

  template<typename T>
  shared_list<T> shared_list<T>::begin() const
  {
    if (_node && _node->value.empty())
      return end();
    else
      return *this;
  }

  template<typename T>
  shared_list<T> shared_list<T>::last() const
  {
    auto result = *this;
    while (result._node && result._node->next) ++result;
    return result;
  }

  template<typename T>
  shared_list<T> shared_list<T>::end() const
  {
    return {nullptr};
  }

  template<typename T>
  shared_list<T> shared_list<T>::clone() const
  {
    auto result = create();
    auto last = result;
    auto end = last;
    for (const auto &node : *this)
    {
      last.value() = node.value;
      last.set_next(create());
      end = last;
      ++last;
    }
    end.clear_next();
    return result;
  }

  template<typename T>
  bool shared_list<T>::operator==(const shared_list<T> &other) const
  {
    return _node.get() == other._node.get();
  }

  template<typename T>
  bool shared_list<T>::operator!=(const shared_list<T> &other) const
  {
    return !operator==(other);
  }

  template<typename T>
  shared_list<T>::operator bool() const
  {
    return _node && !_node->value.empty();
  }

  template<typename T>
  const shared_list_node<T> &shared_list<T>::operator*() const
  {
    if (!_node) _node = shared_list_node<T>::create();
    return *_node;
  }

  template<typename T>
  shared_list_node<T> &shared_list<T>::operator*()
  {
    if (!_node) _node = shared_list_node<T>::create();
    return *_node;
  }

  template<typename T>
  const shared_list_node<T> *shared_list<T>::operator->() const
  {
    if (!_node) _node = shared_list_node<T>::create();
    return _node.get();
  }

  template<typename T>
  shared_list_node<T> *shared_list<T>::operator->()
  {
    if (!_node) _node = shared_list_node<T>::create();
    return _node.get();
  }

  template<typename T>
  string to_string(shared_list<T> list)
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
    }
    while (list);
    result += ')';

    return result;
  }
}
