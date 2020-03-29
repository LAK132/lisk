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

#include "lisk/lisk.hpp"

#include <iostream>

namespace lisk
{
  const std::regex numeric_regex(
    "(?:([\\-\\+])?(\\d+)(\\.\\d+)?)|"
    "(?:([\\-\\+])?0x([a-f\\d]+)(\\.[a-f\\d]+)?)|"
    "(?:([\\-\\+])?0b([01]+))",
    std::regex_constants::ECMAScript | std::regex_constants::icase |
      std::regex_constants::optimize);

  bool is_whitespace(const char c)
  {
    switch (c)
    {
      case ' ':
      case '\n':
      case '\r':
      case '\t': return true;
      default: return false;
    }
  }

  bool is_bracket(const char c)
  {
    switch (c)
    {
      case '(':
      case ')':
      case '[':
      case ']':
      case '{':
      case '}': return true;
      default: return false;
    }
  }

  bool is_numeric(const string &token)
  {
    return std::regex_match(token, numeric_regex);
  }

  bool is_nil(const expression &expr)
  {
    return (expr.is_list() && expr.as_list().value().is_null()) ||
           (expr.is_atom() && expr.as_atom().is_nil());
  }

  bool is_null(const expression &expr) { return expr.is_null(); }

  std::vector<string> tokenise(const string &str, size_t *chars_used)
  {
    std::vector<string> result;
    string buffer;
    size_t chars_read = 0;

    auto begin_next = [&] {
      if (!buffer.empty()) result.emplace_back(std::move(buffer));
      buffer.clear();
      if (chars_used) *chars_used = chars_read;
    };

    bool in_string          = false;
    bool is_string_escaping = false;
    bool in_comment         = false;
    char string_char        = 0;
    for (const auto c : str)
    {
      ++chars_read;
      if (in_comment)
      {
        if (c == '\n') in_comment = false;
      }
      else if (in_string)
      {
        if (is_string_escaping)
        {
          if (c == 'n')
            buffer += '\n';
          else if (c == 'r')
            buffer += '\r';
          else if (c == 't')
            buffer += '\t';
          else if (c == '0')
            buffer += '\0';
          else
            buffer += c;
          is_string_escaping = false;
        }
        else if (c == '\\')
        {
          is_string_escaping = true;
        }
        else
        {
          buffer += c;
          if (c == string_char)
          {
            in_string = false;
            begin_next();
          }
        }
      }
      else if (c == ';')
      {
        in_comment = true;
      }
      else if (c == '"' || c == '\'')
      {
        buffer += c;
        in_string          = true;
        is_string_escaping = false;
        string_char        = c;
      }
      else if (is_whitespace(c))
      {
        if (!buffer.empty()) begin_next();
      }
      else if (is_bracket(c))
      {
        if (!buffer.empty()) begin_next();
        buffer += c;
        begin_next();
      }
      else
      {
        buffer += c;
      }
    }

    return result;
  }

  std::vector<string> root_tokenise(const string &str, size_t *chars_used)
  {
    auto result = tokenise(str, chars_used);
    if (result.empty()) return {};

    result.reserve(result.size() + 3);
    result.insert(result.begin(), {"(", "begin"});
    result.push_back(")");
    return result;
  }

  number parse_number(const string &token)
  {
    number result;

    std::smatch match;

    if (!std::regex_match(token, match, numeric_regex) || match.size() != 9)
      return std::numeric_limits<real_t>::signaling_NaN();

    if (match[2].matched)
    {
      if (match[3].matched)
      {
        // floating point
        result = std::stold(match[2].str() + match[3].str());
      }
      else if (match[1].matched)
      {
        // signed int
        result = static_cast<sint_t>(
          std::stoll(match[1].str() + match[2].str(), nullptr, 10));
      }
      else
      {
        // unsigned int
        result = static_cast<uint_t>(std::stoull(match[2].str(), nullptr, 10));
      }
    }
    else if (match[5].matched)
    {
      if (match[6].matched)
      {
        // floating point
        result = std::stold("0x" + match[2].str() + match[3].str());
      }
      else if (match[4].matched)
      {
        // signed int
        result = static_cast<sint_t>(
          std::stoll(match[4].str() + match[5].str(), nullptr, 16));
      }
      else
      {
        // unsigned int
        result = static_cast<uint_t>(std::stoull(match[5].str(), nullptr, 16));
      }
    }
    else if (match[8].matched)
    {
      if (match[7].matched)
      {
        // signed int
        result = static_cast<sint_t>(
          std::stoll(match[7].str() + match[8].str(), nullptr, 2));
      }
      else
      {
        // unsigned int
        result = static_cast<uint_t>(std::stoull(match[8].str(), nullptr, 2));
      }
    }
    else
      return std::numeric_limits<real_t>::signaling_NaN();

    return result;
  }

