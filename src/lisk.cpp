#include "lisk/lisk.hpp"

#include "lak/array.hpp"
#include "lak/span_manip.hpp"

#include <iostream>

const std::regex lisk::numeric_regex(
  "(?:([\\-\\+])?(\\d+)(\\.\\d+)?)|"
  "(?:([\\-\\+])?0x([a-f\\d]+)(\\.[a-f\\d]+)?)|"
  "(?:([\\-\\+])?0b([01]+))",
  std::regex_constants::ECMAScript | std::regex_constants::icase |
    std::regex_constants::optimize);

bool lisk::is_whitespace(const char c)
{
	switch (c)
	{
		case ' ':
		case '\n':
		case '\r':
		case '\t':
			return true;
		default:
			return false;
	}
}

bool lisk::is_bracket(const char c)
{
	switch (c)
	{
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
			return true;
		default:
			return false;
	}
}

bool lisk::is_numeric(const lisk::string &token)
{
	return std::regex_match(token, lisk::numeric_regex);
}

bool lisk::is_nil(const lisk::expression &expr)
{
	return expr.get_list().map_or([](auto &&l) { return l.value().is_null(); },
	                              false) ||
	       expr.get_atom().map_or([](auto &&a) { return a.is_nil(); }, false);
}

bool lisk::is_null(const lisk::expression &expr)
{
	return expr.is_null();
}

lak::vector<lisk::string> lisk::tokenise(const lisk::string &str,
                                         size_t *chars_used)
{
	lak::vector<lisk::string> result;
	lisk::string buffer;
	size_t chars_read = 0;

	auto begin_next = [&]
	{
		if (!buffer.empty()) result.emplace_back(lak::move(buffer));
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
		else if (lisk::is_whitespace(c))
		{
			if (!buffer.empty()) begin_next();
		}
		else if (lisk::is_bracket(c))
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

lak::vector<lisk::string> lisk::root_tokenise(const lisk::string &str,
                                              size_t *chars_used)
{
	auto result = lisk::tokenise(str, chars_used);
	if (result.empty()) return {};

	result.reserve(result.size() + 3);
	result.insert(result.begin(), {"(", "begin"});
	result.push_back(")");
	return result;
}

lisk::number lisk::parse_number(const lisk::string &token)
{
	lisk::number result;

	std::smatch match;

	if (!std::regex_match(token, match, lisk::numeric_regex) ||
	    match.size() != 9)
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
			result = static_cast<lisk::sint_t>(
			  std::stoll(match[1].str() + match[2].str(), nullptr, 10));
		}
		else
		{
			// unsigned int
			result =
			  static_cast<lisk::uint_t>(std::stoull(match[2].str(), nullptr, 10));
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
			result = static_cast<lisk::sint_t>(
			  std::stoll(match[4].str() + match[5].str(), nullptr, 16));
		}
		else
		{
			// unsigned int
			result =
			  static_cast<lisk::uint_t>(std::stoull(match[5].str(), nullptr, 16));
		}
	}
	else if (match[8].matched)
	{
		if (match[7].matched)
		{
			// signed int
			result = static_cast<lisk::sint_t>(
			  std::stoll(match[7].str() + match[8].str(), nullptr, 2));
		}
		else
		{
			// unsigned int
			result =
			  static_cast<lisk::uint_t>(std::stoull(match[8].str(), nullptr, 2));
		}
	}
	else
		return std::numeric_limits<lisk::real_t>::signaling_NaN();

	return result;
}

lisk::string lisk::parse_string(const lisk::string &token)
{
	if (token.size() == 2) return lisk::string{};
	return lisk::string{token.substr(1, token.size() - 2)};
}

lisk::expression lisk::parse(const lak::vector<lisk::string> &tokens)
{
	lisk::shared_list root;
	lak::vector<lak::vector<lisk::shared_list>> stack;

	auto push_element = [&]() -> lisk::shared_list
	{
		if (stack.empty()) return {};
		// Get the previous nill element.
		lisk::shared_list old_element = stack.back().back();
		// If this element holds null_t then no values have been added to this
		// scope yet, so return immediately.
		if (!old_element) return old_element;
		// Else if the last element already has a value, add a new element after
		// it
		auto new_element = lisk::shared_list::create();
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
		auto scope    = lisk::shared_list::create();
		scope.value() = lisk::expression::null{};

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
			push_element().value() = lisk::atom{lisk::symbol("list")};
		}
		else if (c == '{')
		{
			push_scope();
			push_element().value() = lisk::atom{lisk::symbol("eval-stack")};
		}
		else if (c == ')' || c == ']' || c == '}')
		{
			pop_scope();
		}
		else
		{
			expression value;

			if (c == '"')
				value = lisk::atom{lisk::parse_string(token)};
			else if (token == "nil")
				value = lisk::atom{lisk::atom::nil{}};
			else if (token == "true")
				value = lisk::atom{true};
			else if (token == "false")
				value = lisk::atom{false};
			else if (is_numeric(token))
				value = lisk::atom{lisk::parse_number(token)};
			else
				value = lisk::atom{lisk::symbol(token)};

			if (stack.empty())
				return value;
			else
				push_element().value() = value;
		}
	}

	return lisk::expression{root};
}

