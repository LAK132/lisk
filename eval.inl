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

  template<typename T>
  bool list_reader::operator>>(T &out)
  {
    static_assert(list_reader_traits<T>::allow_get ||
                  list_reader_traits<T>::allow_eval,
                  "Type must be get-able or eval-able");

    if (!list) return false;

    if constexpr (list_reader_traits<T>::allow_get &&
                  list_reader_traits<T>::allow_eval)
    {
      if (list.value() >> out ||
          eval(list.value(), env, allow_tail_eval) >> out)
      {
        ++list;
        return true;
      }
    }
    else if constexpr (list_reader_traits<T>::allow_get)
    {
      if (list.value() >> out)
      {
        ++list;
        return true;
      }
    }
    else if constexpr (list_reader_traits<T>::allow_eval)
    {
      if (eval(list.value(), env, allow_tail_eval) >> out)
      {
        ++list;
        return true;
      }
    }
    return false;
  }

  template<typename ...TYPES, size_t ...I>
  bool _get_or_eval_arg_as(shared_list in_list, environment &e,
                           bool allow_tail, std::tuple<TYPES...> &out_arg,
                           std::index_sequence<I...>)
  {
    std::tuple<std::remove_cv_t<TYPES>...> result;

    list_reader reader(in_list, e, allow_tail);

    [[maybe_unused]] auto _get_or_eval = [&](auto &&element, auto i) -> bool
    {
      if (!(reader >> element))
      {
        ERROR("Failed to evaluate element " << i << " "
              "'" << to_string(reader.list.value()) << "' "
              "of '" << to_string(reader.list) << "', "
              "expected type '" << type_name(element) << "'");
        return false;
      }
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
  bool get_or_eval_arg_as(shared_list in_list, environment &e, bool allow_tail,
                          std::tuple<TYPES...> &out_arg)
  {
    return _get_or_eval_arg_as(in_list, e, allow_tail, out_arg,
                               std::index_sequence_for<TYPES...>{});
  }

  template<typename ...ARGS>
  expression wrapper_function(void (*func)(), shared_list l,
                              environment &e, bool allow_tail)
  {
    std::tuple<ARGS...> args;
    if (!get_or_eval_arg_as(l, e, allow_tail, args))
      return expression::null{};
    else
      return std::apply((expression (*)(environment &, bool, ARGS...))func,
                        std::tuple_cat(std::forward_as_tuple(e, allow_tail),
                                       args));
  }
}