  string parse_string(const string &token)
  {
    if (token.size() == 2) return string{};
    return string{token.substr(1, token.size() - 2)};
  }

  expression parse(const std::vector<string> &tokens)
  {
    shared_list root;
    std::vector<std::vector<shared_list>> stack;

    auto push_element = [&]() -> shared_list {
      if (stack.empty()) return {};
      // Get the previous nill element.
      shared_list old_element = stack.back().back();
      // If this element holds null_t then no values have been added to this
      // scope yet, so return immediately.
      if (!old_element) return old_element;
      // Else if the last element already has a value, add a new element after
      // it
      auto new_element = shared_list::create();
      // Set the new nill element as the next element from the previous nill
      // element.
      old_element.set_next(new_element);
      // Push the new nill element into the stack scope.
      stack.back().emplace_back(new_element);
      // Return the old nill element so a value can be added to it.
      return new_element;
    };

    auto push_scope = [&]() {
      // Create the root element for the new stack.
      auto scope    = shared_list::create();
      scope.value() = expression::null{};

      // If this is the first scope, make sure to mark it as the root.
      // Else this is a nested scope, push it as a value onto the parent scope.
      if (stack.empty())
        root = scope;
      else
        push_element().value() = scope;

      // Push the new scope onto the stack.
      stack.emplace_back();
      stack.back().emplace_back(scope);
    };

    auto pop_scope = [&]() {
      // Pop the scope off the stack.
      stack.pop_back();
    };

    for (const auto &token : tokens)
    {
      const char c = token.front();
      if (c == '(')
      {
        push_scope();
      }
      else if (c == ')')
      {
        pop_scope();
      }
      else
      {
        expression value;

        if (c == '"')
          value = atom{parse_string(token)};
        else if (token == "nil")
          value = atom{atom::nil{}};
        else if (token == "true")
          value = atom{true};
        else if (token == "false")
          value = atom{false};
        else if (is_numeric(token))
          value = atom{parse_number(token)};
        else
          value = atom{symbol(token)};

        if (stack.empty())
          return value;
        else
          push_element().value() = value;
      }
    }

    return expression{root};
  }

  expression eval_string(const string &str, environment &env)
  {
    return eval(parse(tokenise(str)), env, true);
  }

  expression root_eval_string(const string &str, environment &env)
  {
    return eval(parse(root_tokenise(str)), env, true);
  }

  bool reader::iterator::operator==(sentinel) const { return !ref; }

  bool reader::iterator::operator!=(sentinel) const { return ref; }

  lisk::expression reader::iterator::operator*()
  {
    return lisk::eval(lisk::parse(ref.tokens.front()),
                      ref.env,
                      ref.allow_tail_eval);
  }

  reader::iterator &reader::iterator::operator++()
  {
    ref.tokens.pop_front();
    return *this;
  }

  reader::reader(const lisk::environment e, bool allow_tail)
  : env(e), allow_tail_eval(allow_tail)
  {
  }

  void reader::clear()
  {
    string_buffer.clear();
    token_buffer.clear();
    tokens.clear();
  }

  reader::operator bool() const { return tokens.size() > 0; }

  reader::iterator reader::begin() { return {*this}; }

  reader::iterator::sentinel reader::end() const { return {}; }