lisk::expression lisk::eval_string(const lisk::string &str,
                                   lisk::environment &env)
{
	return lisk::eval(lisk::parse(lisk::tokenise(str)), env, true);
}

lisk::expression lisk::root_eval_string(const lisk::string &str,
                                        lisk::environment &env)
{
	return lisk::eval(lisk::parse(lisk::root_tokenise(str)), env, true);
}

lisk::expression lisk::tail_eval(lisk::expression expr,
                                 lisk::environment &env,
                                 bool allow_tail)
{
	// grandp   parent   env
	// * ------ * ------ *
	//  `.
	//    `.
	//      `* ------ * tail_env (env clone)
	//    parent (clone)
	lisk::environment tail_env = env.clone(2);
	// grandp   parent   env
	// * ------ * ------ *
	//  `.
	//    `.
	//      `* tail_env (squashed env->parent clone)
	tail_env.squash(2);

	auto result = lisk::shared_list::create();

	// (lambda () (expr))
	lisk::shared_list lambda_list;
	lambda_list.value()      = lisk::shared_list::create(); // no arguments
	lambda_list.next_value() = expr;                        // lambda body
	result.value() =
	  lisk::callable(lisk::lambda(lambda_list, tail_env, allow_tail));

	// Return an eval_shared_list containing a lambda that immediately
	// evaluates our expr.
	return lisk::eval_shared_list{result};
}

bool lisk::reader::iterator::operator==(sentinel) const
{
	return !ref;
}

bool lisk::reader::iterator::operator!=(sentinel) const
{
	return ref;
}

lisk::expression lisk::reader::iterator::operator*()
{
	return lisk::eval(
	  lisk::parse(ref.tokens.front()), ref.env, ref.allow_tail_eval);
}

lisk::reader::iterator &lisk::reader::iterator::operator++()
{
	ref.tokens.pop_front();
	return *this;
}

lisk::reader::reader(const lisk::environment e, bool allow_tail)
: env(e), allow_tail_eval(allow_tail)
{
}

void lisk::reader::clear()
{
	string_buffer.clear();
	token_buffer.clear();
	tokens.clear();
}

lisk::reader::operator bool() const
{
	return tokens.size() > 0;
}

lisk::reader::iterator lisk::reader::begin()
{
	return {*this};
}

lisk::reader::iterator::sentinel lisk::reader::end() const
{
	return {};
}

lisk::reader &lisk::reader::operator+=(const lisk::string &str)
{
	string_buffer += str;

	// Tokenise as much of the buffer as we can.
	size_t chars_used;
	auto new_tokens = lisk::tokenise(string_buffer, &chars_used);
	string_buffer.erase(string_buffer.begin(),
	                    string_buffer.begin() + chars_used);

	// Push the new tokens into the reader's token buffer.
	token_buffer.reserve(token_buffer.size() + new_tokens.size());
	for (auto &token : new_tokens) token_buffer.emplace_back(lak::move(token));

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
				tokens.back().emplace_back(lak::move(*it2));

			token_buffer.erase(begin, end);

			it = token_buffer.begin();
			if (it == token_buffer.end()) break;
		}
	}

	return *this;
}

lisk::expression lisk::builtin::list_env(lisk::environment &env, bool)
{
	auto root = lisk::shared_list::create();

	lisk::shared_list previous = root;
	lisk::shared_list l        = root;
	for (const auto &node : env._map)
	{
		for (const auto &[key, value] : node.value)
		{
			auto entry         = lisk::shared_list::create();
			entry.value()      = lisk::atom{key};
			entry.next_value() = value;

			l.value() = entry;
			l.set_next(lisk::shared_list::create());

			previous = l++;
		}
	}
	previous.clear_next();

	return root;
}

