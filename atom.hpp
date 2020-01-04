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

#ifndef LISK_ATOM_HPP
#define LISK_ATOM_HPP

#include "number.hpp"
#include "string.hpp"

#include <variant>

namespace lisk
{
  struct atom
  {
    struct nil {};

    std::variant<nil, symbol, string, number> _value;

    atom() = default;
    atom(const atom &a) = default;
    atom(atom &&a) = default;

    atom &operator=(const atom &expr) = default;
    atom &operator=(atom &&expr) = default;

    atom(const nil);
    atom(const symbol &sym);
    atom(const string &str);
    atom(const number &num);

    atom &operator=(nil);
    atom &operator=(const symbol &sym);
    atom &operator=(const string &str);
    atom &operator=(const number &num);

    bool is_nil() const;
    bool is_symbol() const;
    bool is_string() const;
    bool is_number() const;

    const symbol *get_symbol() const;
    symbol *get_symbol();

    const string *get_string() const;
    string *get_string();

    const number *get_number() const;
    number *get_number();

    const symbol &as_symbol() const;
    symbol &as_symbol();

    const string &as_string() const;
    string &as_string();

    const number &as_number() const;
    number &as_number();

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda) const { return std::visit(lambda, _value); }

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda) { return std::visit(lambda, _value); }
  };

  string to_string(atom::nil);
  string to_string(const atom &a);
}

#endif
