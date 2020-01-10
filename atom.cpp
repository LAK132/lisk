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
#include "expression.hpp"

namespace lisk
{
  atom::atom(nil)
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

  atom::atom(const pointer &ptr)
  {
    _value.emplace<pointer>(ptr);
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

  atom &atom::operator=(const pointer &ptr)
  {
    _value.emplace<pointer>(ptr);
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

  bool atom::is_pointer() const
  {
    return std::holds_alternative<pointer>(_value);
  }

  const symbol *atom::get_symbol() const
  {
    return is_symbol() ? &std::get<symbol>(_value) : nullptr;
  }

  symbol *atom::get_symbol()
  {
    return is_symbol() ? &std::get<symbol>(_value) : nullptr;
  }

  const string *atom::get_string() const
  {
    return is_string() ? &std::get<string>(_value) : nullptr;
  }

  string *atom::get_string()
  {
    return is_string() ? &std::get<string>(_value) : nullptr;
  }

  const number *atom::get_number() const
  {
    return is_number() ? &std::get<number>(_value) : nullptr;
  }

  number *atom::get_number()
  {
    return is_number() ? &std::get<number>(_value) : nullptr;
  }

  const pointer *atom::get_pointer() const
  {
    return is_pointer() ? &std::get<pointer>(_value) : nullptr;
  }

  pointer *atom::get_pointer()
  {
    return is_pointer() ? &std::get<pointer>(_value) : nullptr;
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

  const pointer &atom::as_pointer() const
  {
    return std::get<pointer>(_value);
  }

  pointer &atom::as_pointer()
  {
    return std::get<pointer>(_value);
  }

  string to_string(atom::nil)
  {
    return "nil";
  }

  const string &type_name(atom::nil)
  {
    const static string name = "nill";
    return name;
  }

  string to_string(const atom &a)
  {
    return a.visit([](auto &&a) { return to_string(a); });
  }

  const string &type_name(const atom &)
  {
    const static string name = "atom";
    return name;
  }
}

bool operator>>(const lisk::expression &arg, lisk::atom::nil)
{
  return arg.is_atom() && arg.as_atom().is_nil();
}

bool operator>>(const lisk::expression &arg, lisk::atom &out)
{
  if (!arg.is_atom()) return false;

  out = arg.as_atom();
  return true;
}
