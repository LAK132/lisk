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

#include "expression.hpp"

#include "eval.hpp"

namespace lisk
{
  expression::expression(null)
  {
    _value.emplace<null>();
  }

  expression::expression(const atom &a)
  {
    _value.emplace<atom>(a);
  }

  expression::expression(const shared_list<expression> &list)
  {
    _value.emplace<shared_list<expression>>(list);
  }

  expression::expression(const callable &c)
  {
    _value.emplace<callable>(c);
  }

  expression &expression::operator=(const null)
  {
    _value.emplace<null>();
    return *this;
  }

  expression &expression::operator=(const atom &a)
  {
    _value.emplace<atom>(a);
    return *this;
  }

  expression &expression::operator=(const shared_list<expression> &list)
  {
    _value.emplace<shared_list<expression>>(list);
    return *this;
  }

  expression &expression::operator=(const callable &c)
  {
    _value.emplace<callable>(c);
    return *this;
  }

  bool expression::is_null() const
  {
    return std::holds_alternative<null>(_value);
  }

  bool expression::is_atom() const
  {
    return std::holds_alternative<atom>(_value);
  }

  bool expression::is_list() const
  {
    return std::holds_alternative<shared_list<expression>>(_value);
  }

  bool expression::is_callable() const
  {
    return std::holds_alternative<callable>(_value);
  }

  const atom *expression::get_atom() const
  {
    if (is_atom())
      return &std::get<atom>(_value);
    else
      return nullptr;
  }

  atom *expression::get_atom()
  {
    if (is_atom())
      return &std::get<atom>(_value);
    else
      return nullptr;
  }

  const shared_list<expression> *expression::get_list() const
  {
    if (is_list())
      return &std::get<shared_list<expression>>(_value);
    else
      return nullptr;
  }

  shared_list<expression> *expression::get_list()
  {
    if (is_list())
      return &std::get<shared_list<expression>>(_value);
    else
      return nullptr;
  }

  const callable *expression::get_callable() const
  {
    if (is_callable())
      return &std::get<callable>(_value);
    else
      return nullptr;
  }

  callable *expression::get_callable()
  {
    if (is_callable())
      return &std::get<callable>(_value);
    else
      return nullptr;
  }

  const atom &expression::as_atom() const
  {
    return std::get<atom>(_value);
  }

  atom &expression::as_atom()
  {
    return std::get<atom>(_value);
  }

  const shared_list<expression> &expression::as_list() const
  {
    return std::get<shared_list<expression>>(_value);
  }

  shared_list<expression> &expression::as_list()
  {
    return std::get<shared_list<expression>>(_value);
  }

  const callable &expression::as_callable() const
  {
    return std::get<callable>(_value);
  }

  callable &expression::as_callable()
  {
    return std::get<callable>(_value);
  }

  string to_string(const expression &expr)
  {
    return expr.visit([](auto &&a) { return to_string(a); });
  }
}