lisk::expression lisk::builtin::null_check(lisk::environment &,
                                           bool,
                                           lisk::expression exp)
{
	return lisk::atom{lisk::is_null(exp)};
}

lisk::expression lisk::builtin::nil_check(lisk::environment &,
                                          bool,
                                          lisk::expression exp)
{
	return lisk::atom{lisk::is_nil(exp)};
}

lisk::expression lisk::builtin::zero_check(lisk::environment &,
                                           bool,
                                           lisk::number num)
{
	return lisk::atom{num.visit([](auto &&n) -> bool { return n == 0; })};
}

// lisk::expression equal_check(lisk::shared_list l, lisk::environment &)
// {
//   if (lisk::eval(l.value(), e) == lisk::eval(l.next().value(), e))
//     return lisk::expression{lisk::atom{lisk::number{lisk::uint_t{1U}}}};
//   else
//     return lisk::atom::nil{};
// }

lisk::expression lisk::builtin::conditional(lisk::environment &env,
                                            bool allow_tail,
                                            bool b,
                                            lisk::uneval_expr cond,
                                            lisk::uneval_expr alt)
{
	return b ? lisk::eval(cond.expr, env, allow_tail)
	         : lisk::eval(alt.expr, env, allow_tail);
}

lisk::expression lisk::builtin::define(lisk::environment &env,
                                       bool,
                                       lisk::symbol sym,
                                       lisk::expression exp)
{
	env.define_expr(sym, exp);
	return lisk::atom::nil{};
}

lisk::expression lisk::builtin::evaluate(lisk::environment &env,
                                         bool allow_tail,
                                         lisk::expression exp)
{
	return lisk::eval(exp, env, allow_tail);
}

lak::pair<lisk::expression, size_t> lisk::builtin::evaluate_stack(
  lisk::shared_list l, lisk::environment &env, bool allow_tail)
{
	lisk::shared_list head;
	size_t input_length = 0;
	for (; l; ++l, ++input_length)
	{
		auto expr = lisk::eval(l.value(), env, allow_tail);
		if (lisk::callable c; expr >> c)
		{
			auto [result, pop_count] = c(head, env, allow_tail);
			for (size_t i = 0; i < pop_count; ++i) ++head;
			if (head) head = lisk::shared_list::extends(head);
			head.value() = lak::move(result);
		}
		else
		{
			if (head) head = lisk::shared_list::extends(head);
			head.value() = expr;
		}
	}

	return {lisk::expression{head}, input_length};
}

lak::pair<lisk::expression, size_t> lisk::builtin::begin(
  lisk::shared_list l, lisk::environment &env, bool allow_tail)
{
	// Evaluate every element in the list and return the result of the last.
	lak::pair<lisk::expression, size_t> result;
	result.second = 0;
	for (const auto &node : l)
	{
		result.first = lisk::eval(node.value, env, allow_tail);
		++result.second;
	}
	return result;
}

lisk::expression lisk::builtin::repeat(lisk::environment &env,
                                       bool allow_tail,
                                       lisk::uint_t count,
                                       lisk::uneval_expr exp)
{
	while (count-- > 0) lisk::eval(exp.expr, env, allow_tail);
	return lisk::atom::nil{};
}

lisk::expression lisk::builtin::repeat_while(lisk::environment &env,
                                             bool allow_tail,
                                             lisk::uneval_expr exp)
{
	while (!lisk::is_nil(lisk::eval(exp.expr, env, allow_tail)))
		;
	return lisk::atom::nil{};
}

lisk::expression lisk::builtin::foreach (lisk::environment &env,
                                         bool allow_tail,
                                         lisk::symbol sym,
                                         lisk::shared_list iterlist,
                                         lisk::uneval_expr exp)
{
	for (const auto &node : iterlist)
	{
		lisk::environment new_env = env;
		new_env.define_expr(sym, node.value);
		lisk::eval(exp.expr, new_env, allow_tail);
	}
	return lisk::atom::nil{};
}

lisk::expression lisk::builtin::map(lisk::environment &env,
                                    bool allow_tail,
                                    lisk::shared_list iterlist,
                                    lisk::uneval_expr exp)
{
	auto subexp = lisk::eval(exp.expr, env, allow_tail);
	if (lisk::functor f; subexp >> f)
	{
		auto arg    = lisk::shared_list::create();
		auto result = lisk::shared_list::create();
		auto end    = result;
		for (const auto &node : iterlist)
		{
			arg.value()      = node.value;
			end.next_value() = f(arg, env, allow_tail).first;
			++end;
		}
		return ++result;
	}
	else if (lisk::lambda lf; subexp >> lf)
	{
		auto arg    = lisk::shared_list::create();
		auto result = lisk::shared_list::create();
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
		  { return lisk::type_error("Map error", a, "a function or lambda"); });
}