  reader &reader::operator+=(const lisk::string &str)
  {
    string_buffer += str;

    // Tokenise as much of the buffer as we can.
    size_t chars_used;
    auto new_tokens = lisk::tokenise(string_buffer, &chars_used);
    string_buffer.erase(string_buffer.begin(),
                        string_buffer.begin() + chars_used);

    // Push the new tokens into the reader's token buffer.
    token_buffer.reserve(token_buffer.size() + new_tokens.size());
    for (auto &token : new_tokens) token_buffer.emplace_back(std::move(token));

    // Push the groups of tokens into the reader. These should either be
    // individual atoms or complete lists.
    size_t scope_count = 0;
    for (auto it = token_buffer.begin(); it != token_buffer.end(); ++it)
    {
      if (*it == "(")
      {
        ++scope_count;
      }
      else if (*it == ")")
      {
        --scope_count;
      }
      if (scope_count == 0)
      {
        auto begin = token_buffer.begin();
        auto end   = it + 1;

        if (tokens.empty() || !tokens.back().empty()) tokens.emplace_back();
        tokens.back().reserve(end - begin);

        for (auto it2 = begin; it2 != end; ++it2)
          tokens.back().emplace_back(std::move(*it2));

        token_buffer.erase(begin, end);

        it = token_buffer.begin();
        if (it == token_buffer.end()) break;
      }
    }

    return *this;
  }

  namespace builtin
  {
    expression list_env(environment &env, bool allow_tail)
    {
      auto root = shared_list::create();

      shared_list previous = root;
      shared_list l        = root;
      for (const auto &node : env._map)
      {
        for (const auto &[key, value] : node.value)
        {
          auto entry         = shared_list::create();
          entry.value()      = atom{key};
          entry.next_value() = value;

          l.value() = entry;
          l.set_next(shared_list::create());

          previous = l++;
        }
      }
      previous.clear_next();

      return root;
    }

    expression null_check(environment &env, bool allow_tail, expression exp)
    {
      return atom{is_null(exp)};
    }

    expression nil_check(environment &env, bool allow_tail, expression exp)
    {
      return atom{is_nil(exp)};
    }

    expression zero_check(environment &env, bool allow_tail, number num)
    {
      return atom{num.visit([](auto &&n) -> bool { return n == 0; })};
    }

    // expression equal_check(shared_list l, environment &env)
    // {
    //   if (eval(l.value(), e) == eval(l.next().value(), e))
    //     return expression{atom{number{uint_t{1U}}}};
    //   else
    //     return atom::nil{};
    // }

    expression conditional(environment &env,
                           bool allow_tail,
                           bool b,
                           uneval_expr cond,
                           uneval_expr alt)
    {
      return b ? eval(cond.expr, env, allow_tail)
               : eval(alt.expr, env, allow_tail);
    }

    expression define(environment &env,
                      bool allow_tail,
                      symbol sym,
                      expression exp)
    {
      env.define_expr(sym, exp);
      return atom::nil{};
    }

    expression evaluate(environment &env, bool allow_tail, expression exp)
    {
      return eval(exp, env, allow_tail);
    }

    expression begin(shared_list l, environment &env, bool allow_tail)
    {
      // Evaluate every element in the list and return the result of the last.
      expression result;
      for (const auto &node : l)
      {
        result = eval(node.value, env, allow_tail);
      }
      return result;
    }

    expression repeat(environment &env,
                      bool allow_tail,
                      uint_t count,
                      uneval_expr exp)
    {
      while (count-- > 0) eval(exp.expr, env, allow_tail);
      return atom::nil{};
    }

    expression repeat_while(environment &env, bool allow_tail, uneval_expr exp)
    {
      while (!is_nil(eval(exp.expr, env, allow_tail)))
        ;
      return atom::nil{};
    }

    expression foreach (environment &env,
                        bool allow_tail,
                        symbol sym,
                        shared_list iterlist,
                        uneval_expr exp)
    {
      for (const auto &node : iterlist)
      {
        environment new_env = env;
        new_env.define_expr(sym, node.value);
        eval(exp.expr, new_env, allow_tail);
      }
      return atom::nil{};
    }

    expression map(environment &env,
                   bool allow_tail,
                   shared_list iterlist,
                   uneval_expr exp)
    {
      auto subexp = eval(exp.expr, env, allow_tail);
      if (functor f; subexp >> f)
      {
        auto arg    = shared_list::create();
        auto result = shared_list::create();
        auto end    = result;
        for (const auto &node : iterlist)
        {
          arg.value()      = node.value;
          end.next_value() = f(arg, env, allow_tail);
          ++end;
        }
        return ++result;
      }
      else if (lambda lf; subexp >> lf)
      {
        auto arg    = shared_list::create();
        auto result = shared_list::create();
        auto end    = result;
        for (const auto &node : iterlist)
        {
          arg.value()      = node.value;
          end.next_value() = lf(arg, env, allow_tail);
          ++end;
        }
        return ++result;
      }
      else
        return subexp.visit([](auto &&a) {
          return type_error("Map error", a, "a function or lambda");
        });
    }

