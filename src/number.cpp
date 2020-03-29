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

#include "lisk/number.hpp"

#include "lisk/expression.hpp"

#include <variant>

namespace lisk
{
  number::number(uint_t u)
  {
    _value.emplace<uint_t>(u);
  }

  number::number(sint_t s)
  {
    _value.emplace<sint_t>(s);
  }

  number::number(real_t r)
  {
    _value.emplace<real_t>(r);
  }

  number &number::operator=(uint_t u)
  {
    _value.emplace<uint_t>(u);
    return *this;
  }

  number &number::operator=(sint_t s)
  {
    _value.emplace<sint_t>(s);
    return *this;
  }

  number &number::operator=(real_t r)
  {
    _value.emplace<real_t>(r);
    return *this;
  }

  bool number::is_uint() const
  {
    return std::holds_alternative<uint_t>(_value);
  }

  bool number::is_sint() const
  {
    return std::holds_alternative<sint_t>(_value);
  }

  bool number::is_real() const
  {
    return std::holds_alternative<real_t>(_value);
  }

  const uint_t *number::get_uint() const
  {
    if (is_uint())
      return &std::get<uint_t>(_value);
    else
      return nullptr;
  }

  uint_t *number::get_uint()
  {
    if (is_uint())
      return &std::get<uint_t>(_value);
    else
      return nullptr;
  }

  const sint_t *number::get_sint() const
  {
    if (is_sint())
      return &std::get<sint_t>(_value);
    else
      return nullptr;
  }

  sint_t *number::get_sint()
  {
    if (is_sint())
      return &std::get<sint_t>(_value);
    else
      return nullptr;
  }

  const real_t *number::get_real() const
  {
    if (is_real())
      return &std::get<real_t>(_value);
    else
      return nullptr;
  }

  real_t *number::get_real()
  {
    if (is_real())
      return &std::get<real_t>(_value);
    else
      return nullptr;
  }

  const uint_t &number::as_uint() const
  {
    return std::get<uint_t>(_value);
  }

  uint_t &number::as_uint()
  {
    return std::get<uint_t>(_value);
  }

  const sint_t &number::as_sint() const
  {
    return std::get<sint_t>(_value);
  }

  sint_t &number::as_sint()
  {
    return std::get<sint_t>(_value);
  }

  const real_t &number::as_real() const
  {
    return std::get<real_t>(_value);
  }

  real_t &number::as_real()
  {
    return std::get<real_t>(_value);
  }

  string to_string(const number &num)
  {
    if (std::holds_alternative<uint_t>(num._value))
      return lisk::to_string(std::get<uint_t>(num._value));

    if (std::holds_alternative<sint_t>(num._value))
      return lisk::to_string(std::get<sint_t>(num._value));

    if (std::holds_alternative<real_t>(num._value))
      return lisk::to_string(std::get<real_t>(num._value));

    return "<NaN>";
  }

  const string &type_name(const number &)
  {
    const static string name = "number";
    return name;
  }

  string to_string(uint_t num)
  {
    return std::to_string(num);
  }

  const string &type_name(uint_t)
  {
    const static string name = "uint";
    return name;
  }

  string to_string(sint_t num)
  {
    return (num >= 0 ? "+" : "") + std::to_string(num);
  }

  const string &type_name(sint_t)
  {
    const static string name = "sint";
    return name;
  }

  string to_string(real_t num)
  {
    return (num >= 0.0 ? "+" : "") + std::to_string(num);
  }

  const string &type_name(real_t)
  {
    const static string name = "real";
    return name;
  }
}

bool operator>>(const lisk::expression &arg, lisk::number &out)
{
  if (!arg.is_atom() || !arg.as_atom().is_number()) return false;

  out = arg.as_atom().as_number();
  return true;
}

bool operator>>(const lisk::expression &arg, lisk::uint_t &out)
{
  if (!arg.is_atom() || !arg.as_atom().is_number() ||
      !arg.as_atom().as_number().is_uint()) return false;

  out = arg.as_atom().as_number().as_uint();
  return true;
}

bool operator>>(const lisk::expression &arg, lisk::sint_t &out)
{
  if (!arg.is_atom() || !arg.as_atom().is_number() ||
      !arg.as_atom().as_number().is_sint()) return false;

  out = arg.as_atom().as_number().as_sint();
  return true;
}

bool operator>>(const lisk::expression &arg, lisk::real_t &out)
{
  if (!arg.is_atom() || !arg.as_atom().is_number() ||
      !arg.as_atom().as_number().is_real()) return false;

  out = arg.as_atom().as_number().as_real();
  return true;
}

lisk::number operator+(lisk::number A, lisk::number B)
{
  return std::visit([](auto &&A, auto &&B) { return lisk::number(A + B); },
                    A._value, B._value);
}

lisk::number operator-(lisk::number A, lisk::number B)
{
  return std::visit([](auto &&A, auto &&B) { return lisk::number(A - B); },
                    A._value, B._value);
}

lisk::number operator*(lisk::number A, lisk::number B)
{
  return std::visit([](auto &&A, auto &&B) { return lisk::number(A * B); },
                    A._value, B._value);
}

lisk::number operator/(lisk::number A, lisk::number B)
{
  return std::visit([](auto &&A, auto &&B) { return lisk::number(A / B); },
                    A._value, B._value);
}

lisk::number &operator+=(lisk::number &A, lisk::number B)
{
  return A = A + B;
}

lisk::number &operator-=(lisk::number &A, lisk::number B)
{
  return A = A - B;
}

lisk::number &operator*=(lisk::number &A, lisk::number B)
{
  return A = A * B;
}

lisk::number &operator/=(lisk::number &A, lisk::number B)
{
  return A = A / B;
}