lak::pair<lisk::expression, size_t> lisk::builtin::tail_call(
  lisk::shared_list list, lisk::environment &env, bool allow_tail)
{
	// (tail (func args...))
	return {lisk::tail_eval(list.value(), env, allow_tail), 1};
}

lisk::expression lisk::builtin::car(lisk::environment &,
                                    bool,
                                    lisk::shared_list l)
{
	return l.value();
}

lisk::expression lisk::builtin::cdr(lisk::environment &,
                                    bool,
                                    lisk::shared_list l)
{
	return l.next();
}

lisk::expression lisk::builtin::cons(lisk::environment &,
                                     bool,
                                     lisk::expression exp,
                                     lisk::shared_list l)
{
	lisk::shared_list result;
	result.value() = exp;
	result.set_next(l);
	return result;
}

lak::pair<lisk::expression, size_t> lisk::builtin::join(lisk::shared_list l,
                                                        lisk::environment &env,
                                                        bool allow_tail)
{
	lisk::shared_list first;
	lisk::shared_list end;
	size_t count = 0;

	if (lisk::eval(l.value(), env, allow_tail) >> first)
	{
		++count;
		end = first.last();
	}
	else
	{
		return {lisk::type_error("Join error", l.value(), "a list"), 0};
	}

	for (const auto &node : l.next())
	{
		++count;
		if (lisk::shared_list next;
		    lisk::eval(node.value, env, allow_tail) >> next)
		{
			end.set_next(next);
			end = next.last();
		}
		else
		{
			return {lisk::type_error("Join error", node.value, "a list"), 0};
		}
	}

	return {first, count};
}

lisk::expression lisk::builtin::range_list(lisk::environment &,
                                           bool,
                                           lisk::number start,
                                           lisk::uint_t count,
                                           lisk::number step)
{
	auto result = lisk::shared_list::create();
	auto end    = result;
	for (lisk::uint_t i = 0; i < count; ++i)
	{
		end.next_value() = lisk::atom{start + (step * lisk::number{i})};
		++end;
	}
	return ++result;
}

lak::pair<lisk::expression, size_t> lisk::builtin::make_list(
  lisk::shared_list l, lisk::environment &env, bool allow_tail)
{
	auto result{lisk::eval_all(l, env, allow_tail)};
	return {lisk::expression(result.first), result.second};
}

lak::pair<lisk::expression, size_t> lisk::builtin::make_lambda(
  lisk::shared_list l, lisk::environment &env, bool allow_tail)
{
	// :TODO: check that there's exactly 2 arguments
	return {lisk::callable(lisk::lambda(l, env, allow_tail)), 2};
}

lisk::expression lisk::builtin::make_uint(lisk::environment &,
                                          bool,
                                          lisk::expression exp)
{
	auto to_uint = [](auto &&n) -> lisk::number { return lisk::uint_t(n); };
	if (lisk::number n; exp >> n)
		return lisk::atom{n.visit(to_uint)};
	else if (lisk::string s; exp >> s)
		return lisk::atom{lisk::parse_number(s).visit(to_uint)};
	else
		return lisk::atom{lisk::parse_number(to_string(exp)).visit(to_uint)};
}

lisk::expression lisk::builtin::make_sint(lisk::environment &,
                                          bool,
                                          lisk::expression exp)
{
	auto to_sint = [](auto &&n) -> lisk::number { return lisk::sint_t(n); };
	if (lisk::number n; exp >> n)
		return lisk::atom{n.visit(to_sint)};
	else if (lisk::string s; exp >> s)
		return lisk::atom{lisk::parse_number(s).visit(to_sint)};
	else
		return lisk::atom{lisk::parse_number(to_string(exp)).visit(to_sint)};
}

lisk::expression lisk::builtin::make_real(lisk::environment &,
                                          bool,
                                          lisk::expression exp)
{
	auto to_real = [](auto &&n) -> lisk::number { return (lisk::real_t)(n); };
	if (lisk::number n; exp >> n)
		return lisk::atom{n.visit(to_real)};
	else if (lisk::string s; exp >> s)
		return lisk::atom{lisk::parse_number(s).visit(to_real)};
	else
		return lisk::atom{lisk::parse_number(to_string(exp)).visit(to_real)};
}