    expression tail_call(shared_list list, environment &env, bool allow_tail)
    {
      // grandp   parent   env
      // * ------ * ------ *
      //  `.
      //    `.
      //      `* ------ * tail_env (env clone)
      //    parent (clone)
      environment tail_env = env.clone(2);
      // grandp   parent   env
      // * ------ * ------ *
      //  `.
      //    `.
      //      `* tail_env (squashed env->parent clone)
      tail_env.squash(2);

      auto result = shared_list::create();

      // (lambda () list)
      shared_list lambda_list;
      lambda_list.value()      = shared_list::create();
      lambda_list.next_value() = list.value();
      lambda lmbd(lambda_list, tail_env, allow_tail);
      result.value() = callable(lmbd);

      // Return an eval_shared_list containing a lambda that immediately
      // evaluates our list param.
      return eval_shared_list{result};
    }

    expression car(environment &env, bool allow_tail, shared_list l)
    {
      return l.value();
    }

    expression cdr(environment &env, bool allow_tail, shared_list l)
    {
      return l.next();
    }

    expression cons(environment &env,
                    bool allow_tail,
                    expression exp,
                    shared_list l)
    {
      shared_list result;
      result.value() = exp;
      result.set_next(l);
      return result;
    }

    expression join(shared_list l, environment &env, bool allow_tail)
    {
      shared_list first;
      shared_list end;

      if (eval(l.value(), env, allow_tail) >> first)
      {
        end = first.last();
      }
      else
      {
        return type_error("Join error", l.value(), "a list");
      }

      for (const auto &node : l.next())
      {
        if (shared_list next; eval(node.value, env, allow_tail) >> next)
        {
          end.set_next(next);
          end = next.last();
        }
        else
        {
          return type_error("Join error", node.value, "a list");
        }
      }

      return first;
    }

    expression range_list(environment &env,
                          bool allow_tail,
                          number start,
                          uint_t count,
                          number step)
    {
      auto result = shared_list::create();
      auto end    = result;
      for (uint_t i = 0; i < count; ++i)
      {
        end.next_value() = atom{start + (step * number{i})};
        ++end;
      }
      return ++result;
    }

    expression make_list(shared_list l, environment &env, bool allow_tail)
    {
      return eval_all(l, env, allow_tail);
    }

    expression make_lambda(shared_list l, environment &env, bool allow_tail)
    {
      return callable(lambda(l, env, allow_tail));
    }

    expression make_uint(environment &env, bool allow_tail, expression exp)
    {
      auto to_uint = [](auto &&n) -> number { return uint_t(n); };
      if (number n; exp >> n)
        return atom{n.visit(to_uint)};
      else if (string s; exp >> s)
        return atom{parse_number(s).visit(to_uint)};
      else
        return atom{parse_number(to_string(exp)).visit(to_uint)};
    }

    expression make_sint(environment &env, bool allow_tail, expression exp)
    {
      auto to_sint = [](auto &&n) -> number { return sint_t(n); };
      if (number n; exp >> n)
        return atom{n.visit(to_sint)};
      else if (string s; exp >> s)
        return atom{parse_number(s).visit(to_sint)};
      else
        return atom{parse_number(to_string(exp)).visit(to_sint)};
    }

    expression make_real(environment &env, bool allow_tail, expression exp)
    {
      auto to_real = [](auto &&n) -> number { return (real_t)(n); };
      if (number n; exp >> n)
        return atom{n.visit(to_real)};
      else if (string s; exp >> s)
        return atom{parse_number(s).visit(to_real)};
      else
        return atom{parse_number(to_string(exp)).visit(to_real)};
    }

    expression make_string(environment &env, bool allow_tail, expression exp)
    {
      if (string s; exp >> s)
        return expression(s);
      else
        return expression(string{to_string(exp)});
    }

    expression read_string(shared_list, environment &, bool)
    {
      std::string str;
      std::getline(std::cin, str);
      if (std::cin.good()) return expression{atom{string(std::move(str))}};
      return atom::nil{};
    }

    expression parse_string(environment &env, bool allow_tail, string str)
    {
      return parse(tokenise(str));
    }

    expression print_string(shared_list l, environment &env, bool allow_tail)
    {
      expression result = eval(l.value(), env, allow_tail);
      // If the list evaluates to a pure string, then print it verbatim.
      // Otherwise to_string the result.
      if (string str; result >> str)
        std::cout << str;
      else
        std::cout << to_string(result);
      return atom::nil{};
    }

