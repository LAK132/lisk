#ifndef LISK_CALLABLE_HPP
#define LISK_CALLABLE_HPP

#include "functor.hpp"
#include "shared_list.hpp"

#include <memory>
#include <variant>

namespace lisk
{
  struct expression;
  struct environment;
  struct lambda;

  struct callable
  {
    using value_type =
      std::variant<std::shared_ptr<lambda>, std::shared_ptr<functor>>;
    value_type _value;

    // :TODO: add a way to track the number of arguments that a callable will
    // take, and if it's variadic then make it return how many it read.

    callable()                  = default;
    callable(const callable &c) = default;
    callable(callable &&c)      = default;

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

    functor *get_functor() const;

    const lambda &as_lambda() const;
    lambda &as_lambda();

    functor as_functor() const;

    std::pair<expression, size_t> operator()(basic_shared_list<expression> l,
                                             environment &e,
                                             bool allow_tail_eval) const;
  };

  string to_string(const callable &c);
  const string &type_name(const callable &);
}

bool operator>>(const lisk::expression &arg, lisk::callable &out);

#endif
