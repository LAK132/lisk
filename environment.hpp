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

#ifndef LISK_ENVIRONMENT_HPP
#define LISK_ENVIRONMENT_HPP

#include "callable.hpp"
#include "expression.hpp"
#include "shared_list.hpp"

#include <unordered_map>

namespace lisk
{
  struct environment
  {
    using value_type =
      basic_shared_list<std::unordered_map<symbol, expression>>;
    value_type _map = {};

    environment() = default;
    environment(const environment &) = default;
    environment(environment &&) = default;

    environment& operator=(const environment &) = default;
    environment& operator=(environment &&) = default;

    static environment extends(const environment &other);

    void define_expr(const symbol &sym, const expression &expr);
    void define_atom(const symbol &sym, const atom &a);
    void define_list(const symbol &sym, const shared_list &list);
    void define_callable(const symbol &sym, const callable &c);
    void define_functor(const symbol &sym, const functor &f);

    expression operator[](const symbol &sym) const;

    environment clone(size_t depth = 0) const;
    environment &squash(size_t depth);
  };

  string to_string(const environment &env);
}

#endif
