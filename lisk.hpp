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

#ifndef LISK_HPP
#define LISK_HPP

#include "atom.hpp"
#include "callable.hpp"
#include "environment.hpp"
#include "eval.hpp"
#include "expression.hpp"
#include "functor.hpp"
#include "lambda.hpp"
#include "number.hpp"
#include "shared_list.hpp"

#include "debug.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <regex>
#include <string>
#include <tuple>
#include <variant>

namespace lisk
{
  extern const std::regex numeric_regex;

  bool is_whitespace(const char c);
  bool is_bracket(const char c);
  bool is_numeric(const string &token);

  bool is_nil(const expression &expr);
  bool is_null(const expression &expr);

  std::vector<string> tokenise(const string &str);

  number parse_number(const string &token);
  string parse_string(const string &token);
  expression parse(const std::vector<string> &tokens);

  namespace builtin
  {
    expression list_env(environment &env, bool allow_tail);
    expression null_check(environment &env, bool allow_tail, expression exp);
    expression nil_check(environment &env, bool allow_tail, expression exp);
    expression zero_check(environment &env, bool allow_tail, number num);
    // expr equal_check(shared_list l, environment &env);
    expression conditional(environment &env, bool allow_tail, expression exp,
                           uneval_expr cond, uneval_expr alt);
    expression define(environment &env, bool allow_tail, symbol sym,
                      expression exp);
    expression evaluate(environment &env, bool allow_tail, expression exp);
    expression begin(shared_list l, environment &env, bool allow_tail);
    expression repeat(environment &env, bool allow_tail, uint_t count,
                      uneval_expr exp);
    expression repeat_while(environment &env, bool allow_tail,
                            uneval_expr exp);
    expression foreach(environment &env, bool allow_tail, symbol sym,
                       shared_list iterlist, uneval_expr exp);
    expression map(environment &env, bool allow_tail, shared_list iterlist,
                   uneval_expr exp);
    expression tail_call(shared_list l, environment &env, bool allow_tail);

    expression car(environment &env, bool allow_tail, shared_list l);
    expression cdr(environment &env, bool allow_tail, shared_list l);
    expression cons(environment &env, bool allow_tail, expression exp,
                    shared_list l);
    expression join(shared_list l, environment &env, bool allow_tail);

    expression range_list(environment &env, bool allow_tail, number start,
                          uint_t count, number step);
    expression make_list(shared_list l, environment &env, bool allow_tail);
    expression make_lambda(shared_list l, environment &env, bool allow_tail);
    expression make_uint(environment &env, bool allow_tail, expression exp);
    expression make_sint(environment &env, bool allow_tail, expression exp);
    expression make_real(environment &env, bool allow_tail, expression exp);
    expression make_string(environment &env, bool allow_tail, expression exp);

    expression read_string(shared_list, environment &, bool allow_tail);
    expression parse_string(environment &env, bool allow_tail, string str);
    expression print_string(shared_list l, environment &env, bool allow_tail);
    expression print_line(shared_list l, environment &env, bool allow_tail);

    expression add(shared_list l, environment &env, bool allow_tail);
    expression sub(shared_list l, environment &env, bool allow_tail);
    expression mul(shared_list l, environment &env, bool allow_tail);
    expression div(shared_list l, environment &env, bool allow_tail);

    environment default_env();
  };
}

#endif