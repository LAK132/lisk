#ifndef LISK_FUNCTOR_HPP
#define LISK_FUNCTOR_HPP

#include "shared_list.hpp"

#include <variant>

namespace lisk
{
  struct environment;
  struct expression;

  template<typename FUNC>
  struct function_signature;

  template<typename R, typename... ARGS>
  struct function_signature<R (*)(ARGS...)>
  {
    using return_type = R;
    using arguments   = std::tuple<ARGS...>;
  };

  template<typename R, typename... ARGS>
  struct function_signature<R(ARGS...)>
  {
    using return_type = R;
    using arguments   = std::tuple<ARGS...>;
  };

  template<typename FUNC>
  using function_return_t = typename function_signature<FUNC>::return_type;

  template<typename FUNC>
  using function_arguments_t = typename function_signature<FUNC>::arguments;

  template<typename TUPLE>
  struct as_functor_arguments;

  template<typename... ARGS>
  struct as_functor_arguments<std::tuple<environment &, bool, ARGS...>>
  {
    using arguments = std::tuple<ARGS...>;
  };

  template<typename TUPLE>
  using as_functor_arguments_t =
    typename as_functor_arguments<TUPLE>::arguments;

  typedef std::pair<lisk::expression, size_t> (*functor)(
    basic_shared_list<expression>, environment &, bool);

#define LISK_FUNCTOR_WRAPPER(F)                                               \
  static_cast<lisk::functor>(                                                 \
    [](lisk::shared_list l,                                                   \
       lisk::environment &e,                                                  \
       bool allow_tail) -> std::pair<lisk::expression, size_t>                \
    {                                                                         \
      static_assert(                                                          \
        std::is_same_v<                                                       \
          std::tuple_element_t<0, lisk::function_arguments_t<decltype(F)>>,   \
          lisk::environment &>);                                              \
      static_assert(                                                          \
        std::is_same_v<                                                       \
          std::tuple_element_t<1, lisk::function_arguments_t<decltype(F)>>,   \
          bool>);                                                             \
      static_assert(std::is_same_v<lisk::function_return_t<decltype(F)>,      \
                                   lisk::expression>);                        \
      using arguments_t = lisk::as_functor_arguments_t<                       \
        lisk::function_arguments_t<decltype(F)>>;                             \
      arguments_t args;                                                       \
      lisk::exception exc;                                                    \
      if (!lisk::get_or_eval_arg_as(l, e, allow_tail, exc, args))             \
        return {exc, 0};                                                      \
      else                                                                    \
        return {                                                              \
          std::apply(                                                         \
            F, std::tuple_cat(std::forward_as_tuple(e, allow_tail), args)),   \
          std::tuple_size_v<arguments_t>};                                    \
    })

  string to_string(functor f);
  const string &type_name(functor);
}

bool operator>>(const lisk::expression &arg, lisk::functor &out);

#endif
