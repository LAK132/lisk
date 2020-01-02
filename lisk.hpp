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

#ifndef LAK_LISK_HPP
#define LAK_LISK_HPP

#include "debug.hpp"

#include <memory>
#include <unordered_map>
#include <regex>
#include <string>
#include <tuple>
#include <variant>

namespace lisk
{
  extern const std::regex numeric_regex;

  struct nill_t {};
  extern nill_t nill;
  struct null_t {};
  extern null_t null;

  struct list;
  struct env;
  struct functor;
  struct lambda;

  struct symbol : public std::string
  {
    symbol() = default;
    symbol(const symbol &) = default;
    symbol(symbol &&) = default;
    symbol &operator=(const symbol &) = default;
    symbol &operator=(symbol &&) = default;

    symbol(const std::string &value) : std::string(value) {}
    symbol(std::string &&value) : std::string(std::move(value)) {}
    symbol(const char *value) : std::string(value) {}
    symbol &operator=(const std::string &value)
    { std::string::operator=(value); return *this; }
    symbol &operator=(std::string &&value)
    { std::string::operator=(std::move(value)); return *this; }
    symbol &operator=(const char *value)
    { std::string::operator=(value); return *this; }

    using std::string::operator[];
  };

  struct string : public std::string
  {
    string() = default;
    string(const string &) = default;
    string(string &&) = default;
    string &operator=(const string &) = default;
    string &operator=(string &&) = default;

    string(const std::string &value) : std::string(value) {}
    string(std::string &&value) : std::string(std::move(value)) {}
    string(const char *value) : std::string(value) {}
    string &operator=(const std::string &value)
    { std::string::operator=(value); return *this; }
    string &operator=(std::string &&value)
    { std::string::operator=(std::move(value)); return *this; }
    string &operator=(const char *value)
    { std::string::operator=(value); return *this; }

    using std::string::operator[];
  };
}

template<> struct std::hash<lisk::symbol> : public std::hash<std::string> {};
template<> struct std::hash<lisk::string> : public std::hash<std::string> {};

namespace lisk
{
  using number = std::variant<uintmax_t, intmax_t, long double>;
  number operator+(number A, number B);
  number operator-(number A, number B);
  number operator*(number A, number B);
  number operator/(number A, number B);
  number &operator+=(number &A, number B);
  number &operator-=(number &A, number B);
  number &operator*=(number &A, number B);
  number &operator/=(number &A, number B);

  using atom = std::variant<nill_t, symbol, string, number>;

  using expr = std::variant<null_t, atom, list, functor, lambda>;

  struct list
  {
    struct node_t;
    std::shared_ptr<node_t> node;

    static std::shared_ptr<node_t> create();

    // Move along to the next node.
    list &operator++();
    list operator++(int);

    expr value() const;
    list next(size_t n = 1) const;

    void set_value(const expr &e);
    void set_next(list l);
    void set_next_value(const expr &e);
    void clear();
    void clear_next();

    list clone() const;

    list begin() const;
    list last() const;
    list end() const;

    bool operator==(const list &other) const;
    bool operator!=(const list &other) const;

    operator bool() const;

    const node_t &operator*() const;
    node_t &operator*();

    // const node_t *operator->() const;
    // node_t *operator->();
  };

  struct functor
  {
    struct basic_functor
    {
      using function_type = expr (*)(list, env &);
      function_type function;
    };
    struct wrapped_functor
    {
      using function_type = void (*)();
      using wrapper_type = expr (*)(function_type, list, env &);
      wrapper_type wrapper;
      function_type function;
    };
    union value_type
    {
      basic_functor basic;
      wrapped_functor wrapped;
    };
    value_type value = value_type{basic_functor{}};
    bool wrapped = false;

    functor() = default;
    functor(const functor &) = default;
    functor(functor &&) = default;
    functor &operator=(const functor &) = default;
    functor &operator=(functor &&) = default;

    functor(basic_functor::function_type f);
    functor(wrapped_functor::wrapper_type w, wrapped_functor::function_type f);
    template<typename ...ARGS>
    functor(expr (*f)(env &, ARGS...));

