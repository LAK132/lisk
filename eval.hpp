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

#ifndef LISK_EVAL_HPP
#define LISK_EVAL_HPP

#include "environment.hpp"
#include "expression.hpp"
#include "shared_list.hpp"
#include "string.hpp"

#include <tuple>

namespace lisk
{
  struct functor;
  struct lambda;

  struct uneval_expr : public expression {};
  struct uneval_list : public shared_list<expression> {};

  shared_list<expression> eval_all(shared_list<expression> l, environment &e);
  expression eval(const expression &exp, environment &e);

  template<typename T, typename U =
    std::remove_cv_t<std::remove_reference_t<T>>>
  string type_name();

  bool get_arg_as(const expression &in_expr, expression &out_arg);
  bool get_arg_as(const expression &in_expr, uneval_expr &out_arg);
  bool get_arg_as(const expression &in_expr, shared_list<expression> &out_arg);
  bool get_arg_as(const expression &in_expr, uneval_list &out_arg);
  bool get_arg_as(const expression &in_expr, callable &out_arg);
  bool get_arg_as(const expression &in_expr, functor &out_arg);
  bool get_arg_as(const expression &in_expr, lambda &out_arg);
  bool get_arg_as(const expression &in_expr, atom &out_arg);
  bool get_arg_as(const expression &in_expr, symbol &out_arg);
  bool get_arg_as(const expression &in_expr, string &out_arg);
  bool get_arg_as(const expression &in_expr, number &out_arg);
  bool get_arg_as(const expression &in_expr, uint_t &out_arg);
  bool get_arg_as(const expression &in_expr, sint_t &out_arg);
  bool get_arg_as(const expression &in_expr, real_t &out_arg);

  template<typename T>
  bool eval_arg_as(const expression &in_expr, environment &e,
                   T &out_arg)
  {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(!std::is_same_v<type, uneval_expr> &&
                  !std::is_same_v<type, uneval_list>,
                  "Cannot eval-as uneval types");
    return get_arg_as(eval(in_expr, e), out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          expression &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          uneval_expr &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          shared_list<expression> &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          uneval_list &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          callable &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          functor &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          lambda &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          atom &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          symbol &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          string &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          number &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          uint_t &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          sint_t &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          real_t &out_arg);

  template<typename ...ARGS>
  expression wrapper_function(void (*func)(), shared_list<expression> l,
                              environment &e);
}

#include "eval.inl"

#endif