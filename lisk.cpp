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

#include "lisk.hpp"
#include "strconv/tostring.hpp"
#include "debug.hpp"

namespace lisk
{
  const std::regex numeric_regex(
    "(?:([\\-\\+])?(\\d+)(\\.\\d+)?)|"
    "(?:([\\-\\+])?0x([a-f\\d]+)(\\.[a-f\\d]+)?)|"
    "(?:([\\-\\+])?0b([01]+))",
    std::regex_constants::ECMAScript |
    std::regex_constants::icase |
    std::regex_constants::optimize);

  nill_t nill;
  null_t null;

  number operator+(number A, number B)
  {
    return std::visit([](auto &&A, auto &&B) { return number(A + B); }, A, B);
  }

  number operator-(number A, number B)
  {
    return std::visit([](auto &&A, auto &&B) { return number(A - B); }, A, B);
  }

  number operator*(number A, number B)
  {
    return std::visit([](auto &&A, auto &&B) { return number(A * B); }, A, B);
  }

  number operator/(number A, number B)
  {
    return std::visit([](auto &&A, auto &&B) { return number(A / B); }, A, B);
  }

  number &operator+=(number &A, number B)
  {
    return A = A + B;
  }

  number &operator-=(number &A, number B)
  {
    return A = A - B;
  }

  number &operator*=(number &A, number B)
  {
    return A = A * B;
  }

  number &operator/=(number &A, number B)
  {
    return A = A / B;
  }

  std::shared_ptr<list::node_t> list::create()
  {
    return std::make_shared<node_t>();
  }

  list &list::operator++()
  {
    if (node) node = node->next;
    return *this;
  }

  list list::operator++(int)
  {
    list result = *this;
    operator++();
    return result;
  }

  expr list::value() const
  {
    if (node) return node->value;
    ERROR("Attempt to get expression value from empty list");
    return expr{atom{nill}};
  }

  list list::next(size_t n) const
  {
    auto result = node;
    while (result && n --> 0) result = result->next;
    return {result};
  }

  void list::set_value(const expr &e)
  {
    if (!node) node = create();
    node->value = e;
  }

  void list::set_next(list l)
  {
    if (!node) node = create();
    node->next = l.node;
  }

  void list::set_next_value(const expr &e)
  {
    if (!node) node = create();
    if (!node->next) node->next = create();
    node->next->value = e;
  }

  void list::clear()
  {
    node.reset();
  }

  void list::clear_next()
  {
    if (node) node->next.reset();
  }

  list list::clone() const
  {
    list result{list::create()};
    list end = result;
    for (const auto &node : *this)
    {
      end.set_next_value(node.value);
      ++end;
    }
    return ++result;
  }

  list list::begin() const
  {
    // If the value of this node is null, this is the empty list so return end.
    if (node && is_null(node->value))
      return end();
    else
      return *this;
  }

  list list::last() const
  {
    list result = *this;
    list n = result;
    while (n) result = n++;
    return result;
  }

  list list::end() const
  {
    return {nullptr};
  }

  bool list::operator==(const list &other) const
  {
    return node.get() == other.node.get();
  }

  bool list::operator!=(const list &other) const
  {
    return !operator==(other);
  }

  list::operator bool() const
  {
    return node && !is_null(node->value);
  }

  const list::node_t &list::operator*() const
  {
    return *node;
  }

  list::node_t &list::operator*()
  {
    return *node;
  }

  // const list::node_t *list::operator->() const
  // {
  //   return node.get();
  // }

  // list::node_t *list::operator->()
  // {
  //   return node.get();
  // }

  functor::functor(basic_functor::function_type f)
  : wrapped(false)
  {
    value.basic.function = f;
  }

  functor::functor(wrapped_functor::wrapper_type w,
                   wrapped_functor::function_type f)
  : wrapped(true)
  {
    value.wrapped.wrapper = w;
    value.wrapped.function = f;
  }

  expr functor::operator()(list l, env &e) const
  {
    if (wrapped)
    {
      if (value.wrapped.wrapper && value.wrapped.function)
        return value.wrapped.wrapper(value.wrapped.function, l, e);
      else
        return expr{atom{nill}};
    }
    else
    {
      if (value.basic.function)
        return value.basic.function(l, e);
      else
        return expr{atom{nill}};
    }
  }

