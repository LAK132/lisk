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

#ifndef LISK_STRING_HPP
#define LISK_STRING_HPP

#include <string>

namespace lisk
{
  struct expression;

  struct symbol : public std::string
  {
    symbol()               = default;
    symbol(const symbol &) = default;
    symbol(symbol &&)      = default;

    symbol(const std::string &value) : std::string(value) {}
    symbol(std::string &&value) : std::string(std::move(value)) {}
    symbol(const char *value) : std::string(value) {}

    symbol &operator=(const symbol &) = default;
    symbol &operator=(symbol &&) = default;

    symbol &operator=(const std::string &value)
    {
      std::string::operator=(value);
      return *this;
    }
    symbol &operator=(std::string &&value)
    {
      std::string::operator=(std::move(value));
      return *this;
    }
    symbol &operator=(const char *value)
    {
      std::string::operator=(value);
      return *this;
    }

    using std::string::operator[];
  };

  struct string : public std::string
  {
    string()               = default;
    string(const string &) = default;
    string(string &&)      = default;

    string(const std::string &value) : std::string(value) {}
    string(std::string &&value) : std::string(std::move(value)) {}
    string(const char *value) : std::string(value) {}

    string &operator=(const string &) = default;
    string &operator=(string &&) = default;

    string &operator=(const std::string &value)
    {
      std::string::operator=(value);
      return *this;
    }
    string &operator=(std::string &&value)
    {
      std::string::operator=(std::move(value));
      return *this;
    }
    string &operator=(const char *value)
    {
      std::string::operator=(value);
      return *this;
    }

    using std::string::operator[];
  };

  string to_string(const symbol &sym);
  const string &type_name(const symbol &);

  string to_string(const string &str);
  const string &type_name(const string &);
}

bool operator>>(const lisk::expression &arg, lisk::symbol &out);
bool operator>>(const lisk::expression &arg, lisk::string &out);

template<>
struct std::hash<lisk::symbol> : public std::hash<std::string>
{
};
template<>
struct std::hash<lisk::string> : public std::hash<std::string>
{
};

#endif