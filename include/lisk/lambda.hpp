#ifndef LISK_LAMBDA_HPP
#define LISK_LAMBDA_HPP

#define LISK_ENVIRONMENT_FORWARD_ONLY
#include "lisk/environment.hpp"

#define LISK_EVAL_FORWARD_ONLY
#include "lisk/eval.hpp"

#define LISK_SHARED_LIST_FORWARD_ONLY
#include "lisk/shared_list.hpp"

namespace lisk
{
	struct lambda
	{
		lisk::shared_list params;
		lisk::shared_list exp;
		lisk::environment captured_env;

		lambda()               = default;
		lambda(const lambda &) = default;
		lambda &operator=(const lambda &) = default;

		lambda(lisk::shared_list l, lisk::environment &e, bool allow_tail_eval);

		lak::pair<lisk::expression, size_t> operator()(lisk::shared_list l,
		                                               lisk::environment &e,
		                                               bool allow_tail_eval) const;
	};

	lisk::string to_string(const lisk::lambda &l);
	const lisk::string &type_name(const lisk::lambda &);

	struct expression;
}

bool operator>>(const lisk::expression &arg, lisk::lambda &out);

#endif
