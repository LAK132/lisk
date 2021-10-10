#ifndef LISK_LAMBDA_HPP
#define LISK_LAMBDA_HPP

#include "environment.hpp"
#include "eval.hpp"
#include "expression.hpp"
#include "shared_list.hpp"

namespace lisk
{
  struct lambda
  {
    shared_list params;
    shared_list exp;
    environment captured_env;

    lambda()               = default;
    lambda(const lambda &) = default;
    lambda &operator=(const lambda &) = default;

    lambda(shared_list l, environment &e, bool allow_tail_eval);

    std::pair<expression, size_t> operator()(shared_list l,
                                             environment &e,
                                             bool allow_tail_eval) const;
  };

  string to_string(const lambda &l);
  const string &type_name(const lambda &);
}

bool operator>>(const lisk::expression &arg, lisk::lambda &out);

#endif
