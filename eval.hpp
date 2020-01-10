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
#include "pointer.hpp"
#include "string.hpp"

#include <tuple>

namespace lisk
{
  struct functor;
  struct lambda;

  shared_list eval_all(shared_list l, environment &e, bool allow_tail_eval);
  expression eval(const expression &exp, environment &e, bool allow_tail_eval);

  template<typename T>
  struct list_reader_traits
  {
    // Default to allowing both get-ing and eval-ing, this results in less work
    // to get user defined types working with lisk. If the user wants to modify
    // this behaviour, they can still override these values (see
    // specialisations below)
    static constexpr bool allow_get = true;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<shared_list>
  {
    static constexpr bool allow_get = false;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<eval_shared_list>
  {
    static constexpr bool allow_get = false;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<expression>
  {
    static constexpr bool allow_get = false;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<eval_expr>
  {
    static constexpr bool allow_get = false;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<uneval_shared_list>
  {
    static constexpr bool allow_get = true;
    static constexpr bool allow_eval = false;
  };

  template<>
  struct list_reader_traits<uneval_expr>
  {
    static constexpr bool allow_get = true;
    static constexpr bool allow_eval = false;
  };

  struct list_reader
  {
    shared_list list;
    environment env;
    bool allow_tail_eval;

    list_reader(shared_list l, environment e, bool allow_tail)
    : list(l), env(e), allow_tail_eval(allow_tail) {}

    inline operator bool() const { return list; }

    template<typename T>
    bool operator>>(T &out);
  };

  template<typename ...ARGS>
  expression wrapper_function(void (*func)(), shared_list l,
                              environment &e, bool allow_tail);
}

#include "eval.inl"

#endif