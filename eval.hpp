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

  shared_list eval_all(shared_list l, environment &e, bool allow_tail_eval);
  expression eval(const expression &exp, environment &e, bool allow_tail_eval);

  template<typename T, typename U =
    std::remove_cv_t<std::remove_reference_t<T>>>
  string type_name();

  bool get_arg_as(const expression &in_expr, expression &out_arg);
  bool get_arg_as(const expression &in_expr, uneval_expr &out_arg);
  bool get_arg_as(const expression &in_expr, shared_list &out_arg);
  bool get_arg_as(const expression &in_expr, uneval_shared_list &out_arg);
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
  bool eval_arg_as(const expression &in_expr, environment &e, bool allow_tail,
                   T &out_arg)
  {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(!std::is_same_v<type, uneval_expr> &&
                  !std::is_same_v<type, uneval_shared_list>,
                  "Cannot eval-as uneval types");
    return get_arg_as(eval(in_expr, e, allow_tail), out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, expression &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, uneval_expr &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, shared_list &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, uneval_shared_list &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, callable &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, functor &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, lambda &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, atom &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, symbol &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, string &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, number &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, uint_t &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, sint_t &out_arg);
  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, real_t &out_arg);

  template<typename ...ARGS>
  expression wrapper_function(void (*func)(), shared_list l,
                              environment &e, bool allow_tail);
}

#include "eval.inl"

#endif