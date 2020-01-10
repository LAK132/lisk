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

#include "debug.hpp"

#include <variant>

namespace lisk
{
  struct expression;

  using shared_list = basic_shared_list<expression>;

  struct eval_shared_list { shared_list list; };
  struct uneval_shared_list { shared_list list; };

  struct expression
  {
    struct null {};

    std::variant<null, atom, eval_shared_list, shared_list, callable> _value;

    expression() = default;
    expression(const expression &expr) = default;
    expression(expression &&expr) = default;

    expression &operator=(const expression &expr) = default;
    expression &operator=(expression &&expr) = default;

    expression(null);
    expression(const atom &a);
    expression(atom::nil);
    expression(const shared_list &list);
    expression(const eval_shared_list &list);
    expression(const callable &c);

    expression &operator=(null);
    expression &operator=(const atom &a);
    expression &operator=(atom::nil);
    expression &operator=(const shared_list &list);
    expression &operator=(const eval_shared_list &list);
    expression &operator=(const callable &c);

    bool is_null() const;
    bool is_atom() const;
    bool is_list() const;
    bool is_eval_list() const;
    bool is_callable() const;

    inline bool empty() const { return is_null(); }
    inline operator bool() const { return !is_null(); }

    const atom *get_atom() const;
    atom *get_atom();

    const shared_list *get_list() const;
    shared_list *get_list();

    const eval_shared_list *get_eval_list() const;
    eval_shared_list *get_eval_list();

    const callable *get_callable() const;
    callable *get_callable();

    const atom &as_atom() const;
    atom &as_atom();

    const shared_list &as_list() const;
    shared_list &as_list();

    const eval_shared_list &as_eval_list() const;
    eval_shared_list &as_eval_list();

    const callable &as_callable() const;
    callable &as_callable();

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda) const { return std::visit(lambda, _value); }

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda) { return std::visit(lambda, _value); }
  };

  struct eval_expr { expression expr; };
  struct uneval_expr { expression expr; };

  const string &type_name(const shared_list &);

  string to_string(const eval_shared_list &list);
  const string &type_name(const eval_shared_list &);

  string to_string(const uneval_shared_list &list);
  const string &type_name(const uneval_shared_list &);

  string to_string(expression::null);
  const string &type_name(expression::null);

  string to_string(const expression &expr);
  const string &type_name(const expression &);

  // string to_string(const eval_expr &expr);
  const string &type_name(const eval_expr &);

  // string to_string(const uneval_expr &expr);
  const string &type_name(const uneval_expr &);
}

bool operator>>(const lisk::expression &arg, lisk::expression &out);
bool operator>>(const lisk::expression &arg, lisk::uneval_expr &out);
bool operator>>(const lisk::expression &arg, lisk::shared_list &out);
bool operator>>(const lisk::expression &arg, lisk::uneval_shared_list &out);

// These are here instead of in pointer.hpp because of include ordering issues.
template<typename T>
bool operator>>(const lisk::expression &arg, T *&out);
template<typename T>
bool operator>>(const lisk::expression &arg, const T *&out);
template<typename T>
bool operator>>(const lisk::expression &arg, std::shared_ptr<T> &out);

#include "expression.inl"

#endif
