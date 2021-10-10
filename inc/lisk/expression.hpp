#ifndef LISK_EXPRESSION_HPP
#define LISK_EXPRESSION_HPP

#include "atom.hpp"
#include "callable.hpp"
#include "shared_list.hpp"
#include "string.hpp"

#include <variant>

namespace lisk
{
  struct expression;

  using shared_list = basic_shared_list<expression>;

  struct eval_shared_list
  {
    shared_list list;
  };
  struct uneval_shared_list
  {
    shared_list list;
  };

  struct exception
  {
    string message;
  };

  struct expression
  {
    struct null
    {
    };

    std::
      variant<null, atom, eval_shared_list, shared_list, callable, exception>
        _value;

    expression()                       = default;
    expression(const expression &expr) = default;
    expression(expression &&expr)      = default;

    expression &operator=(const expression &expr) = default;
    expression &operator=(expression &&expr) = default;

    expression(null);
    expression(const atom &a);
    expression(atom::nil);
    expression(const shared_list &list);
    expression(const eval_shared_list &list);
    expression(const callable &c);
    expression(const exception &exc);

    expression &operator=(null);
    expression &operator=(const atom &a);
    expression &operator=(atom::nil);
    expression &operator=(const shared_list &list);
    expression &operator=(const eval_shared_list &list);
    expression &operator=(const callable &c);
    expression &operator=(const exception &exc);

    bool is_null() const;
    bool is_atom() const;
    bool is_list() const;
    bool is_eval_list() const;
    bool is_callable() const;
    bool is_exception() const;

    inline bool empty() const { return is_null() || is_exception(); }
    inline operator bool() const { return !empty(); }

    const atom *get_atom() const;
    atom *get_atom();

    const shared_list *get_list() const;
    shared_list *get_list();

    const eval_shared_list *get_eval_list() const;
    eval_shared_list *get_eval_list();

    const callable *get_callable() const;
    callable *get_callable();

    const exception *get_exception() const;
    exception *get_exception();

    const atom &as_atom() const;
    atom &as_atom();

    const shared_list &as_list() const;
    shared_list &as_list();

    const eval_shared_list &as_eval_list() const;
    eval_shared_list &as_eval_list();

    const callable &as_callable() const;
    callable &as_callable();

    const exception &as_exception() const;
    exception &as_exception();

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda) const
    {
      return std::visit(lambda, _value);
    }

    template<typename LAMBDA>
    auto visit(LAMBDA &&lambda)
    {
      return std::visit(lambda, _value);
    }
  };

  struct eval_expr
  {
    expression expr;
  };
  struct uneval_expr
  {
    expression expr;
  };

  const string &type_name(const shared_list &);

  string to_string(const eval_shared_list &list);
  const string &type_name(const eval_shared_list &);

  string to_string(const uneval_shared_list &list);
  const string &type_name(const uneval_shared_list &);

  string to_string(const exception &exc);
  const string &type_name(const exception &);

  string to_string(expression::null);
  const string &type_name(expression::null);

  string to_string(const expression &expr);
  const string &type_name(const expression &);

  // string to_string(const eval_expr &expr);
  const string &type_name(const eval_expr &);

  // string to_string(const uneval_expr &expr);
  const string &type_name(const uneval_expr &);
}

bool operator>>(const lisk::expression &arg, lisk::expression &out);
bool operator>>(const lisk::expression &arg, lisk::exception &out);
bool operator>>(const lisk::expression &arg, lisk::uneval_expr &out);
bool operator>>(const lisk::expression &arg, lisk::shared_list &out);
bool operator>>(const lisk::expression &arg, lisk::uneval_shared_list &out);

// These are here instead of in pointer.hpp because of include ordering issues.
template<typename T>
bool operator>>(const lisk::expression &arg, T *&out);
template<typename T>
bool operator>>(const lisk::expression &arg, const T *&out);
template<typename T>
bool operator>>(const lisk::expression &arg, std::shared_ptr<T> &out);

#include "expression.inl"

#endif
