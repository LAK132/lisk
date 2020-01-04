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

#include "atom.hpp"

namespace lisk
{
  atom::atom(const nil)
  {
    _value.emplace<nil>();
  }

  atom::atom(const symbol &sym)
  {
    _value.emplace<symbol>(sym);
  }

  atom::atom(const string &str)
  {
    _value.emplace<string>(str);
  }

  atom::atom(const number &num)
  {
    _value.emplace<number>(num);
  }

  atom &atom::operator=(atom::nil)
  {
    _value.emplace<nil>();
    return *this;
  }

  atom &atom::operator=(const symbol &sym)
  {
    _value.emplace<symbol>(sym);
    return *this;
  }

  atom &atom::operator=(const string &str)
  {
    _value.emplace<string>(str);
    return *this;
  }

  atom &atom::operator=(const number &num)
  {
    _value.emplace<number>(num);
    return *this;
  }

  bool atom::is_nil() const
  {
    return std::holds_alternative<nil>(_value);
  }

  bool atom::is_symbol() const
  {
    return std::holds_alternative<symbol>(_value);
  }

  bool atom::is_string() const
  {
    return std::holds_alternative<string>(_value);
  }

  bool atom::is_number() const
  {
    return std::holds_alternative<number>(_value);
  }

  const symbol *atom::get_symbol() const
  {
    if (is_symbol())
      return &std::get<symbol>(_value);
    else
      return nullptr;
  }

  symbol *atom::get_symbol()
  {
    if (is_symbol())
      return &std::get<symbol>(_value);
    else
      return nullptr;
  }

  const string *atom::get_string() const
  {
    if (is_string())
      return &std::get<string>(_value);
    else
      return nullptr;
  }

  string *atom::get_string()
  {
    if (is_string())
      return &std::get<string>(_value);
    else
      return nullptr;
  }

  const number *atom::get_number() const
  {
    if (is_number())
      return &std::get<number>(_value);
    else
      return nullptr;
  }

  number *atom::get_number()
  {
    if (is_number())
      return &std::get<number>(_value);
    else
      return nullptr;
  }

  const symbol &atom::as_symbol() const
  {
    return std::get<symbol>(_value);
  }

  symbol &atom::as_symbol()
  {
    return std::get<symbol>(_value);
  }

  const string &atom::as_string() const
  {
    return std::get<string>(_value);
  }

  string &atom::as_string()
  {
    return std::get<string>(_value);
  }

  const number &atom::as_number() const
  {
    return std::get<number>(_value);
  }

  number &atom::as_number()
  {
    return std::get<number>(_value);
  }

  string to_string(atom::nil)
  {
    return "nil";
  }

  string to_string(const atom &a)
  {
    return a.visit([](auto &&a) { return to_string(a); });
  }
}