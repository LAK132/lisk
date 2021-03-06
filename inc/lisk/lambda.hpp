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

    expression operator()(shared_list l,
                          environment &e,
                          bool allow_tail_eval) const;
  };

  string to_string(const lambda &l);
  const string &type_name(const lambda &);
}

bool operator>>(const lisk::expression &arg, lisk::lambda &out);

#endif