  lambda::lambda(list l, env &e)
  {
    if (list arg1, arg2;
        get_arg_as(l.value(), arg1) && get_arg_as(l.next().value(), arg2))
    {
      params = arg1.clone();

      size_t param_index = 0;
      for (auto &node : params)
      {
        if (symbol s; get_or_eval_as(node.value, e, s))
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

  expr lambda::operator()(list l, env &e) const
  {
    env new_env = e;

    size_t param_index = 0;
    for (const auto &node : params)
    {
      if (!l)
      {
        ERROR("Too few parameters in '" << to_string(l) << "' to call lambda, "
              "expected parameters are '" << to_string(params) << "'")
        return expr{atom{nill}};
      }

      if (symbol s; get_or_eval_as(node.value, e, s))
      {
        new_env.define_expr(s, l.value());
      }
      else
      {
        ERROR("Failed to get symbol " << std::dec << param_index << " "
              "from '" << to_string(node.value) << "'");
        ERROR("Failed to evaluate '" << to_string(*this) << "'");
        return expr{atom{nill}};
      }

      ++param_index;
      ++l;
    }

    return eval(exp, new_env);
  }

  void env::define_expr(const symbol &s, const expr &e)
  {
    values[s] = e;
  }

  void env::define_atom(const symbol &s, const atom &a)
  {
    values[s] = a;
  }

  void env::define_list(const symbol &s, list l)
  {
    values[s] = l;
  }

  void env::define_functor(const symbol &s, functor f)
  {
    values[s] = f;
  }

  void env::define_lambda(const symbol &s, const lambda &l)
  {
    values[s] = l;
  }

  expr env::operator[](const symbol &s) const
  {
    if (auto it = values.find(s); it != values.end())
    {
      return it->second;
    }
    else
    {
      WARNING("Symbol '" << to_string(s) << "' not found");
      return atom{nill};
    }
  }

  bool is_whitespace(const char c)
  {
    switch (c)
    {
      case ' ': case '\n': case '\r': case '\t': return true;
      default: return false;
    }
  }

  bool is_bracket(const char c)
  {
    switch (c)
    {
      case '(': case ')': case '[': case ']': case '{': case '}': return true;
      default: return false;
    }
  }

  bool is_numeric(const std::string &token)
  {
    return std::regex_match(token, numeric_regex);
  }

  bool is_nill(const expr &e)
  {
    return
      (std::holds_alternative<list>(e) && !std::get<list>(e).node) ||
      (std::holds_alternative<atom>(e) &&
       std::holds_alternative<nill_t>(std::get<atom>(e)));
  }

  bool is_null(const expr &e)
  {
    return std::holds_alternative<null_t>(e);
  }

  std::vector<std::string> tokenise(const std::string &str)
  {
    std::vector<std::string> result;
    result.emplace_back();

    bool in_string = false;
    bool is_string_escaping = false;
    char string_char = 0;
    for (const auto c : str)
    {
      if (in_string)
      {
        if (is_string_escaping)
        {
          if (c == 'n') result.back() += '\n';
          else if (c == 'r') result.back() += '\r';
          else if (c == 't') result.back() += '\t';
          else if (c == '0') result.back() += '\0';
          else result.back() += c;
          is_string_escaping = false;
        }
        else if (c == '\\')
        {
          is_string_escaping = true;
        }
        else
        {
          result.back() += c;
          if (c == string_char)
          {
            in_string = false;
            result.emplace_back();
          }
        }
      }
      else if (c == '"' || c == '\'')
      {
        result.back() += c;
        in_string = true;
        is_string_escaping = false;
        string_char = c;
      }
      else if (is_whitespace(c))
      {
        if (!result.back().empty()) result.emplace_back();
      }
      else if (is_bracket(c))
      {
        if (result.back().empty()) result.back() += c;
        else result.emplace_back(std::string() + c);
        result.emplace_back();
      }
      else
      {
        result.back() += c;
      }
    }

    if (result.back().empty()) result.pop_back();
    return result;
  }

  number parse_number(const std::string &token)
  {
    using namespace std::string_literals;

    number result;

    std::smatch match;

    ASSERTF(std::regex_match(token, match, numeric_regex),
            "Bad number (" << token << ")");

    ASSERTF(match.size() == 9,
            "Bad number (" << token << ")");

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
        result = static_cast<intmax_t>(
          std::stoll(match[1].str() + match[2].str(), nullptr, 10));
      }
      else
      {
        // unsigned int
        result = static_cast<uintmax_t>(
          std::stoull(match[2].str(), nullptr, 10));
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
        result = static_cast<intmax_t>(
          std::stoll(match[4].str() + match[5].str(), nullptr, 16));
      }
      else
      {
        // unsigned int
        result = static_cast<uintmax_t>(
          std::stoull(match[5].str(), nullptr, 16));
      }
    }
    else if (match[8].matched)
    {
      if (match[7].matched)
      {
        // signed int
        result = static_cast<intmax_t>(
          std::stoll(match[7].str() + match[8].str(), nullptr, 2));
      }
      else
      {
        // unsigned int
        result = static_cast<uintmax_t>(
          std::stoull(match[8].str(), nullptr, 2));
      }
    }
    else FATAL("Bad number (" << token << ")");

    return result;
  }

  string parse_string(const std::string &token)
  {
    ASSERTF(token.size() >= 2,
            "string token (" << lak::strconv_ascii(token) << ") too short");
    if (token.size() == 2) return string{};
    return string{token.substr(1, token.size() - 2)};
  }

  expr parse(const std::vector<std::string> &tokens)
  {
    using namespace std::string_literals;

    list root;
    std::vector<std::vector<list>> stack;

    auto push_element = [&]() -> list
    {
      ASSERTF(!stack.empty(), "Cannot push element onto an empty stack");
      // Get the previous nill element.
      list old_element = stack.back().back();
      // If this element holds null_t then no values have been added to this
      // scope yet, so return immediately.
      if (!old_element) return old_element;
      // Else if the last element already has a value, add a new element after
      // it
      list new_element{list::create()};
      // Set the new nill element as the next element from the previous nill
      // element.
      old_element.set_next(new_element);
      // Push the new nill element into the stack scope.
      stack.back().emplace_back(new_element);
      // Return the old nill element so a value can be added to it.
      return new_element;
    };

    auto push_scope = [&]()
    {
      // Create the root element for the new stack.
      list scope{list::create()};
      scope.set_value(null);

      // If this is the first scope, make sure to mark it as the root.
      // Else this is a nested scope, push it as a value onto the parent scope.
      if (stack.empty())
        root = scope;
      else
        push_element().set_value(scope);

      // Push the new scope onto the stack.
      stack.emplace_back();
      stack.back().emplace_back(scope);
    };

    auto pop_scope = [&]()
    {
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
        expr value;

        if (c == '"')
          value = expr{atom{parse_string(token)}};
        else if (token == "nill")
          value = expr{atom{nill}};
        else if (is_numeric(token))
          value = expr{atom{parse_number(token)}};
        else
          value = expr{atom{symbol(token)}};

        if (stack.empty())
          return value;
        else
          push_element().set_value(value);
      }
    }

    return expr{root};
  }

