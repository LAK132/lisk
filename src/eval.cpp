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

#include "lisk/eval.hpp"

#include "lisk/functor.hpp"
#include "lisk/lambda.hpp"

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
    if (exp.is_null())
    {
      // Expr was the empty list, which evaluates to nil.
      return atom::nil{};
    }
    else if (exp.is_eval_list())
    {
      if (!allow_tail_eval) return exp;

      auto result = exp;

      for (callable c;
           result.is_eval_list() &&
           result.as_eval_list().list.value() >> c;
           result = eval(c({}, e, false), e, false));

      return result;
    }
    else if (symbol sym; exp >> sym)
    {
      return e[sym];
    }
    else if (atom a; exp >> a)
    {
      return a;
    }
    else if (shared_list l; exp >> l)
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
      else if (subexp >> sym)
      {
        return e[sym];
      }
      else if (subexp >> a)
      {
        return a;
      }
      else if (shared_list l2; subexp >> l2)
      {
        // :TODO: Comment on why we would ever end up here?
        return l2;
      }
      else if (callable c; subexp >> c)
      {
        return c(l.next(), e, allow_tail_eval);
      }
      else if (exception exc; subexp >> exc)
      {
        return exc;
      }
      else
      {
        return exception{
          "Failed to eval sub-expression '" + to_string(l.value()) +
          "' of '" + to_string(exp) + "', got '" + to_string(subexp) +
          "', expected a symbol, atom or callable"
        };
      }
    }
    else if (exception exc; exp >> exc)
    {
      return exc;
    }
    else
    {
      return exception{
        "Failed to eval expression '" + to_string(exp) + "' type '" +
        exp.visit([](auto &&a) { return type_name(a); }) + "'"
      };
    }
  }
}
