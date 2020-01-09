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

  template<typename T>
  struct list_reader_traits;

  template<bool GET, bool EVAL>
  struct basic_list_reader_traits
  {
    static constexpr bool allow_get = GET;
    static constexpr bool allow_eval = EVAL;
  };

  template<>
  struct list_reader_traits<atom::nil>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<atom>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<callable>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<functor>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<lambda>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<shared_list>
  : public basic_list_reader_traits<false, true> {};

  template<>
  struct list_reader_traits<eval_shared_list>
  : public basic_list_reader_traits<false, true> {};

  template<>
  struct list_reader_traits<uneval_shared_list>
  : public basic_list_reader_traits<true, false> {};

  template<>
  struct list_reader_traits<expression>
  : public basic_list_reader_traits<false, true> {};

  template<>
  struct list_reader_traits<eval_expr>
  : public basic_list_reader_traits<false, true> {};

  template<>
  struct list_reader_traits<uneval_expr>
  : public basic_list_reader_traits<true, false> {};

  template<>
  struct list_reader_traits<symbol>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<number>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<uint_t>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<sint_t>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<real_t>
  : public basic_list_reader_traits<true, true> {};

  template<>
  struct list_reader_traits<string>
  : public basic_list_reader_traits<true, true> {};

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