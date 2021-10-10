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

#include <deque>
#include <memory>
#include <regex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

namespace lisk
{
  extern const std::regex numeric_regex;

  bool is_whitespace(const char c);
  bool is_bracket(const char c);
  bool is_numeric(const string &token);

  bool is_nil(const expression &expr);
  bool is_null(const expression &expr);

  std::vector<string> tokenise(const string &str,
                               size_t *chars_used = nullptr);
  std::vector<string> root_tokenise(const string &str,
                                    size_t *chars_used = nullptr);

  number parse_number(const string &token);
  string parse_string(const string &token);
  expression parse(const std::vector<string> &tokens);

  // Top level eval function.
  expression eval_string(const string &str, environment &env);
  expression root_eval_string(const string &str, environment &env);

  template<typename T>
  expression type_error(const string &message,
                        const T &t,
                        const string &expected)
  {
    return exception{message + ": '" + to_string(t) + "' is '" + type_name(t) +
                     "', expected " + expected};
  }

  struct reader
  {
    struct iterator
    {
      struct sentinel
      {
      };

      reader &ref;

      bool operator==(sentinel) const;

      bool operator!=(sentinel) const;

      lisk::expression operator*();

      iterator &operator++();
    };

    reader() = default;
    reader(const lisk::environment e, bool allow_tail = true);

    lisk::environment env;
    bool allow_tail_eval;

    lisk::string string_buffer;
    std::vector<lisk::string> token_buffer;
    std::deque<std::vector<lisk::string>> tokens;

    void clear();

    operator bool() const;

    iterator begin();

    iterator::sentinel end() const;

    reader &operator+=(const lisk::string &str);
  };

  namespace builtin
  {
    expression list_env(environment &env, bool allow_tail);
    expression null_check(environment &env, bool allow_tail, expression exp);
    expression nil_check(environment &env, bool allow_tail, expression exp);
    expression zero_check(environment &env, bool allow_tail, number num);
    // expr equal_check(shared_list l, environment &env);
    expression conditional(environment &env,
                           bool allow_tail,
                           bool b,
                           uneval_expr cond,
                           uneval_expr alt);
    expression define(environment &env,
                      bool allow_tail,
                      symbol sym,
                      expression exp);
    expression evaluate(environment &env, bool allow_tail, expression exp);
    std::pair<expression, size_t> evaluate_stack(shared_list l,
                                                 environment &env,
                                                 bool allow_tail);
    std::pair<expression, size_t> begin(shared_list l,
                                        environment &env,
                                        bool allow_tail);
    expression repeat(environment &env,
                      bool allow_tail,
                      uint_t count,
                      uneval_expr exp);
    expression repeat_while(environment &env,
                            bool allow_tail,
                            uneval_expr exp);
    expression foreach (environment &env,
                        bool allow_tail,
                        symbol sym,
                        shared_list iterlist,
                        uneval_expr exp);
    expression map(environment &env,
                   bool allow_tail,
                   shared_list iterlist,
                   uneval_expr exp);
    std::pair<expression, size_t> tail_call(shared_list l,
                                            environment &env,
                                            bool allow_tail);

    expression car(environment &env, bool allow_tail, shared_list l);
    expression cdr(environment &env, bool allow_tail, shared_list l);
    expression cons(environment &env,
                    bool allow_tail,
                    expression exp,
                    shared_list l);
    std::pair<expression, size_t> join(shared_list l,
                                       environment &env,
                                       bool allow_tail);

    expression range_list(environment &env,
                          bool allow_tail,
                          number start,
                          uint_t count,
                          number step);
    std::pair<expression, size_t> make_list(shared_list l,
                                            environment &env,
                                            bool allow_tail);
    std::pair<expression, size_t> make_lambda(shared_list l,
                                              environment &env,
                                              bool allow_tail);
    expression make_uint(environment &env, bool allow_tail, expression exp);
    expression make_sint(environment &env, bool allow_tail, expression exp);
    expression make_real(environment &env, bool allow_tail, expression exp);
    expression make_string(environment &env, bool allow_tail, expression exp);

    expression read_string(environment &, bool allow_tail);
    expression parse_string(environment &env, bool allow_tail, string str);
    std::pair<expression, size_t> print_string(shared_list l,
                                               environment &env,
                                               bool allow_tail);
    std::pair<expression, size_t> print_line(shared_list l,
                                             environment &env,
                                             bool allow_tail);

    expression add(environment &env, bool allow_tail, number a, number b);
    expression sub(environment &env, bool allow_tail, number a, number b);
    expression mul(environment &env, bool allow_tail, number a, number b);
    expression div(environment &env, bool allow_tail, number a, number b);

    std::pair<expression, size_t> sum(shared_list l,
                                      environment &env,
                                      bool allow_tail);
    std::pair<expression, size_t> product(shared_list l,
                                          environment &env,
                                          bool allow_tail);

    environment default_env();
  };
}

#endif
