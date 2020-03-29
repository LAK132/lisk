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

#include "lisk/environment.hpp"

namespace lisk
{
  environment environment::extends(const environment &other)
  {
    return {value_type::extends(other._map)};
  }

  void environment::define_expr(const symbol &sym, const expression &expr)
  {
    _map.value()[sym] = expr;
  }

  void environment::define_atom(const symbol &sym, const atom &a)
  {
    _map.value()[sym] = a;
  }

  void environment::define_list(const symbol &sym, const shared_list &list)
  {
    _map.value()[sym] = list;
  }

  void environment::define_callable(const symbol &sym, const callable &c)
  {
    _map.value()[sym] = c;
  }

  void environment::define_functor(const symbol &sym, const functor &f)
  {
    _map.value()[sym] = f;
  }

  expression environment::operator[](const symbol &sym) const
  {
    for (const auto &node : _map)
      if (const auto it = node.value.find(sym); it != node.value.end())
        return it->second;

    return exception{"Environment lookup failed, couldn't find '" + sym +
                     "' in '" + to_string(*this) + "'"};
  }

  environment environment::clone(size_t depth) const
  {
    environment result;
    result._map = _map.clone(depth);
    return result;
  }

  environment &environment::squash(size_t depth)
  {
    if (depth == 0)
    {
      *this = clone(0);
      while (_map._node->next)
      {
        _map.next_value().merge(_map.value());
        _map++;
      }
    }
    else
    {
      *this = clone(depth + 1);
      while (depth-- > 0 && _map._node->next)
      {
        _map.next_value().merge(_map.value());
        _map++;
      }
    }
    return *this;
  }

  string to_string(const environment &env)
  {
    string result;
    result += "(";
    for (const auto &node : env._map)
      for (const auto &[key, value] : node.value)
        result += "(" + to_string(key) + " " + to_string(value) + ") ";
    if (result.back() == ' ') result.pop_back();
    result += ")";
    return result;
  }
}