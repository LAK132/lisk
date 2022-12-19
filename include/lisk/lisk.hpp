#ifndef LISK_HPP
#define LISK_HPP

#include "lisk/atom.hpp"
#include "lisk/callable.hpp"
#include "lisk/environment.hpp"
#include "lisk/eval.hpp"
#include "lisk/expression.hpp"
#include "lisk/functor.hpp"
#include "lisk/lambda.hpp"
#include "lisk/number.hpp"
#include "lisk/pointer.hpp"
#include "lisk/shared_list.hpp"

#include <lak/array.hpp>
#include <lak/memory.hpp>
#include <lak/string.hpp>
#include <lak/tuple.hpp>
#include <lak/variant.hpp>

#include <regex>
#include <unordered_map>

namespace lisk
{
	extern const std::regex numeric_regex;

	bool is_whitespace(const char c);
	bool is_bracket(const char c);
	bool is_numeric(const lisk::string &token);

	bool is_nil(const lisk::expression &expr);
	bool is_null(const lisk::expression &expr);

	lak::vector<lisk::string> tokenise(const lisk::string &str,
	                                   size_t *chars_used = nullptr);
	lak::vector<lisk::string> root_tokenise(const lisk::string &str,
	                                        size_t *chars_used = nullptr);

	lisk::number parse_number(const lisk::string &token);
	lisk::string parse_string(const lisk::string &token);
	lisk::expression parse(const lak::vector<lisk::string> &tokens);

	// Top level eval function.
	lisk::expression eval_string(const lisk::string &str,
	                             lisk::environment &env);
	lisk::expression root_eval_string(const lisk::string &str,
	                                  lisk::environment &env);

	// Delays evaluation of the expression.
	lisk::expression tail_eval(lisk::expression expr,
	                           lisk::environment &env,
	                           bool allow_tail);

	template<typename T>
	lisk::expression type_error(const lisk::string &message,
	                            const T &t,
	                            const lisk::string &expected)
	{
		return lisk::exception{message + ": '" + to_string(t) + "' is '" +
		                       type_name(t) + "', expected " + expected};
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
		lak::vector<lisk::string> token_buffer;
		lak::vector<lak::vector<lisk::string>> tokens;

		void clear();

		operator bool() const;

		iterator begin();

		iterator::sentinel end() const;

		reader &operator+=(const lisk::string &str);
	};

	namespace builtin
	{
		/* --- env --- */

		lisk::expression list_env(lisk::environment &env, bool allow_tail);

		lisk::environment default_env();

		/* --- check --- */

		lisk::expression null_check(lisk::environment &env,
		                            bool allow_tail,
		                            lisk::expression exp);

		lisk::expression nil_check(lisk::environment &env,
		                           bool allow_tail,
		                           lisk::expression exp);

		lisk::expression zero_check(lisk::environment &env,
		                            bool allow_tail,
		                            lisk::number num);

		// expr equal_check(lisk::shared_list l, lisk::environment &env);

		/* --- --- */

		lisk::expression conditional(lisk::environment &env,
		                             bool allow_tail,
		                             bool b,
		                             lisk::uneval_expr cond,
		                             lisk::uneval_expr alt);

		/* --- --- */

		lisk::expression define(lisk::environment &env,
		                        bool allow_tail,
		                        lisk::symbol sym,
		                        lisk::expression exp);

		lisk::expression evaluate(lisk::environment &env,
		                          bool allow_tail,
		                          lisk::expression exp);

		lak::pair<lisk::expression, size_t> evaluate_stack(lisk::shared_list l,
		                                                   lisk::environment &env,
		                                                   bool allow_tail);

		/* --- --- */

		lak::pair<lisk::expression, size_t> begin(lisk::shared_list l,
		                                          lisk::environment &env,
		                                          bool allow_tail);

		lisk::expression repeat(lisk::environment &env,
		                        bool allow_tail,
		                        lisk::uint_t count,
		                        lisk::uneval_expr exp);

		lisk::expression repeat_while(lisk::environment &env,
		                              bool allow_tail,
		                              lisk::uneval_expr exp);

		lisk::expression foreach (lisk::environment &env,
		                          bool allow_tail,
		                          lisk::symbol sym,
		                          lisk::shared_list iterlist,
		                          lisk::uneval_expr exp);

		lisk::expression map(lisk::environment &env,
		                     bool allow_tail,
		                     lisk::shared_list iterlist,
		                     lisk::uneval_expr exp);

		lak::pair<lisk::expression, size_t> tail_call(lisk::shared_list l,
		                                              lisk::environment &env,
		                                              bool allow_tail);

		/* --- --- */

		lisk::expression car(lisk::environment &env,
		                     bool allow_tail,
		                     lisk::shared_list l);

		lisk::expression cdr(lisk::environment &env,
		                     bool allow_tail,
		                     lisk::shared_list l);

		lisk::expression cons(lisk::environment &env,
		                      bool allow_tail,
		                      lisk::expression exp,
		                      lisk::shared_list l);

		lak::pair<lisk::expression, size_t> join(lisk::shared_list l,
		                                         lisk::environment &env,
		                                         bool allow_tail);

		/* ---  --- */

		lisk::expression range_list(lisk::environment &env,
		                            bool allow_tail,
		                            lisk::number start,
		                            lisk::uint_t count,
		                            lisk::number step);

		/* --- constructor --- */

		lak::pair<lisk::expression, size_t> make_list(lisk::shared_list l,
		                                              lisk::environment &env,
		                                              bool allow_tail);

		lak::pair<lisk::expression, size_t> make_lambda(lisk::shared_list l,
		                                                lisk::environment &env,
		                                                bool allow_tail);

		lisk::expression make_uint(lisk::environment &env,
		                           bool allow_tail,
		                           lisk::expression exp);

		lisk::expression make_sint(lisk::environment &env,
		                           bool allow_tail,
		                           lisk::expression exp);

		lisk::expression make_real(lisk::environment &env,
		                           bool allow_tail,
		                           lisk::expression exp);

		lisk::expression make_string(lisk::environment &env,
		                             bool allow_tail,
		                             lisk::expression exp);

		/* --- string --- */

		lisk::expression read_string(lisk::environment &, bool allow_tail);

		lisk::expression parse_string(lisk::environment &env,
		                              bool allow_tail,
		                              lisk::string str);

		lak::pair<lisk::expression, size_t> print_string(lisk::shared_list l,
		                                                 lisk::environment &env,
		                                                 bool allow_tail);

		lak::pair<lisk::expression, size_t> print_line(lisk::shared_list l,
		                                               lisk::environment &env,
		                                               bool allow_tail);

		/* --- math --- */

		lisk::expression add(lisk::environment &env,
		                     bool allow_tail,
		                     lisk::number a,
		                     lisk::number b);

		lisk::expression sub(lisk::environment &env,
		                     bool allow_tail,
		                     lisk::number a,
		                     lisk::number b);

		lisk::expression mul(lisk::environment &env,
		                     bool allow_tail,
		                     lisk::number a,
		                     lisk::number b);

		lisk::expression div(lisk::environment &env,
		                     bool allow_tail,
		                     lisk::number a,
		                     lisk::number b);

		lak::pair<lisk::expression, size_t> sum(lisk::shared_list l,
		                                        lisk::environment &env,
		                                        bool allow_tail);

		lak::pair<lisk::expression, size_t> product(lisk::shared_list l,
		                                            lisk::environment &env,
		                                            bool allow_tail);

	};
}

#endif
