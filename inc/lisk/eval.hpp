#ifndef LISK_EVAL_HPP
#define LISK_EVAL_HPP

#include "environment.hpp"
#include "expression.hpp"
#include "functor.hpp"
#include "pointer.hpp"
#include "shared_list.hpp"
#include "string.hpp"

#include <tuple>

namespace lisk
{
  struct lambda;

  std::pair<shared_list, size_t> eval_all(shared_list l,
                                          environment &e,
                                          bool allow_tail_eval);
  expression eval(const expression &exp, environment &e, bool allow_tail_eval);

  template<typename T>
  struct list_reader_traits
  {
    // Default to allowing both get-ing and eval-ing, this results in less work
    // to get user defined types working with lisk. If the user wants to modify
    // this behaviour, they can still override these values (see
    // specialisations below)
    static constexpr bool allow_get  = true;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<shared_list>
  {
    static constexpr bool allow_get  = false;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<eval_shared_list>
  {
    static constexpr bool allow_get  = false;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<expression>
  {
    static constexpr bool allow_get  = false;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<eval_expr>
  {
    static constexpr bool allow_get  = false;
    static constexpr bool allow_eval = true;
  };

  template<>
  struct list_reader_traits<uneval_shared_list>
  {
    static constexpr bool allow_get  = true;
    static constexpr bool allow_eval = false;
  };

  template<>
  struct list_reader_traits<uneval_expr>
  {
    static constexpr bool allow_get  = true;
    static constexpr bool allow_eval = false;
  };

  struct list_reader
  {
    shared_list list;
    environment env;
    bool allow_tail_eval;

    list_reader(shared_list l, environment e, bool allow_tail)
    : list(l), env(e), allow_tail_eval(allow_tail)
    {
    }

    inline operator bool() const { return list; }

    template<typename T>
    bool operator>>(T &out);
  };
}

#include "eval.inl"

#endif