    expression print_line(shared_list l, environment &env, bool allow_tail)
    {
      // No arguments, just print a newline.
      if (is_nil(l)) std::cout << "\n";

      expression result = eval(l.value(), env, allow_tail);
      // If the list evaluates to a pure string, then print it verbatim.
      // Otherwise to_string the result.
      if (string str; result >> str)
        std::cout << str << "\n";
      else
        std::cout << to_string(result) << "\n";
      return atom::nil{};
    }

    expression add(shared_list l, environment &env, bool allow_tail)
    {
      if (!l) return atom::nil{};

      number result;

      if (number n; eval(l.value(), env, allow_tail) >> n)
      {
        result = n;
      }
      else
      {
        return type_error("Add error", l.value(), "a number");
      }

      for (const auto &it : l.next())
      {
        if (number n; eval(it.value, env, allow_tail) >> n)
        {
          result += n;
        }
        else
        {
          return type_error("Add error", l.value(), "a number");
        }
      }

      return expression{atom{result}};
    }

    expression sub(shared_list l, environment &env, bool allow_tail)
    {
      if (!l) return atom::nil{};

      number result;

      if (number n; eval(l.value(), env, allow_tail) >> n)
      {
        result = n;
      }
      else
      {
        return type_error("Sub error", l.value(), "a number");
      }

      for (const auto &it : l.next())
      {
        if (number n; eval(it.value, env, allow_tail) >> n)
        {
          result -= n;
        }
        else
        {
          return type_error("Sub error", l.value(), "a number");
        }
      }

      return expression{atom{result}};
    }

    expression mul(shared_list l, environment &env, bool allow_tail)
    {
      if (!l) return atom::nil{};

      number result;

      if (number n; eval(l.value(), env, allow_tail) >> n)
      {
        result = n;
      }
      else
      {
        return type_error("Mul error", l.value(), "a number");
      }

      for (const auto &it : l.next())
      {
        if (number n; eval(it.value, env, allow_tail) >> n)
        {
          result *= n;
        }
        else
        {
          return type_error("Mul error", l.value(), "a number");
        }
      }

      return expression{atom{result}};
    }

    expression div(shared_list l, environment &env, bool allow_tail)
    {
      if (!l) return atom::nil{};

      number result;

      if (number n; eval(l.value(), env, allow_tail) >> n)
      {
        result = n;
      }
      else
      {
        return type_error("Div error", l.value(), "a number");
      }

      for (const auto &it : l.next())
      {
        if (number n; eval(it.value, env, allow_tail) >> n)
        {
          result /= n;
        }
        else
        {
          return type_error("Div error", l.value(), "a number");
        }
      }

      return expression{atom{result}};
    }

    environment default_env()
    {
      environment e;

      e.define_atom("pi", atom(number(3.14159L)));

      e.define_functor("env", &list_env);
      e.define_functor("null?", &null_check);
      e.define_functor("nil?", &nil_check);
      e.define_functor("zero?", &zero_check);
      // e.define_functor("eq?",     &equal_check);
      e.define_functor("if", &conditional);
      e.define_functor("define", &define);
      e.define_functor("eval", &evaluate);
      e.define_functor("begin", &begin);
      e.define_functor("repeat", &repeat);
      e.define_functor("while", &repeat_while);
      e.define_functor("foreach", &foreach);
      e.define_functor("map", &map);
      e.define_functor("tail", &tail_call);

      e.define_functor("car", &car);
      e.define_functor("cdr", &cdr);
      e.define_functor("cons", &cons);
      e.define_functor("join", &join);

      e.define_functor("range", &range_list);
      e.define_functor("list", &make_list);
      e.define_functor("lambda", &make_lambda);
      e.define_functor("uint", &make_uint);
      e.define_functor("sint", &make_sint);
      e.define_functor("real", &make_real);
      e.define_functor("string", &make_string);

      e.define_functor("read", &read_string);
      e.define_functor("parse", &parse_string);
      e.define_functor("print", &print_string);
      e.define_functor("println", &print_line);

      e.define_functor("+", &add);
      e.define_functor("-", &sub);
      e.define_functor("*", &mul);
      e.define_functor("/", &div);

      return e;
    }
  }
}