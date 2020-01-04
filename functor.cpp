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

#include "functor.hpp"

#include "strconv/tostring.hpp"

namespace lisk
{
  functor::functor(basic_functor::function_type f)
  {
    _value.emplace<basic_functor>(basic_functor{f});
  }

  functor::functor(wrapped_functor::wrapper_type w,
                   wrapped_functor::function_type f)
  {
    _value.emplace<wrapped_functor>(wrapped_functor{w, f});
  }

  expression functor::operator()(shared_list<expression> l,
                                 environment &e) const
  {
    if (std::holds_alternative<basic_functor>(_value))
    {
      if (auto &basic = std::get<basic_functor>(_value); basic.function)
        return basic.function(l, e);
      else
        return expression::null{};
    }
    else if (std::holds_alternative<wrapped_functor>(_value))
    {
      if (auto &wrapped = std::get<wrapped_functor>(_value);
          wrapped.wrapper && wrapped.function)
        return wrapped.wrapper(wrapped.function, l, e);
      else
        return expression::null{};
    }
    else return expression::null{};
  }

  string to_string(const functor &f)
  {
    if (std::holds_alternative<functor::basic_functor>(f._value))
    {
      auto &basic = std::get<functor::basic_functor>(f._value);
      return "<builtin " +
        lak::to_string((uintptr_t)basic.function) + ">";
    }
    else if (std::holds_alternative<functor::wrapped_functor>(f._value))
    {
      auto &wrapped = std::get<functor::wrapped_functor>(f._value);
      return "<builtin " +
        lak::to_string((uintptr_t)wrapped.wrapper) + " " +
        lak::to_string((uintptr_t)wrapped.function) + ">";
    }
    else return to_string(expression::null{});
  }
}
