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

  using list = shared_list<expression>;

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
    expression list_env(environment &e);
    expression null_check(environment &e, expression exp);
    expression nil_check(environment &e, expression exp);
    // expr equal_check(list l, environment &e);
    expression conditional(environment &e, expression exp, uneval_expr cond,
                           uneval_expr alt);
    expression define(environment &e, symbol sym, expression exp);
    expression evaluate(environment &e, expression exp);
    expression begin(list l, environment &e);
    expression repeat(environment &e, uint_t count, uneval_expr exp);
    expression repeat_while(environment &e, uneval_expr exp);
    expression foreach(environment &e, symbol sym, list iterlist,
                       uneval_expr exp);
    expression map(environment &e, list iterlist, uneval_expr exp);

    expression car(environment &e, list l);
    expression cdr(environment &e, list l);
    expression cons(environment &e, expression exp, list l);
    expression join(list l, environment &e);

    expression range_list(environment &e, number start, uint_t count,
                          number step);
    expression make_list(list l, environment &e);
    expression make_lambda(list l, environment &e);
    expression make_uint(environment &e, expression exp);
    expression make_sint(environment &e, expression exp);
    expression make_real(environment &e, expression exp);
    expression make_string(environment &e, expression exp);

    expression read_string(list, environment &);
    expression parse_string(environment &e, string str);
    expression print_string(list l, environment &e);
    expression print_line(list l, environment &e);

    expression add(list l, environment &e);
    expression sub(list l, environment &e);
    expression mul(list l, environment &e);
    expression div(list l, environment &e);

    environment default_env();
  };
}

#endif