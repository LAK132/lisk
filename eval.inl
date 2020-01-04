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

#include "eval.hpp"

#include "debug.hpp"

namespace lisk
{
  template<typename...>
  struct _false : std::bool_constant<false> {};

  template<typename T, typename U>
  string type_name()
  {
    if constexpr (std::is_same_v<U, expression>)
      return "expression";
    else if constexpr (std::is_same_v<U, uneval_expr>)
      return "<UNEVAL EXPRESSION>";
    else if constexpr (std::is_same_v<U, expression::null>)
      return "<NULL>";
    else if constexpr (std::is_same_v<U, shared_list<expression>>)
      return "list";
    else if constexpr (std::is_same_v<U, callable>)
      return "callable";
    else if constexpr (std::is_same_v<U, functor>)
      return "functor";
    else if constexpr (std::is_same_v<U, lambda>)
      return "lambda";
    else if constexpr (std::is_same_v<U, atom>)
      return "atom";
    else if constexpr (std::is_same_v<U, atom::nil>)
      return "nil";
    else if constexpr (std::is_same_v<U, symbol>)
      return "symbol";
    else if constexpr (std::is_same_v<U, string>)
      return "string";
    else if constexpr (std::is_same_v<U, number>)
      return "number";
    else if constexpr (std::is_same_v<U, uint_t>)
      return "uint";
    else if constexpr (std::is_same_v<U, sint_t>)
      return "sint";
    else if constexpr (std::is_same_v<U, real_t>)
      return "real";
    else static_assert(_false<T>{}, "Invalid type");
  }

  template<typename ...TYPES, size_t ...I>
  bool _get_or_eval_arg_as(shared_list<expression> in_list, environment &e,
                           std::tuple<TYPES...> &out_arg,
                           std::index_sequence<I...>)
  {
    std::tuple<std::remove_cv_t<TYPES>...> result;
    [[maybe_unused]] auto _get_or_eval = [&](auto &&element, auto i) -> bool
    {
      if (!get_or_eval_arg_as(in_list.value(), e, element))
      {
        ERROR("Failed to evaluate element " << i << " "
              "'" << to_string(in_list.value()) << "' "
              "of '" << to_string(in_list) << "', "
              "expected type '" << type_name<decltype(element)>() << "'");
        return false;
      }
      ++in_list;
      return true;
    };
    if ((_get_or_eval(std::get<I>(result), I) && ...))
    {
      out_arg = result;
      return true;
    }
    else return false;
  }

  template<typename ...TYPES>
  bool get_or_eval_arg_as(shared_list<expression> in_list, environment &e,
                          std::tuple<TYPES...> &out_arg)
  {
    return _get_or_eval_arg_as(in_list, e, out_arg,
                               std::make_index_sequence<sizeof...(TYPES)>{});
  }

  template<typename ...ARGS>
  expression wrapper_function(void (*func)(), shared_list<expression> l,
                              environment &e)
  {
    std::tuple<ARGS...> args;
    if (!get_or_eval_arg_as(l, e, args)) return expression{atom{atom::nil{}}};
    return std::apply((expression (*)(environment &, ARGS...))func,
                      std::tuple_cat(std::forward_as_tuple(e), args));
  }
}
