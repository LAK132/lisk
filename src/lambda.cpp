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

#include "lisk/lambda.hpp"

namespace lisk
{
  lambda::lambda(shared_list l, environment &e, bool allow_tail_eval)
  : captured_env(environment::extends(e))
  {
    if (shared_list arg1, arg2;
        l.value() >> arg1 &&
        l.next().value() >> arg2)
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

  expression lambda::operator()(shared_list l,
                                environment &e,
                                bool allow_tail_eval) const
  {
    auto new_env = environment::extends(captured_env);

    list_reader reader(params, e, allow_tail_eval);

    size_t param_index = 0;
    for (const auto &node : l)
    {
      auto evaled = reader.list;
      if (symbol s; reader >> s)
      {
        new_env.define_expr(s, eval(node.value, e, allow_tail_eval));
        ++param_index;
      }
      else
      {
        return exception{
          "Failed to get symbol " + std::to_string(param_index) + " "
          "from '" + to_string(node.value) + "' "
          "for '" + to_string(evaled) + "'"
        };
      }
    }

    if (reader)
    {
      return exception{
        "Too few parameters in '" + to_string(l) +
        "' to call lambda, expected parameters are '" +
        to_string(params) + "'"
      };
    }

    return eval(exp, new_env, allow_tail_eval);
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
