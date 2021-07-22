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

#include "lisk/callable.hpp"

#include "lisk/expression.hpp"
#include "lisk/functor.hpp"
#include "lisk/lambda.hpp"

namespace lisk
{
  callable::callable(const lambda &l)
  {
    _value.emplace<std::shared_ptr<lambda>>(std::make_shared<lambda>(l));
  }

  callable::callable(const functor &f)
  {
    _value.emplace<std::shared_ptr<functor>>(std::make_shared<functor>(f));
  }

  callable &callable::operator=(const lambda &l)
  {
    _value.emplace<std::shared_ptr<lambda>>(std::make_shared<lambda>(l));
    return *this;
  }

  callable &callable::operator=(const functor &f)
  {
    _value.emplace<std::shared_ptr<functor>>(std::make_shared<functor>(f));
    return *this;
  }

  bool callable::is_null() const { return !is_lambda() && !is_functor(); }

  bool callable::is_lambda() const { return get_lambda() != nullptr; }

  bool callable::is_functor() const { return get_functor() != nullptr; }

  const lambda *callable::get_lambda() const
  {
    if (std::holds_alternative<std::shared_ptr<lambda>>(_value))
      return std::get<std::shared_ptr<lambda>>(_value).get();
    else
      return nullptr;
  }

  lambda *callable::get_lambda()
  {
    if (std::holds_alternative<std::shared_ptr<lambda>>(_value))
      return std::get<std::shared_ptr<lambda>>(_value).get();
    else
      return nullptr;
  }

  const functor *callable::get_functor() const
  {
    if (std::holds_alternative<std::shared_ptr<functor>>(_value))
      return std::get<std::shared_ptr<functor>>(_value).get();
    else
      return nullptr;
  }

  functor *callable::get_functor()
  {
    if (std::holds_alternative<std::shared_ptr<functor>>(_value))
      return std::get<std::shared_ptr<functor>>(_value).get();
    else
      return nullptr;
  }

  const lambda &callable::as_lambda() const
  {
    return *std::get<std::shared_ptr<lambda>>(_value);
  }

  lambda &callable::as_lambda()
  {
    return *std::get<std::shared_ptr<lambda>>(_value);
  }

  const functor &callable::as_functor() const
  {
    return *std::get<std::shared_ptr<functor>>(_value);
  }

  functor &callable::as_functor()
  {
    return *std::get<std::shared_ptr<functor>>(_value);
  }

  expression callable::operator()(shared_list l,
                                  environment &e,
                                  bool allow_tail_eval) const
  {
    if (is_null()) return expression::null{};

    auto result = std::visit(
      [&](auto &&func) { return (*func)(l, e, allow_tail_eval); }, _value);

    if (allow_tail_eval && result.is_eval_list())
      result = eval(result, e, allow_tail_eval);

    return result;
  }

  string to_string(const callable &c)
  {
    return std::visit(
      [](auto &&func)
      { return func ? to_string(*func) : to_string(expression::null{}); },
      c._value);
  }

  const string &type_name(const callable &)
  {
    const static string name = "callable";
    return name;
  }
}

bool operator>>(const lisk::expression &arg, lisk::callable &out)
{
  if (!arg.is_callable()) return false;

  out = arg.as_callable();
  return true;
}