  std::string to_string(const expr &e)
  {
    return std::visit([](auto &&e) { return lisk::to_string(e); }, e);
  }

  std::string to_string(list l)
  {
    if (!l.node) return "";
    if (is_null(l.value())) return "()";

    std::string result;

    result += '(';
    do
    {
      result += to_string(l.value());
      ++l;
      if (l) result += ' ';
    }
    while (l);
    result += ')';

    return result;
  }

  std::string to_string(functor f)
  {
    if (f.wrapped)
      return "<builtin " +
        lak::to_string((uintptr_t)f.value.wrapped.wrapper) + " " +
        lak::to_string((uintptr_t)f.value.wrapped.function) + ">";
    else
      return "<builtin " +
        lak::to_string((uintptr_t)f.value.basic.function) + ">";
  }

  std::string to_string(const lambda &l)
  {
    return "(lambda " + to_string(l.params) + " " + to_string(l.exp) + ")";
  }

  std::string to_string(const atom &a)
  {
    return std::visit([](auto &&a) { return lisk::to_string(a); }, a);
  }

  std::string to_string(nill_t)
  {
    return "nill";
  }

  std::string to_string(const symbol &s)
  {
    return s;
  }

  std::string to_string(const string &s)
  {
    string str = s;
    auto replace = [](string &str, const char c, const char *cstr)
    {
      for (auto i = str.find(c); i != string::npos; i = str.find(c))
        str.replace(i, 1, cstr);
    };
    replace(str, '\n', "\\n");
    replace(str, '\r', "\\r");
    replace(str, '\t', "\\t");
    replace(str, '\0', "\\0");
    replace(str, '\"', "\\\"");

    return "\"" + str + "\"";
  }

