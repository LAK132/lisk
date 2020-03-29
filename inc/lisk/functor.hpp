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

#ifndef LISK_FUNCTOR_HPP
#define LISK_FUNCTOR_HPP

#include "environment.hpp"
#include "eval.hpp"
#include "expression.hpp"
#include "shared_list.hpp"

#include <variant>

namespace lisk
{
  struct functor
  {
    struct basic_functor
    {
      using function_type = expression (*)(shared_list, environment &, bool);
      function_type function;
    };

    struct wrapped_functor
    {
      using function_type = void (*)();
      using wrapper_type  = expression (*)(function_type,
                                          shared_list,
                                          environment &,
                                          bool);
      wrapper_type wrapper;
      function_type function;
    };

    std::variant<std::monostate, basic_functor, wrapped_functor> _value;

    functor()                = default;
    functor(const functor &) = default;
    functor(functor &&)      = default;

    functor &operator=(const functor &) = default;
    functor &operator=(functor &&) = default;

    functor(basic_functor::function_type f);
    functor(wrapped_functor::wrapper_type w, wrapped_functor::function_type f);

    template<typename... ARGS>
    functor(expression (*f)(environment &, bool, ARGS...))
    : functor(&wrapper_function<ARGS...>, (void (*)())f)
    {
    }

    expression operator()(shared_list l,
                          environment &e,
                          bool allow_tail_eval) const;
  };

  string to_string(const functor &l);
  const string &type_name(const functor &);
}

bool operator>>(const lisk::expression &arg, lisk::functor &out);

#endif