    expr operator()(list l, env &e) const;
  };

  struct lambda
  {
    list params;
    list exp;

    lambda() = default;
    lambda(const lambda &) = default;
    lambda &operator=(const lambda &) = default;

    lambda(list l, env &e);

    expr operator()(list l, env &e) const;
  };

  struct list::node_t
  {
    expr value;
    std::shared_ptr<node_t> next;
  };

  struct env
  {
    std::unordered_map<symbol, expr> values;

    void define_expr(const symbol &s, const expr &e);
    void define_atom(const symbol &s, const atom &a);
    void define_list(const symbol &s, list l);
    void define_functor(const symbol &s, functor f);
    void define_lambda(const symbol &s, const lambda &l);

    expr operator[](const symbol &s) const;
  };

  struct uneval_expr : public expr {};

  bool is_whitespace(const char c);
  bool is_bracket(const char c);
  bool is_numeric(const std::string &token);
  bool is_nill(const expr &e);
  bool is_null(const expr &e);

  std::vector<std::string> tokenise(const std::string &str);

  number parse_number(const std::string &token);
  string parse_string(const std::string &token);
  expr parse(const std::vector<std::string> &tokens);

  std::string to_string(const expr &e);
  std::string to_string(list l);
  std::string to_string(functor f);
  std::string to_string(const lambda &l);
  std::string to_string(const atom &a);
  std::string to_string(nill_t);
  std::string to_string(const symbol &s);
  std::string to_string(const string &s);
  std::string to_string(const number &n);

  template<typename T> std::string type_name()
  {
    static_assert(
      !std::is_same_v<T, std::remove_cv_t<std::remove_reference_t<T>>>,
      "Attempting to get type name of unknown type");
    return
      type_name<std::remove_cv_t<std::remove_reference_t<T>>>();
  }
  template<> std::string type_name<expr>()        { return "expr"; }
  template<> std::string type_name<uneval_expr>() { return "<UNEVAL EXPR>"; }
  template<> std::string type_name<null_t>()      { return "<NULL>"; }
  template<> std::string type_name<list>()        { return "list"; }
  template<> std::string type_name<functor>()     { return "functor"; }
  template<> std::string type_name<lambda>()      { return "lambda"; }
  template<> std::string type_name<atom>()        { return "atom"; }
  template<> std::string type_name<nill_t>()      { return "nill"; }
  template<> std::string type_name<symbol>()      { return "symbol"; }
  template<> std::string type_name<string>()      { return "string"; }
  template<> std::string type_name<number>()      { return "number"; }
  template<> std::string type_name<uintmax_t>()   { return "uint"; }
  template<> std::string type_name<intmax_t>()    { return "sint"; }
  template<> std::string type_name<long double>() { return "float"; }

  list eval_all(list l, env &e);
  expr eval(const expr &exp, env &e);

  bool get_arg_as(const expr &in_expr, expr &out_arg);
  bool get_arg_as(const expr &in_expr, list &out_arg);
  bool get_arg_as(const expr &in_expr, functor &out_arg);
  bool get_arg_as(const expr &in_expr, lambda &out_arg);
  bool get_arg_as(const expr &in_expr, atom &out_arg);
  bool get_arg_as(const expr &in_expr, symbol &out_arg);
  bool get_arg_as(const expr &in_expr, string &out_arg);
  bool get_arg_as(const expr &in_expr, number &out_arg);
  bool get_arg_as(const expr &in_expr, uintmax_t &out_arg);
  bool get_arg_as(const expr &in_expr, intmax_t &out_arg);
  bool get_arg_as(const expr &in_expr, long double &out_arg);

  template<typename T>
  bool eval_as(const expr &in_expr, env &e, T &out_arg)
  {
    return get_arg_as(eval(in_expr, e), out_arg);
  }

  template<typename T>
  bool get_or_eval_as(const expr &in_expr, env &e, T &out_arg)
  {
    return get_arg_as(in_expr, out_arg) || eval_as(in_expr, e, out_arg);
  }