  std::string _to_string(uintmax_t n)
  {
    return lak::to_string(n);
  }

  std::string _to_string(intmax_t n)
  {
    return (n >= 0 ? "+" : "") + lak::to_string(n);
  }

  std::string _to_string(long double n)
  {
    return (n >= 0.0 ? "+" : "") + lak::to_string(n);
  }

  std::string to_string(const number &n)
  {
    return std::visit([](auto &&n) { return _to_string(n); }, n);
  }

  list eval_all(list l, env &e)
  {
    list result{list::create()};
    list end = result;
    for (const auto &node : l)
    {
      end.set_next({list::create()});
      ++end;
      end.set_value(eval(node.value, e));
    }
    return ++result;
  }

  expr eval(const expr &exp, env &e)
  {
    if (symbol a; get_arg_as(exp, a))
    {
      return e[a];
    }
    else if (atom n; get_arg_as(exp, n))
    {
      return n;
    }
    else if (list l; get_arg_as(exp, l))
    {
      auto subexp = eval(l.value(), e);

      // This is the empty list (nill).
      if (is_nill(subexp)) return expr{atom{nill}};

      if (functor f; get_arg_as(subexp, f))
      {
        // Built-ins handle argument evaluation themselves.
        return f(l.next(), e);
      }
      else if (lambda lf; get_arg_as(subexp, lf))
      {
        // Lambdas have their arguments automatically evaluated.
        return lf(eval_all(l.next(), e), e);
      }
      else
      {
        ERROR("Failed to eval sub-expression '" << to_string(subexp) << "' "
              "of '" << to_string(exp) << "', expected first element to be a "
              "functor or lambda");
        return expr{atom{nill}};
      }
    }
    else
    {
      ERROR("Failed to eval expression '" << to_string(exp) << "'");
      return expr{atom{nill}};
    }
  }

  bool get_arg_as(const expr &in_expr, expr &out_arg)
  {
    out_arg = in_expr;
    return true;
  }

  // bool get_arg_as(const expr &in_expr, uneval_expr &out_arg)
  // {
  //   out_arg = uneval_expr{in_expr};
  //   return true;
  // }

  bool get_arg_as(const expr &in_expr, list &out_arg)
  {
    if (std::holds_alternative<list>(in_expr))
    {
      out_arg = std::get<list>(in_expr);
      return true;
    }
    return false;
  }

  bool get_arg_as(const expr &in_expr, functor &out_arg)
  {
    if (std::holds_alternative<functor>(in_expr))
    {
      out_arg = std::get<functor>(in_expr);
      return true;
    }
    return false;
  }

  bool get_arg_as(const expr &in_expr, lambda &out_arg)
  {
    if (std::holds_alternative<lambda>(in_expr))
    {
      out_arg = std::get<lambda>(in_expr);
      return true;
    }
    return false;
  }

  bool get_arg_as(const expr &in_expr, atom &out_arg)
  {
    if (std::holds_alternative<atom>(in_expr))
    {
      out_arg = std::get<atom>(in_expr);
      return true;
    }
    return false;
  }

  bool get_arg_as(const expr &in_expr, symbol &out_arg)
  {
    if (std::holds_alternative<atom>(in_expr))
    {
      auto &a = std::get<atom>(in_expr);
      if (std::holds_alternative<symbol>(a))
      {
        out_arg = std::get<symbol>(a);
        return true;
      }
    }
    return false;
  }

  bool get_arg_as(const expr &in_expr, string &out_arg)
  {
    if (std::holds_alternative<atom>(in_expr))
    {
      auto &a = std::get<atom>(in_expr);
      if (std::holds_alternative<string>(a))
      {
        out_arg = std::get<string>(a);
        return true;
      }
    }
    return false;
  }

