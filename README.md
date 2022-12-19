# lisk: A lisp like language framework for C++

Make sure to `git submodule update --init` after cloning this repo.

# Example usage

```cpp
#include <lisk/lisk.hpp>

#include <iostream>

// Lisk can use your own types!
struct my_type
{
  lisk::number value;
  my_type() = default;
  my_type(lisk::number num) : value(num) {}
};

// You just need to provide a function to get the name of your type...
const lisk::string &type_name(const my_type &)
{
  const static lisk::string name = "my_type";
  return name;
}

// ... and if you aren't only using your type through pointers, a function to
// convert from a lisk::expression to your type.
bool operator>>(const lisk::expression &arg, my_type &out)
{
  return arg >> out.value;
}

// Now your type can show up in functions! Lisk can handle raw pointers and
// shared pointers for you.
lisk::expression print_my_type(lisk::environment &e,
                               bool allow_tail_eval,
                               std::shared_ptr<my_type> my)
{
  std::cout << "My type value: " << to_string(my->value) << "\n";
  // Use nil when you don't really want to return anything.
  return lisk::atom::nil{};
}

lisk::expression create_my_type(lisk::environment &e,
                                bool allow_tail_eval,
                                lisk::number num)
{
  // We can safely return shared_ptrs to any type we like. They must be stored
  // in a lisk::atom. Lisk makes use of C++'s RTTI to make sure you don't do
  // any invalid type conversions.
  return lisk::atom(lisk::pointer(lak::shared_ptr<my_type>::make(num)));
}

int main()
{
  auto env = lisk::builtin::default_env();
  env.define_functor("print-my-type", &print_my_type);
  env.define_functor("create-my-type", &create_my_type);

  lisk::eval_string("(print-my-type (create-my-type 420.69))", env);
}
```

# Functions

## Variable length functions with unevaluated arguments

This function type will get an unevaluated list as its first argument, this is
the rest of the lisp-list that the function was called with. The
lisk::environment is the current environment at evaluation time, and the bool
parameter is used if you need to call lisk::eval within the function.

```cpp
lisk::expression my_function(lisk::list unevaluated_list,
                             lisk::environment &environment,
                             bool allow_tail_eval);
```

## Fix length functions with evaluated arguments

Like the previos function type, this function type gets a lisk::environment
which is the current environment at evaluation time, and the bool
parameter is used if you need to call lisk::eval within the function. Unlike
the previous, this function type can have any number of arguments following it,
which lisk will automatically evaluate and type check. Note that this means
that once compliled, the function is technically fixed length.

```cpp
lisk::expression my_function(lisk::environment &environment,
                             bool allow_tail_eval,
                             ...);

// e.g.
lisk::expression my_function(lisk::environment &environment,
                             bool allow_tail_eval,
                             lisk::number number,
                             lisk::atom atom)
{
  // ... do something with the number.
  // ... do something with the atom.
  return // ... return something
}
```
