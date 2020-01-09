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

#include "functor.hpp"
#include "lambda.hpp"

#include "debug.hpp"

namespace lisk
{
  shared_list eval_all(shared_list l, environment &e, bool allow_tail_eval)
  {
    auto result = shared_list::create();
    auto end = result;
    for (const auto &node : l)
    {
      end.set_next(shared_list::create());
      ++end;
      end.value() = eval(node.value, e, allow_tail_eval);
    }
    return ++result;
  }

  expression eval(const expression &exp, environment &e, bool allow_tail_eval)
  {
    if (exp.is_eval_list())
    {
      auto result = exp;
      while (allow_tail_eval && result.is_eval_list() &&
             result.as_eval_list().list.value().is_callable())
      {
        result = eval(
          result.as_eval_list().list.value().as_callable()({}, e, false),
          e, false);
      }
      return result;
    }
    else if (symbol sym; get_arg_as(exp, sym))
    {
      return e[sym];
    }
    else if (atom a; get_arg_as(exp, a))
    {
      return a;
    }
    else if (shared_list l; get_arg_as(exp, l))
    {
      // If we're about do do a function call, this should evalutate the symbol
      // to the relevant function pointer.
      auto subexp = eval(l.value(), e, allow_tail_eval);

      // This is the empty list or nil atom.
      if ((subexp.is_list() && subexp.as_list().value().is_null()) ||
          (subexp.is_atom() && subexp.as_atom().is_nil()))
      {
        return atom::nil{};
      }
      else if (get_arg_as(subexp, sym))
      {
        return e[sym];
      }
      else if (get_arg_as(subexp, a))
      {
        return a;
      }
      else if (shared_list l2; get_arg_as(subexp, l2))
      {
        // :TODO: Comment on why we would ever end up here?
        return l2;
      }
      else if (callable c; get_arg_as(subexp, c))
      {
        return c(l.next(), e, allow_tail_eval);
      }
      else
      {
        ERROR("Failed to eval sub-expression '" << to_string(l.value()) <<
              "' of '" << to_string(exp) <<
              "', got '" << to_string(subexp) << "', expected a symbol, "
              "atom or callable");
        return expression::null{};
      }
    }
    else
    {
      ERROR("Failed to eval expression '" << to_string(exp) << "' type '"
            << exp.visit([](auto &&a){return type_name<decltype(a)>();})
            << "'");
      return expression::null{};
    }
  }

  bool get_arg_as(const expression &in_expr, expression &out_arg)
  {
    out_arg = in_expr;
    return true;
  }

  bool get_arg_as(const expression &in_expr, uneval_expr &out_arg)
  {
    out_arg = {in_expr};
    return true;
  }

  bool get_arg_as(const expression &in_expr, shared_list &out_arg)
  {
    if (in_expr.is_list())
    {
      out_arg = in_expr.as_list();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, uneval_shared_list &out_arg)
  {
    if (in_expr.is_list())
    {
      out_arg = {in_expr.as_list()};
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, callable &out_arg)
  {
    if (in_expr.is_callable())
    {
      out_arg = in_expr.as_callable();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, functor &out_arg)
  {
    if (in_expr.is_callable() && in_expr.as_callable().is_functor())
    {
      out_arg = in_expr.as_callable().as_functor();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, lambda &out_arg)
  {
    if (in_expr.is_callable() && in_expr.as_callable().is_lambda())
    {
      out_arg = in_expr.as_callable().as_lambda();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, atom &out_arg)
  {
    if (in_expr.is_atom())
    {
      out_arg = in_expr.as_atom();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, symbol &out_arg)
  {
    if (in_expr.is_atom() && in_expr.as_atom().is_symbol())
    {
      out_arg = in_expr.as_atom().as_symbol();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, string &out_arg)
  {
    if (in_expr.is_atom() && in_expr.as_atom().is_string())
    {
      out_arg = in_expr.as_atom().as_string();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, number &out_arg)
  {
    if (in_expr.is_atom() && in_expr.as_atom().is_number())
    {
      out_arg = in_expr.as_atom().as_number();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, uint_t &out_arg)
  {
    if (in_expr.is_atom() &&
        in_expr.as_atom().is_number() &&
        in_expr.as_atom().as_number().is_uint())
    {
      out_arg = in_expr.as_atom().as_number().as_uint();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, sint_t &out_arg)
  {
    if (in_expr.is_atom() &&
        in_expr.as_atom().is_number() &&
        in_expr.as_atom().as_number().is_sint())
    {
      out_arg = in_expr.as_atom().as_number().as_sint();
      return true;
    }
    return false;
  }

  bool get_arg_as(const expression &in_expr, real_t &out_arg)
  {
    if (in_expr.is_atom() &&
        in_expr.as_atom().is_number() &&
        in_expr.as_atom().as_number().is_real())
    {
      out_arg = in_expr.as_atom().as_number().as_real();
      return true;
    }
    return false;
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, expression &out_arg)
  {
    return eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, uneval_expr &out_arg)
  {
    return get_arg_as(in_expr, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, shared_list &out_arg)
  {
    return eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, uneval_shared_list &out_arg)
  {
    return get_arg_as(in_expr, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, callable &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, functor &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, lambda &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, atom &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, symbol &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, string &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, number &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, uint_t &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, sint_t &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }

  bool get_or_eval_arg_as(const expression &in_expr, environment &e,
                          bool allow_tail, real_t &out_arg)
  {
    return
      get_arg_as(in_expr, out_arg) ||
      eval_arg_as(in_expr, e, allow_tail, out_arg);
  }
}
