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

    auto begin_next = [&]
    {
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

    auto push_element = [&]() -> shared_list
    {
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

    auto push_scope = [&]()
    {
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
      else if (c == '[')
      {
        push_scope();
        push_element().value() = atom{symbol("list")};
      }
      else if (c == '{')
      {
        push_scope();
        push_element().value() = atom{symbol("eval-stack")};
      }
      else if (c == ')' || c == ']' || c == '}')
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
    return lisk::eval(
      lisk::parse(ref.tokens.front()), ref.env, ref.allow_tail_eval);
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
    expression list_env(environment &env, bool)
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

    expression null_check(environment &, bool, expression exp)
    {
      return atom{is_null(exp)};
    }

    expression nil_check(environment &, bool, expression exp)
    {
      return atom{is_nil(exp)};
    }

    expression zero_check(environment &, bool, number num)
    {
      return atom{num.visit([](auto &&n) -> bool { return n == 0; })};
    }

    // expression equal_check(shared_list l, environment &)
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

    expression define(environment &env, bool, symbol sym, expression exp)
    {
      env.define_expr(sym, exp);
      return atom::nil{};
    }

    expression evaluate(environment &env, bool allow_tail, expression exp)
    {
      return eval(exp, env, allow_tail);
    }

    std::pair<expression, size_t> evaluate_stack(shared_list l,
                                                 environment &env,
                                                 bool allow_tail)
    {
      shared_list head;
      size_t input_length = 0;
      for (; l; ++l, ++input_length)
      {
        auto expr = eval(l.value(), env, allow_tail);
        if (callable c; expr >> c)
        {
          auto [result, pop_count] = c(head, env, allow_tail);
          for (size_t i = 0; i < pop_count; ++i) ++head;
          if (head) head = shared_list::extends(head);
          head.value() = std::move(result);
        }
        else
        {
          if (head) head = shared_list::extends(head);
          head.value() = expr;
        }
      }

      return {expression{head}, input_length};
    }

    std::pair<expression, size_t> begin(shared_list l,
                                        environment &env,
                                        bool allow_tail)
    {
      // Evaluate every element in the list and return the result of the last.
      std::pair<expression, size_t> result;
      result.second = 0;
      for (const auto &node : l)
      {
        result.first = eval(node.value, env, allow_tail);
        ++result.second;
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
          end.next_value() = f(arg, env, allow_tail).first;
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
          end.next_value() = lf(arg, env, allow_tail).first;
          ++end;
        }
        return ++result;
      }
      else
        return subexp.visit(
          [](auto &&a)
          { return type_error("Map error", a, "a function or lambda"); });
    }

    std::pair<expression, size_t> tail_call(shared_list list,
                                            environment &env,
                                            bool allow_tail)
    {
      // (tail (func args...))

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
      lambda_list.value()      = shared_list::create(); // no arguments
      lambda_list.next_value() = list.value();          // lambda body
      result.value() = callable(lambda(lambda_list, tail_env, allow_tail));

      // Return an eval_shared_list containing a lambda that immediately
      // evaluates our list param.
      return {eval_shared_list{result}, 1};
    }

    expression car(environment &, bool, shared_list l) { return l.value(); }

    expression cdr(environment &, bool, shared_list l) { return l.next(); }

    expression cons(environment &, bool, expression exp, shared_list l)
    {
      shared_list result;
      result.value() = exp;
      result.set_next(l);
      return result;
    }

    std::pair<expression, size_t> join(shared_list l,
                                       environment &env,
                                       bool allow_tail)
    {
      shared_list first;
      shared_list end;
      size_t count = 0;

      if (eval(l.value(), env, allow_tail) >> first)
      {
        ++count;
        end = first.last();
      }
      else
      {
        return {type_error("Join error", l.value(), "a list"), 0};
      }

      for (const auto &node : l.next())
      {
        ++count;
        if (shared_list next; eval(node.value, env, allow_tail) >> next)
        {
          end.set_next(next);
          end = next.last();
        }
        else
        {
          return {type_error("Join error", node.value, "a list"), 0};
        }
      }

      return {first, count};
    }

    expression range_list(
      environment &, bool, number start, uint_t count, number step)
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

    std::pair<expression, size_t> make_list(shared_list l,
                                            environment &env,
                                            bool allow_tail)
    {
      return eval_all(l, env, allow_tail);
    }

    std::pair<expression, size_t> make_lambda(shared_list l,
                                              environment &env,
                                              bool allow_tail)
    {
      // :TODO: check that there's exactly 2 arguments
      return {callable(lambda(l, env, allow_tail)), 2};
    }

    expression make_uint(environment &, bool, expression exp)
    {
      auto to_uint = [](auto &&n) -> number { return uint_t(n); };
      if (number n; exp >> n)
        return atom{n.visit(to_uint)};
      else if (string s; exp >> s)
        return atom{parse_number(s).visit(to_uint)};
      else
        return atom{parse_number(to_string(exp)).visit(to_uint)};
    }

    expression make_sint(environment &, bool, expression exp)
    {
      auto to_sint = [](auto &&n) -> number { return sint_t(n); };
      if (number n; exp >> n)
        return atom{n.visit(to_sint)};
      else if (string s; exp >> s)
        return atom{parse_number(s).visit(to_sint)};
      else
        return atom{parse_number(to_string(exp)).visit(to_sint)};
    }

    expression make_real(environment &, bool, expression exp)
    {
      auto to_real = [](auto &&n) -> number { return (real_t)(n); };
      if (number n; exp >> n)
        return atom{n.visit(to_real)};
      else if (string s; exp >> s)
        return atom{parse_number(s).visit(to_real)};
      else
        return atom{parse_number(to_string(exp)).visit(to_real)};
    }

    expression make_string(environment &, bool, expression exp)
    {
      if (string s; exp >> s)
        return expression(s);
      else
        return expression(string{to_string(exp)});
    }

    expression read_string(environment &, bool)
    {
      std::string str;
      std::getline(std::cin, str);
      if (std::cin.good()) return expression{atom{string(std::move(str))}};
      return atom::nil{};
    }

    expression parse_string(environment &, bool, string str)
    {
      return parse(tokenise(str));
    }

    std::pair<expression, size_t> print_string(shared_list l,
                                               environment &env,
                                               bool allow_tail)
    {
      expression result = eval(l.value(), env, allow_tail);
      // If the list evaluates to a pure string, then print it verbatim.
      // Otherwise to_string the result.
      if (string str; result >> str)
        std::cout << str;
      else
        std::cout << to_string(result);
      return {atom::nil{}, 1};
    }

    std::pair<expression, size_t> print_line(shared_list l,
                                             environment &env,
                                             bool allow_tail)
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
      return {atom::nil{}, 1};
    }

    expression add(environment &, bool, number a, number b)
    {
      return lisk::atom{a + b};
    }

    expression sub(environment &, bool, number a, number b)
    {
      return lisk::atom{a - b};
    }

    expression mul(environment &, bool, number a, number b)
    {
      return lisk::atom{a * b};
    }

    expression div(environment &, bool, number a, number b)
    {
      return lisk::atom{a / b};
    }

    std::pair<expression, size_t> sum(shared_list l,
                                      environment &env,
                                      bool allow_tail)
    {
      if (!l) return {atom::nil{}, 0};

      number result;
      size_t count = 0;

      if (number n; eval(l.value(), env, allow_tail) >> n)
      {
        ++count;
        result = n;
      }
      else
      {
        return {type_error("Add error", l.value(), "a number"), 0};
      }

      for (const auto &it : l.next())
      {
        ++count;
        if (number n; eval(it.value, env, allow_tail) >> n)
        {
          result += n;
        }
        else
        {
          return {type_error("Add error", l.value(), "a number"), 0};
        }
      }

      return {expression{atom{result}}, count};
    }

    std::pair<expression, size_t> product(shared_list l,
                                          environment &env,
                                          bool allow_tail)
    {
      if (!l) return {atom::nil{}, 0};

      number result;
      size_t count = 0;

      if (number n; eval(l.value(), env, allow_tail) >> n)
      {
        ++count;
        result = n;
      }
      else
      {
        return {type_error("Mul error", l.value(), "a number"), 0};
      }

      for (const auto &it : l.next())
      {
        ++count;
        if (number n; eval(it.value, env, allow_tail) >> n)
        {
          result *= n;
        }
        else
        {
          return {type_error("Mul error", l.value(), "a number"), 0};
        }
      }

      return {expression{atom{result}}, count};
    }

    environment default_env()
    {
      environment e;

      e.define_atom("pi", atom(number(3.14159L)));

      e.define_functor("env", LISK_FUNCTOR_WRAPPER(list_env));
      e.define_functor("null?", LISK_FUNCTOR_WRAPPER(null_check));
      e.define_functor("nil?", LISK_FUNCTOR_WRAPPER(nil_check));
      e.define_functor("zero?", LISK_FUNCTOR_WRAPPER(zero_check));
      // e.define_functor("eq?", LISK_FUNCTOR_WRAPPER(equal_check));
      e.define_functor("if", LISK_FUNCTOR_WRAPPER(conditional));
      e.define_functor("define", LISK_FUNCTOR_WRAPPER(define));
      e.define_functor("eval", LISK_FUNCTOR_WRAPPER(evaluate));
      e.define_functor("eval-stack", evaluate_stack);
      e.define_functor("begin", begin);
      e.define_functor("repeat", LISK_FUNCTOR_WRAPPER(repeat));
      e.define_functor("while", LISK_FUNCTOR_WRAPPER(repeat_while));
      e.define_functor("foreach", LISK_FUNCTOR_WRAPPER(foreach));
      e.define_functor("map", LISK_FUNCTOR_WRAPPER(map));
      e.define_functor("tail", tail_call);

      e.define_functor("car", LISK_FUNCTOR_WRAPPER(car));
      e.define_functor("cdr", LISK_FUNCTOR_WRAPPER(cdr));
      e.define_functor("cons", LISK_FUNCTOR_WRAPPER(cons));
      e.define_functor("join", join);

      e.define_functor("range", LISK_FUNCTOR_WRAPPER(range_list));
      e.define_functor("list", make_list);
      e.define_functor("lambda", make_lambda);
      e.define_functor("uint", LISK_FUNCTOR_WRAPPER(make_uint));
      e.define_functor("sint", LISK_FUNCTOR_WRAPPER(make_sint));
      e.define_functor("real", LISK_FUNCTOR_WRAPPER(make_real));
      e.define_functor("string", LISK_FUNCTOR_WRAPPER(make_string));

      e.define_functor("read", LISK_FUNCTOR_WRAPPER(read_string));
      e.define_functor("parse", LISK_FUNCTOR_WRAPPER(parse_string));
      e.define_functor("print", print_string);
      e.define_functor("println", print_line);

      e.define_functor("+", LISK_FUNCTOR_WRAPPER(add));
      e.define_functor("-", LISK_FUNCTOR_WRAPPER(sub));
      e.define_functor("*", LISK_FUNCTOR_WRAPPER(mul));
      e.define_functor("/", LISK_FUNCTOR_WRAPPER(div));

      e.define_functor("sum", sum);
      e.define_functor("product", product);

      return e;
    }
  }
}