  bool get_arg_as(const expr &in_expr, number &out_arg)
  {
    if (std::holds_alternative<atom>(in_expr))
    {
      auto &a = std::get<atom>(in_expr);
      if (std::holds_alternative<number>(a))
      {
        out_arg = std::get<number>(a);
        return true;
      }
    }
    return false;
  }

  bool get_arg_as(const expr &in_expr, uintmax_t &out_arg)
  {
    if (std::holds_alternative<atom>(in_expr))
    {
      auto &a = std::get<atom>(in_expr);
      if (std::holds_alternative<number>(a))
      {
        auto &n = std::get<number>(a);
        if (std::holds_alternative<uintmax_t>(n))
        {
          out_arg = std::get<uintmax_t>(n);
          return true;
        }
      }
    }
    return false;
  }

  bool get_arg_as(const expr &in_expr, intmax_t &out_arg)
  {
    if (std::holds_alternative<atom>(in_expr))
    {
      auto &a = std::get<atom>(in_expr);
      if (std::holds_alternative<number>(a))
      {
        auto &n = std::get<number>(a);
        if (std::holds_alternative<intmax_t>(n))
        {
          out_arg = std::get<intmax_t>(n);
          return true;
        }
      }
    }
    return false;
  }

  bool get_arg_as(const expr &in_expr, long double &out_arg)
  {
    if (std::holds_alternative<atom>(in_expr))
    {
      auto &a = std::get<atom>(in_expr);
      if (std::holds_alternative<number>(a))
      {
        auto &n = std::get<number>(a);
        if (std::holds_alternative<long double>(n))
        {
          out_arg = std::get<long double>(n);
          return true;
        }
      }
    }
    return false;
  }

  namespace builtin
  {
    expr list_env(env &e)
    {
      list root = {list::create()};

      list previous = root;
      list l = root;
      for (const auto &[key, value] : e.values)
      {
        {
          auto k = key;
          auto v = value;
        }
        list entry = {list::create()};
        entry.set_value(atom{key});
        entry.set_next({list::create()});
        entry.next().set_value(value);

        l.set_value(entry);
        l.set_next({list::create()});

        previous = l++;
      }
      previous.clear_next();

      return root;
    }

    expr nill_check(env &e, expr exp)
    {
      // if (is_nill(eval(l.value(), e)))
      if (is_nill(exp))
        return expr{atom{number{uintmax_t{1U}}}};
      else
        return expr{atom{nill}};
    }

    // expr equal_check(list l, env &e)
    // {
    //   if (eval(l.value(), e) == eval(l.next().value(), e))
    //     return expr{atom{number{uintmax_t{1U}}}};
    //   else
    //     return expr{atom{nill}};
    // }

    expr conditional(env &e, expr exp, uneval_expr cond, uneval_expr alt)
    {
      if (!is_nill(exp))
        return eval(cond, e);
      else
        return eval(alt, e);
    }

    expr define(env &e, symbol sym, expr exp)
    {
      e.define_expr(sym, exp);
      return expr{atom{nill}};
    }

    expr evaluate(env &e, expr exp)
    {
      return eval(exp, e);
    }

    expr begin(list l, env &e)
    {
      // Evaluate every element in the list and return the result of the last.
      expr result;
      for (const auto &node : l)
      {
        result = eval(node.value, e);
      }
      return result;
    }

    expr repeat(env &e, uintmax_t count, uneval_expr exp)
    {
      while (count --> 0) eval(exp, e);
      return expr{atom{nill}};
    }

    expr repeat_while(env &e, uneval_expr exp)
    {
      while (!is_nill(eval(exp, e)));
      return expr{atom{nill}};
    }

    expr foreach(env &e, symbol sym, list iterlist, uneval_expr exp)
    {
      for (const auto &node : iterlist)
      {
        env new_env = e;
        new_env.define_expr(sym, node.value);
        eval(exp, new_env);
      }
      return expr{atom{nill}};
    }

