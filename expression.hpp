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

#ifndef LISK_EXPRESSION_HPP
#define LISK_EXPRESSION_HPP

#include "atom.hpp"
#include "callable.hpp"
#include "string.hpp"
#include "shared_list.hpp"

#include <variant>

namespace lisk
{
  struct expression
  {
    struct null {};

    std::variant<null, atom, shared_list<expression>, callable> _value;

    expression() = default;
    expression(const expression &expr) = default;
    expression(expression &&expr) = default;

    expression &operator=(const expression &expr) = default;
    expression &operator=(expression &&expr) = default;

    expression(const null);
    expression(const atom &a);
    expression(const shared_list<expression> &list);
    expression(const callable &c);

    expression &operator=(null);
    expression &operator=(const atom &a);
    expression &operator=(const shared_list<expression> &list);
    expression &operator=(const callable &c);

    bool is_null() const;
    bool is_atom() const;
    bool is_list() const;
    bool is_callable() const;

    inline bool empty() const { return is_null(); }
    inline operator bool() const { return !is_null(); }

    const atom *get_atom() const;
    atom *get_atom();

    const shared_list<expression> *get_list() const;
    shared_list<expression> *get_list();

    const callable *get_callable() const;
    callable *get_callable();

    const atom &as_atom() const;
    atom &as_atom();

    const shared_list<expression> &as_list() const;
    shared_list<expression> &as_list();

    const callable &as_callable() const;
    callable &as_callable();

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda) const { return std::visit(lambda, _value); }

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda) { return std::visit(lambda, _value); }
  };

  string to_string(const expression &expr);
}

#endif
