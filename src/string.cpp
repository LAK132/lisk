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

#include "lisk/string.hpp"

#include "lisk/expression.hpp"

namespace lisk
{
  string to_string(const symbol &sym)
  {
    return sym;
  }

  const string &type_name(const symbol &)
  {
    const static string name = "symbol";
    return name;
  }

  string to_string(const string &str)
  {
    auto result = str;
    auto replace = [](string &str, const char c, const char *cstr)
    {
      for (auto i = str.find(c); i != string::npos; i = str.find(c))
        str.replace(i, 1, cstr);
    };
    replace(result, '\n', "\\n");
    replace(result, '\r', "\\r");
    replace(result, '\t', "\\t");
    replace(result, '\0', "\\0");
    replace(result, '\"', "\\\"");

    return "\"" + result + "\"";
  }

  const string &type_name(const string &)
  {
    const static string name = "string";
    return name;
  }
}

bool operator>>(const lisk::expression &arg, lisk::symbol &out)
{
  if (!arg.is_atom() || !arg.as_atom().is_symbol()) return false;

  out = arg.as_atom().as_symbol();
  return true;
}

bool operator>>(const lisk::expression &arg, lisk::string &out)
{
  if (!arg.is_atom() || !arg.as_atom().is_string()) return false;

  out = arg.as_atom().as_string();
  return true;
}