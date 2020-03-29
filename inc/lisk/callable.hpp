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

#ifndef LISK_CALLABLE_HPP
#define LISK_CALLABLE_HPP

#include "shared_list.hpp"

#include <memory>
#include <variant>

namespace lisk
{
  struct expression;
  struct environment;
  struct lambda;
  struct functor;

  struct callable
  {
    using value_type = std::variant<lambda, functor>;
    std::shared_ptr<value_type> _value;

    callable() = default;
    callable(const callable &c) = default;
    callable(callable &&c) = default;

    callable &operator=(const callable &c) = default;
    callable &operator=(callable &&c) = default;

    callable(const lambda &l);
    callable(const functor &f);

    callable &operator=(const lambda &l);
    callable &operator=(const functor &f);

    bool is_null() const;
    bool is_lambda() const;
    bool is_functor() const;

    inline bool empty() const { return is_null(); }
    inline operator bool() const { return !is_null(); }

    const lambda *get_lambda() const;
    lambda *get_lambda();

    const functor *get_functor() const;
    functor *get_functor();

    const lambda &as_lambda() const;
    lambda &as_lambda();

    const functor &as_functor() const;
    functor &as_functor();

    expression operator()(basic_shared_list<expression> l,
                          environment &e,
                          bool allow_tail_eval) const;
  };

  string to_string(const callable &c);
  const string &type_name(const callable &);
}

bool operator>>(const lisk::expression &arg, lisk::callable &out);

#endif