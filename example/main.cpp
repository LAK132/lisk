#include <lisk/lisk.hpp>

#include <iostream>
#include <typeindex>
#include <typeinfo>

// Create a new type.
struct my_type
{
  lisk::uint_t value;
};

// Define the function to convert a lisk expression into our new type.
bool operator>>(const lisk::expression &arg, my_type &out)
{
  return arg >> out.value;
}

// Define the function to get a string name of our new type.
const lisk::string &type_name(const my_type &)
{
  const static lisk::string name = "my_type";
  return name;
}

// We can now use this new type as a function parameter!
lisk::expression function_taking_my_type(lisk::environment &, bool, my_type my)
{
  std::cout << "My type value: " << my.value << "\n";

  return lisk::atom::nil{};
}

lisk::expression print_my_type_value(lisk::environment &e,
                                     bool allow_tail,
                                     std::shared_ptr<my_type> my)
{
  return my ? function_taking_my_type(e, allow_tail, *my)
            : lisk::expression::null{};
}

lisk::expression create_my_type_ptr(lisk::environment &, bool)
{
  return lisk::atom(std::make_shared<my_type>(my_type{10U}));
}

bool running = true;

lisk::expression my_exit(lisk::environment &, bool)
{
  running = false;
  return lisk::expression(lisk::atom(lisk::atom::nil{}));
}

int main()
{
  using namespace std::string_literals;

  lisk::environment default_env = lisk::builtin::default_env();
  default_env.define_functor("exit", LISK_FUNCTOR_WRAPPER(my_exit));

  // Add the functions using our new type to the lisk environment.
  default_env.define_functor("print_my_type",
                             LISK_FUNCTOR_WRAPPER(print_my_type_value));
  default_env.define_functor("create_my_type",
                             LISK_FUNCTOR_WRAPPER(create_my_type_ptr));

  // Should print "My type value: 10".
  lisk::eval_string("(print_my_type (create_my_type))", default_env);

  // Should cause a type error.
  std::cout << lisk::to_string(
                 lisk::eval_string("(print_my_type 1337)", default_env))
            << "\n";

  // Should print "2048"
  lisk::eval_string(
    R"(; This is a comment!
(begin
  (define func
    (lambda (x n)
      (begin
        ; (println "Called func")
        ; (println (list x n))
        (if (zero? n) x (tail (func (* x 2) (- n 1))))
      )
    )
  )
  (println (func 2 10))
  ; (exit)
))",
    default_env);

  // REPL. Use "(exit)" to quit the program.
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
    const auto eval   = lisk::eval(expr, default_env, true);
    const auto result = lisk::to_string(eval);
    std::cout << "lisk$ " << result << "\n";
  }
  std::cout << "\n";
}
