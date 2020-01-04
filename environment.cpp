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

#include "environment.hpp"

namespace lisk
{
  environment environment::extends(const environment &other)
  {
    return {value_type::extends(other.map)};
  }

  void environment::define_expr(const string &str, const expression &expr)
  {
    map.value()[str] = expr;
  }

  void environment::define_atom(const string &str, const atom &a)
  {
    map.value()[str] = a;
  }

  void environment::define_list(const string &str,
                                const shared_list<expression> &list)
  {
    map.value()[str] = list;
  }

  void environment::define_callable(const string &str, const callable &c)
  {
    map.value()[str] = c;
  }

  void environment::define_functor(const string &str, const functor &f)
  {
    map.value()[str] = f;
  }

  expression environment::operator[](const string &str) const
  {
    for (const auto &node : map)
      if (const auto it = node.value.find(str); it != node.value.end())
        return it->second;
    return {expression::null{}};
  }
}