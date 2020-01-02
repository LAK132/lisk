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

#include "../lisk.hpp"
#include "../lisk.cpp"

#include "debug.hpp"
#include "debug.cpp"

#include <iostream>

bool running = true;

lisk::expr exit(lisk::env &e)
{
  running = false;
  return lisk::expr{lisk::atom{lisk::nill}};
}

int main(int argc, char **argv)
{
  using namespace std::string_literals;

  lisk::env default_env = lisk::builtin::default_env();

  default_env.define_functor("exit", exit);

  while (running)
  {
    std::cout << "lisk> ";
    std::string string;
    std::getline(std::cin, string);
    if (!std::cin.good())
    {
      std::cout << "\nlisk$ Goodbye!\n";
      break;
    }
    const auto tokens = lisk::tokenise(string);
    const auto expr   = lisk::parse(tokens);
    const auto eval   = lisk::eval(expr, default_env);
    const auto result = lisk::to_string(eval);
    std::cout << "lisk$ " << result << "\n";
  }
  std::cout << "\n";
}