  template<>
  bool get_or_eval_as<expr>(const expr &in_expr, env &e, expr &out_arg)
  {
    return eval_as(in_expr, e, out_arg);
  }

  template<>
  bool get_or_eval_as<uneval_expr>(const expr &in_expr, env &e,
                                   uneval_expr &out_arg)
  {
    return get_arg_as(in_expr, out_arg);
  }

  template<>
  bool get_or_eval_as<list>(const expr &in_expr, env &e, list &out_arg)
  {
    return eval_as(in_expr, e, out_arg);
  }

  // :TODO: Add uneval_list
  // template<>
  // bool get_or_eval_as<uneval_list>(const expr &in_expr, env &e,
  //                                  uneval_list &out_arg)
  // {
  //   return get_arg_as(in_expr, out_arg);
  // }

  template<typename ...TYPES, size_t ...I>
  bool _get_or_eval_as(list in_list, env &e, std::tuple<TYPES...> &out_arg,
                       std::index_sequence<I...>)
  {
    std::tuple<std::remove_cv_t<TYPES>...> result;
    auto _get_or_eval = [&](auto &&element, auto i) -> bool
    {
      if (!get_or_eval_as(in_list.value(), e, element))
      {
        ERROR("Failed to evaluate element " << i << " "
              "'" << to_string(in_list.value()) << "' "
              "of '" << to_string(in_list) << "', "
              "expected type '" << type_name<decltype(element)>() << "'");
        return false;
      }
      ++in_list;
      return true;
    };
    if ((_get_or_eval(std::get<I>(result), I) && ...))
    {
      out_arg = result;
      return true;
    }
    else return false;
  }

  template<typename ...TYPES>
  bool get_or_eval_as(list in_list, env &e, std::tuple<TYPES...> &out_arg)
  {
    return _get_or_eval_as(in_list, e, out_arg,
                           std::make_index_sequence<sizeof...(TYPES)>{});
  }

  template<typename ...ARGS>
  expr wrapper_function(void (*func)(), list l, env &e)
  {
    std::tuple<ARGS...> args;
    if (!get_or_eval_as(l, e, args)) return expr{atom{nill}};
    return std::apply((expr (*)(env &, ARGS...))func,
                      std::tuple_cat(std::forward_as_tuple(e), args));
  }

  template<>
  expr wrapper_function<>(void (*func)(), list l, env &e)
  {
    return ((expr (*)(env &))func)(e);
  }

  template<typename ...ARGS>
  functor::functor(expr (*f)(env &, ARGS...))
  : functor(&wrapper_function<ARGS...>, (void(*)())f)
  {
  }

  namespace builtin
  {
    expr list_env(env &e);
    expr nill_check(env &e, expr exp);
    // expr equal_check(list l, env &e);
    expr conditional(env &e, expr exp, uneval_expr cond, uneval_expr alt);
    expr define(env &e, symbol sym, expr exp);
    expr evaluate(env &e, expr exp);
    expr begin(list l, env &e);
    expr repeat(env &e, uintmax_t count, uneval_expr exp);
    expr repeat_while(env &e, uneval_expr exp);
    expr foreach(env &e, symbol sym, list iterlist, uneval_expr exp);
    expr map(env &e, list iterlist, uneval_expr exp);

    expr car(env &e, list l);
    expr cdr(env &e, list l);
    expr cons(env &e, expr exp, list l);
    expr join(list l, env &e);

    expr range_list(env &e, number start, uintmax_t count, number step);
    expr make_list(list l, env &e);
    expr make_lambda(list l, env &e);
    expr make_uint(env &e, expr exp);
    expr make_sint(env &e, expr exp);
    expr make_float(env &e, expr exp);
    expr make_string(env &e, expr exp);

    expr read_string(list, env &);
    expr parse_string(env &e, string str);
    expr print_string(list l, env &e);
    expr print_line(list l, env &e);

    expr add(list l, env &e);
    expr sub(list l, env &e);
    expr mul(list l, env &e);
    expr div(list l, env &e);

    env default_env();
  };
}

#endif