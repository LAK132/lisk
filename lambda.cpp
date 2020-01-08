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

#include "lambda.hpp"

#include "debug.hpp"
#include "strconv/tostring.hpp"

namespace lisk
{
  lambda::lambda(shared_list l, environment &e, bool allow_tail_eval)
  : captured_env(environment::extends(e))
  {
    if (shared_list arg1, arg2;
        get_arg_as(l.value(), arg1) && get_arg_as(l.next().value(), arg2))
    {
      params = arg1.clone();

      size_t param_index = 0;
      for (auto &node : params)
      {
        if (symbol s; get_or_eval_arg_as(node.value, e, allow_tail_eval, s))
        {
          node.value = s;
          ++param_index;
        }
        else
        {
          ERROR("Failed to get symbol " << std::dec << param_index <<
                " from '" << to_string(arg1) << "'");
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
    // lak::scoped_indenter indent("lambda");
    auto new_env = environment::extends(captured_env);

    size_t param_index = 0;
    for (const auto &node : params)
    {
      if (!l)
      {
        ERROR("Too few parameters in '" << to_string(l) << "' to call lambda, "
              "expected parameters are '" << to_string(params) << "'");
        return expression::null{};
      }

      if (symbol s; get_or_eval_arg_as(node.value, e, allow_tail_eval, s))
      {
        new_env.define_expr(s, eval(l.value(), e, allow_tail_eval));
      }
      else
      {
        ERROR("Failed to get symbol " << std::dec << param_index << " "
              "from '" << to_string(node.value) << "'");
        ERROR("Failed to evaluate '" << to_string(*this) << "'");
        return expression::null{};
      }

      ++param_index;
      ++l;
    }

    auto result = eval(exp, new_env, allow_tail_eval);

    if (allow_tail_eval && result.is_eval_list())
      result = eval(result, new_env, allow_tail_eval);

    return result;
  }

  string to_string(const lambda &l)
  {
    return "(lambda " + to_string(l.params) + " " + to_string(l.exp) + ")";
  }
}