    expr map(env &e, list iterlist, uneval_expr exp)
    {
      auto subexp = eval(exp, e);
      if (functor f; get_arg_as(subexp, f))
      {
        list arg{list::create()};
        list result{list::create()};
        list end = result;
        for (const auto &node : iterlist)
        {
          arg.set_value(node.value);
          end.set_next_value(f(arg, e));
          ++end;
        }
        return ++result;
      }
      else if (lambda lf; get_arg_as(subexp, lf))
      {
        list arg{list::create()};
        list result{list::create()};
        list end = result;
        for (const auto &node : iterlist)
        {
          arg.set_value(node.value);
          end.set_next_value(lf(arg, e));
          ++end;
        }
        return ++result;
      }
      else
      {
        ERROR("Map failed, '" << to_string(subexp) << "' is not a "
              "function/lambda");
        return expr{atom{nill}};
      }
      return expr{atom{nill}};
    }

    expr car(env &e, list l)
    {
      return l.value();
    }

    expr cdr(env &e, list l)
    {
      return l.next();
    }

    expr cons(env &e, expr exp, list l)
    {
      list result;
      result.set_value(exp);
      result.set_next(l);
      return result;
    }

    expr join(list l, env &e)
    {
      list first;
      list end;
      if (eval_as(l.value(), e, first))
      {
        end = first.last();
      }
      else
      {
        ERROR("join failed, argument '" << to_string(l.value()) <<"' "
              "is not a list");
        return expr{atom{nill}};
      }

      for (const auto &node : l.next())
      {
        if (list next; eval_as(node.value, e, next))
        {
          end.set_next(next);
          end = next.last();
        }
        else
        {
          ERROR("join failed, argument '" << to_string(node.value) <<"' "
                "is not a list");
          return expr{atom{nill}};
        }
      }

      return first;
    }

    expr range_list(env &e, number start, uintmax_t count, number step)
    {
      list result{list::create()};
      list end = result;
      for (uintmax_t i = 0; i < count; ++i)
      {
        end.set_next_value(atom{start + (step * number{i})});
        ++end;
      }
      return ++result;
    }

    expr make_list(list l, env &e)
    {
      return eval_all(l, e);
    }

    expr make_lambda(list l, env &e)
    {
      return lambda(l, e);
    }

    expr make_uint(env &e, expr exp)
    {
      auto to_uint = [](auto && n) -> number { return uintmax_t(n); };
      if (number n; get_arg_as(exp, n))
        return atom{std::visit(to_uint, n)};
      else if (string s; get_arg_as(exp, s))
        return atom{std::visit(to_uint, parse_number(s))};
      else
        return atom{std::visit(to_uint, parse_number(to_string(exp)))};
    }

    expr make_sint(env &e, expr exp)
    {
      auto to_sint = [](auto && n) -> number { return intmax_t(n); };
      if (number n; get_arg_as(exp, n))
        return atom{std::visit(to_sint, n)};
      else if (string s; get_arg_as(exp, s))
        return atom{std::visit(to_sint, parse_number(s))};
      else
        return atom{std::visit(to_sint, parse_number(to_string(exp)))};
    }

    expr make_float(env &e, expr exp)
    {
      auto to_ldouble = [](auto && n) -> number { return (long double)(n); };
      if (number n; get_arg_as(exp, n))
        return atom{std::visit(to_ldouble, n)};
      else if (string s; get_arg_as(exp, s))
        return atom{std::visit(to_ldouble, parse_number(s))};
      else
        return atom{std::visit(to_ldouble, parse_number(to_string(exp)))};
    }

    expr make_string(env &e, expr exp)
    {
      if (string s; get_arg_as(exp, s))
        return s;
      else
        return string{to_string(exp)};
    }

    expr read_string(list, env &)
    {
      std::string str;
      std::getline(std::cin, str);
      if (std::cin.good())
        return expr{atom{string(std::move(str))}};
      return expr{atom{nill}};
    }

    expr parse_string(env &e, string str)
    {
      return parse(tokenise(str));
    }

    expr print_string(list l, env &e)
    {
      expr result = eval(l.value(), e);
      // If the list evaluates to a pure string, then print it verbatim.
      // Otherwise to_string the result.
      if (string str; get_arg_as(result, str))
        std::cout << str;
      else
        std::cout << to_string(result);
      return expr{atom{nill}};
    }

