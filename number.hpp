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

#ifndef LISK_NUMBER_HPP
#define LISK_NUMBER_HPP

#include "string.hpp"

#include <variant>

namespace lisk
{
  struct expression;

  using uint_t = unsigned long long;
  using sint_t = signed long long;
  using real_t = long double;

  struct number
  {
    std::variant<uint_t, sint_t, real_t> _value;

    number() = default;
    number(const number &n) = default;

    number &operator=(const number &n) = default;

    number(uint_t u);
    number(sint_t s);
    number(real_t r);

    number &operator=(uint_t u);
    number &operator=(sint_t s);
    number &operator=(real_t r);

    bool is_uint() const;
    bool is_sint() const;
    bool is_real() const;

    const uint_t *get_uint() const;
    uint_t *get_uint();

    const sint_t *get_sint() const;
    sint_t *get_sint();

    const real_t *get_real() const;
    real_t *get_real();

    const uint_t &as_uint() const;
    uint_t &as_uint();

    const sint_t &as_sint() const;
    sint_t &as_sint();

    const real_t &as_real() const;
    real_t &as_real();

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda) const { return std::visit(lambda, _value); }

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda) { return std::visit(lambda, _value); }
  };

  string to_string(const number &num);
  const string &type_name(const number &);

  string to_string(uint_t num);
  const string &type_name(uint_t);

  string to_string(sint_t num);
  const string &type_name(sint_t);

  string to_string(real_t num);
  const string &type_name(real_t);
}

bool operator>>(const lisk::expression &arg, lisk::number &out);
bool operator>>(const lisk::expression &arg, lisk::uint_t &out);
bool operator>>(const lisk::expression &arg, lisk::sint_t &out);
bool operator>>(const lisk::expression &arg, lisk::real_t &out);

lisk::number operator+(lisk::number A, lisk::number B);
lisk::number operator-(lisk::number A, lisk::number B);
lisk::number operator*(lisk::number A, lisk::number B);
lisk::number operator/(lisk::number A, lisk::number B);
lisk::number &operator+=(lisk::number &A, lisk::number B);
lisk::number &operator-=(lisk::number &A, lisk::number B);
lisk::number &operator*=(lisk::number &A, lisk::number B);
lisk::number &operator/=(lisk::number &A, lisk::number B);

#endif
