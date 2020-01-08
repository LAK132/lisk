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

#ifndef LISK_SHARED_LIST_HPP
#define LISK_SHARED_LIST_HPP

#include "string.hpp"

#include <memory>

namespace lisk
{
  template<typename T>
  struct basic_shared_list_node
  {
    using pointer_type = std::shared_ptr<basic_shared_list_node>;

    T value;
    pointer_type next;

    static pointer_type create();
  };

  template<typename T>
  struct basic_shared_list
  {
    mutable std::shared_ptr<basic_shared_list_node<T>> _node;

    static basic_shared_list create();
    static basic_shared_list extends(const basic_shared_list &other);

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

    const basic_shared_list_node<T> &operator*() const;
    basic_shared_list_node<T> &operator*();

    const basic_shared_list_node<T> *operator->() const;
    basic_shared_list_node<T> *operator->();
  };

  template<typename T>
  string to_string(basic_shared_list<T> list);
}

#include "shared_list.inl"

#endif