#include "lisk/lambda.hpp"

namespace lisk
{
  lambda::lambda(shared_list l, environment &e, bool allow_tail_eval)
  : captured_env(environment::extends(e))
  {
    if (shared_list arg1, arg2; l.value() >> arg1 && l.next().value() >> arg2)
    {
      params = arg1.clone();

      list_reader reader(params, e, allow_tail_eval);

      size_t param_index = 0;
      for (auto &node : params)
      {
        if (symbol s; reader >> s)
        {
          node.value = s;
          ++param_index;
        }
        else
        {
          // :TODO: This really should throw an exception. Maybe this should
          // now evaluate to a lambda that returns an exception on call?
          params.clear();
          return;
        }
      }

      exp = arg2;
    }
  }

  std::pair<expression, size_t> lambda::operator()(shared_list l,
                                                   environment &e,
                                                   bool allow_tail_eval) const
  {
    auto new_env = environment::extends(captured_env);

    list_reader reader(params, e, allow_tail_eval);

    size_t param_index = 0;
    for (const auto &node : l)
    {
      auto evaled = reader.list;
      if (!reader)
      {
        if (param_index == 0)
        {
          return {
            exception{"Too many arguments to call lambda, expected none"}, 0};
        }
        else
        {
          return {
            exception{
              "Too many arguments to call lambda, expected params are '" +
              to_string(params) + "'"},
            0};
        }
      }
      else if (symbol s; reader >> s)
      {
        new_env.define_expr(s, eval(node.value, e, allow_tail_eval));
        ++param_index;
      }
      else
      {
        return {exception{"Failed to get symbol " +
                          std::to_string(param_index) + " from '" +
                          to_string(node.value) + "' for '" +
                          to_string(evaled) + "'"},
                0};
      }
    }

    if (reader)
    {
      return {exception{"Too few parameters in '" + to_string(l) +
                        "' to call lambda, expected parameters are '" +
                        to_string(params) + "'"},
              0};
    }

    return {eval(exp, new_env, allow_tail_eval), param_index};
  }

  string to_string(const lambda &l)
  {
    return "(lambda " + to_string(l.params) + " " + to_string(l.exp) + ")";
  }

  const string &type_name(const lambda &)
  {
    const static string name = "lambda";
    return name;
  }
}

bool operator>>(const lisk::expression &arg, lisk::lambda &out)
{
  if (!arg.is_callable() || !arg.as_callable().is_lambda()) return false;

  out = arg.as_callable().as_lambda();
  return true;
}