    expr print_line(list l, env &e)
    {
      // No arguments, just print a newline.
      if (is_nill(l))
        std::cout << "\n";

      expr result = eval(l.value(), e);
      // If the list evaluates to a pure string, then print it verbatim.
      // Otherwise to_string the result.
      if (string str; get_arg_as(result, str))
        std::cout << str << "\n";
      else
        std::cout << to_string(result) << "\n";
      return expr{atom{nill}};
    }

    expr add(list l, env &e)
    {
      if (!l) return expr{atom{nill}};

      number result;

      if (number n; eval_as(l.value(), e, n))
      {
        result = n;
      }
      else
      {
        ERROR("Argument '" << to_string(l.value()) << "' is not a "
              "number");
        return expr{atom{nill}};
      }

      for (const auto &it : l.next())
      {
        if (number n; eval_as(it.value, e, n))
        {
          result += n;
        }
        else
        {
          ERROR("Argument '" << to_string(it.value) << "' is not a number");
          return expr{atom{nill}};
        }
      }

      return expr{atom{result}};
    }

    expr sub(list l, env &e)
    {
      if (!l) return expr{atom{nill}};

      number result;

      if (number n; eval_as(l.value(), e, n))
      {
        result = n;
      }
      else
      {
        ERROR("Argument '" << to_string(l.value()) << "' is not a "
              "number");
        return expr{atom{nill}};
      }

      for (const auto &it : l.next())
      {
        if (number n; eval_as(it.value, e, n))
        {
          result -= n;
        }
        else
        {
          ERROR("Argument '" << to_string(it.value) << "' is not a number");
          return expr{atom{nill}};
        }
      }

      return expr{atom{result}};
    }

    expr mul(list l, env &e)
    {
      if (!l) return expr{atom{nill}};

      number result;

      if (number n; eval_as(l.value(), e, n))
      {
        result = n;
      }
      else
      {
        ERROR("Argument '" << to_string(l.value()) << "' is not a "
              "number");
        return expr{atom{nill}};
      }

      for (const auto &it : l.next())
      {
        if (number n; eval_as(it.value, e, n))
        {
          result *= n;
        }
        else
        {
          ERROR("Argument '" << to_string(it.value) << "' is not a number");
          return expr{atom{nill}};
        }
      }

      return expr{atom{result}};
    }

    expr div(list l, env &e)
    {
      if (!l) return expr{atom{nill}};

      number result;

      if (number n; eval_as(l.value(), e, n))
      {
        result = n;
      }
      else
      {
        ERROR("Argument '" << to_string(l.value()) << "' is not a "
              "number");
        return expr{atom{nill}};
      }

      for (const auto &it : l.next())
      {
        if (number n; eval_as(it.value, e, n))
        {
          result /= n;
        }
        else
        {
          ERROR("Argument '" << to_string(it.value) << "' is not a number");
          return expr{atom{nill}};
        }
      }

      return expr{atom{result}};
    }

    env default_env()
    {
      env e;

      e.define_atom("pi", atom{number{3.14159L}});

      e.define_functor("env",     &list_env);
      e.define_functor("nill?",   &nill_check);
      // e.define_functor("eq?",     &equal_check);
      e.define_functor("if",      &conditional);
      e.define_functor("define",  &define);
      e.define_functor("eval",    &evaluate);
      e.define_functor("begin",   &begin);
      e.define_functor("repeat",  &repeat);
      e.define_functor("while",   &repeat_while);
      e.define_functor("foreach", &foreach);
      e.define_functor("map",     &map);
      e.define_functor("car",     &car);
      e.define_functor("cdr",     &cdr);
      e.define_functor("cons",    &cons);
      e.define_functor("join",    &join);
      e.define_functor("range",   &range_list);
      e.define_functor("list",    &make_list);
      e.define_functor("lambda",  &make_lambda);
      e.define_functor("uint",    &make_uint);
      e.define_functor("sint",    &make_sint);
      e.define_functor("float",   &make_float);
      e.define_functor("string",  &make_string);
      e.define_functor("read",    &read_string);
      e.define_functor("parse",   &parse_string);
      e.define_functor("print",   &print_string);
      e.define_functor("println", &print_line);
      e.define_functor("+",       &add);
      e.define_functor("-",       &sub);
      e.define_functor("*",       &mul);
      e.define_functor("/",       &div);

      return e;
    }
  }
}