lisk::expression lisk::builtin::make_string(lisk::environment &,
                                            bool,
                                            lisk::expression exp)
{
	if (lisk::string s; exp >> s)
		return lisk::expression(s);
	else
		return lisk::expression(lisk::string{to_string(exp)});
}

lisk::expression lisk::builtin::read_string(lisk::environment &, bool)
{
	lak::astring str;
	std::getline(std::cin, str);
	if (std::cin.good())
		return lisk::expression{lisk::atom{lisk::string(lak::move(str))}};
	return lisk::atom::nil{};
}

lisk::expression lisk::builtin::parse_string(lisk::environment &,
                                             bool,
                                             lisk::string str)
{
	return lisk::parse(lisk::tokenise(str));
}

lak::pair<lisk::expression, size_t> lisk::builtin::print_string(
  lisk::shared_list l, lisk::environment &env, bool allow_tail)
{
	lisk::expression result = lisk::eval(l.value(), env, allow_tail);
	// If the list evaluates to a pure string, then print it verbatim.
	// Otherwise to_string the result.
	if (lisk::string str; result >> str)
		std::cout << str;
	else
		std::cout << to_string(result);
	return {lisk::atom::nil{}, 1};
}

lak::pair<lisk::expression, size_t> lisk::builtin::print_line(
  lisk::shared_list l, lisk::environment &env, bool allow_tail)
{
	// No arguments, just print a newline.
	if (lisk::is_nil(l)) std::cout << "\n";

	lisk::expression result = lisk::eval(l.value(), env, allow_tail);
	// If the list evaluates to a pure string, then print it verbatim.
	// Otherwise to_string the result.
	if (lisk::string str; result >> str)
		std::cout << str << "\n";
	else
		std::cout << to_string(result) << "\n";
	return {lisk::atom::nil{}, 1};
}

lisk::expression lisk::builtin::add(lisk::environment &,
                                    bool,
                                    lisk::number a,
                                    lisk::number b)
{
	return lisk::atom{a + b};
}

lisk::expression lisk::builtin::sub(lisk::environment &,
                                    bool,
                                    lisk::number a,
                                    lisk::number b)
{
	return lisk::atom{a - b};
}

lisk::expression lisk::builtin::mul(lisk::environment &,
                                    bool,
                                    lisk::number a,
                                    lisk::number b)
{
	return lisk::atom{a * b};
}

lisk::expression lisk::builtin::div(lisk::environment &,
                                    bool,
                                    lisk::number a,
                                    lisk::number b)
{
	return lisk::atom{a / b};
}

lak::pair<lisk::expression, size_t> lisk::builtin::sum(lisk::shared_list l,
                                                       lisk::environment &env,
                                                       bool allow_tail)
{
	if (!l) return {lisk::atom::nil{}, 0};

	lisk::number result;
	size_t count = 0;

	if (lisk::number n; lisk::eval(l.value(), env, allow_tail) >> n)
	{
		++count;
		result = n;
	}
	else
	{
		return {lisk::type_error("Add error", l.value(), "a number"), 0};
	}

	for (const auto &it : l.next())
	{
		++count;
		if (lisk::number n; lisk::eval(it.value, env, allow_tail) >> n)
		{
			result += n;
		}
		else
		{
			return {lisk::type_error("Add error", l.value(), "a number"), 0};
		}
	}

	return {lisk::expression{lisk::atom{result}}, count};
}

lak::pair<lisk::expression, size_t> lisk::builtin::product(
  lisk::shared_list l, lisk::environment &env, bool allow_tail)
{
	if (!l) return {lisk::atom::nil{}, 0};

	lisk::number result;
	size_t count = 0;

	if (lisk::number n; lisk::eval(l.value(), env, allow_tail) >> n)
	{
		++count;
		result = n;
	}
	else
	{
		return {lisk::type_error("Mul error", l.value(), "a number"), 0};
	}

	for (const auto &it : l.next())
	{
		++count;
		if (lisk::number n; lisk::eval(it.value, env, allow_tail) >> n)
		{
			result *= n;
		}
		else
		{
			return {lisk::type_error("Mul error", l.value(), "a number"), 0};
		}
	}

	return {lisk::expression{lisk::atom{result}}, count};
}

lisk::environment lisk::builtin::default_env()
{
	lisk::environment e;

	e.define_atom("pi", lisk::atom(lisk::number(3.14159L)));

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
