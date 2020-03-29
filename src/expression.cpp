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

#include "lisk/expression.hpp"

#include "lisk/eval.hpp"

namespace lisk
{
  expression::expression(null) { _value.emplace<null>(); }

  expression::expression(const atom &a) { _value.emplace<atom>(a); }

  expression::expression(atom::nil) { _value.emplace<atom>(atom::nil{}); }

  expression::expression(const shared_list &list)
  {
    _value.emplace<shared_list>(list);
  }

  expression::expression(const eval_shared_list &list)
  {
    _value.emplace<eval_shared_list>(list);
  }

  expression::expression(const callable &c) { _value.emplace<callable>(c); }

  expression::expression(const exception &exc)
  {
    _value.emplace<exception>(exc);
  }

  expression &expression::operator=(null)
  {
    _value.emplace<null>();
    return *this;
  }

  expression &expression::operator=(const atom &a)
  {
    _value.emplace<atom>(a);
    return *this;
  }

  expression &expression::operator=(atom::nil)
  {
    _value.emplace<atom>(atom::nil{});
    return *this;
  }

  expression &expression::operator=(const shared_list &list)
  {
    _value.emplace<shared_list>(list);
    return *this;
  }

  expression &expression::operator=(const eval_shared_list &list)
  {
    _value.emplace<eval_shared_list>(list);
    return *this;
  }

  expression &expression::operator=(const callable &c)
  {
    _value.emplace<callable>(c);
    return *this;
  }

  expression &expression::operator=(const exception &exc)
  {
    _value.emplace<exception>(exc);
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
    return std::holds_alternative<shared_list>(_value);
  }

  bool expression::is_eval_list() const
  {
    return std::holds_alternative<eval_shared_list>(_value);
  }

  bool expression::is_callable() const
  {
    return std::holds_alternative<callable>(_value);
  }

  bool expression::is_exception() const
  {
    return std::holds_alternative<exception>(_value);
  }

  const atom *expression::get_atom() const
  {
    return is_atom() ? &std::get<atom>(_value) : nullptr;
  }

  atom *expression::get_atom()
  {
    return is_atom() ? &std::get<atom>(_value) : nullptr;
  }

  const shared_list *expression::get_list() const
  {
    return is_list() ? &std::get<shared_list>(_value) : nullptr;
  }

  shared_list *expression::get_list()
  {
    return is_list() ? &std::get<shared_list>(_value) : nullptr;
  }

  const eval_shared_list *expression::get_eval_list() const
  {
    return is_eval_list() ? &std::get<eval_shared_list>(_value) : nullptr;
  }

  eval_shared_list *expression::get_eval_list()
  {
    return is_eval_list() ? &std::get<eval_shared_list>(_value) : nullptr;
  }

  const callable *expression::get_callable() const
  {
    return is_callable() ? &std::get<callable>(_value) : nullptr;
  }

  callable *expression::get_callable()
  {
    return is_callable() ? &std::get<callable>(_value) : nullptr;
  }

  const exception *expression::get_exception() const
  {
    return is_exception() ? &std::get<exception>(_value) : nullptr;
  }

  exception *expression::get_exception()
  {
    return is_exception() ? &std::get<exception>(_value) : nullptr;
  }

  const atom &expression::as_atom() const { return std::get<atom>(_value); }

  atom &expression::as_atom() { return std::get<atom>(_value); }

  const shared_list &expression::as_list() const
  {
    return std::get<shared_list>(_value);
  }

  shared_list &expression::as_list() { return std::get<shared_list>(_value); }

  const eval_shared_list &expression::as_eval_list() const
  {
    return std::get<eval_shared_list>(_value);
  }

  eval_shared_list &expression::as_eval_list()
  {
    return std::get<eval_shared_list>(_value);
  }

  const callable &expression::as_callable() const
  {
    return std::get<callable>(_value);
  }

  callable &expression::as_callable() { return std::get<callable>(_value); }

  const exception &expression::as_exception() const
  {
    return std::get<exception>(_value);
  }

  exception &expression::as_exception() { return std::get<exception>(_value); }

  const string &type_name(const shared_list &)
  {
    const static string name = "list";
    return name;
  }

  string to_string(const eval_shared_list &list)
  {
    return "<EVAL " + to_string((const shared_list &)list) + ">";
  }

  const string &type_name(const eval_shared_list &)
  {
    const static string name = "<EVAL LIST>";
    return name;
  }

  string to_string(const uneval_shared_list &list)
  {
    return "<UNEVAL " + to_string((const shared_list &)list) + ">";
  }

  const string &type_name(const uneval_shared_list &)
  {
    const static string name = "<UNEVAL LIST>";
    return name;
  }

  string to_string(const exception &exc)
  {
    return "<EXCEPTION '" + exc.message + "'>";
  }

  const string &type_name(const exception &)
  {
    const static string name = "<EXCEPTION>";
    return name;
  }

  string to_string(const expression &expr)
  {
    return expr.visit([](auto &&a) { return to_string(a); });
  }

  const string &type_name(const expression &)
  {
    const static string name = "expression";
    return name;
  }

  string to_string(expression::null) { return "null"; }

  const string &type_name(expression::null)
  {
    const static string name = "null";
    return name;
  }

  const string &type_name(const eval_expr &)
  {
    const static string name = "<EVAL EXPRESSION>";
    return name;
  }

  const string &type_name(const uneval_expr &)
  {
    const static string name = "<UNEVAL EXPRESSION>";
    return name;
  }
}

bool operator>>(const lisk::expression &arg, lisk::expression &out)
{
  out = arg;
  return true;
}

bool operator>>(const lisk::expression &arg, lisk::exception &out)
{
  if (!arg.is_exception()) return false;

  out = arg.as_exception();
  return true;
}

bool operator>>(const lisk::expression &arg, lisk::uneval_expr &out)
{
  out = {arg};
  return true;
}

bool operator>>(const lisk::expression &arg, lisk::shared_list &out)
{
  if (!arg.is_list()) return false;

  out = arg.as_list();
  return true;
}

bool operator>>(const lisk::expression &arg, lisk::uneval_shared_list &out)
{
  if (!arg.is_list()) return false;

  out = {arg.as_list()};
  return true;
}
