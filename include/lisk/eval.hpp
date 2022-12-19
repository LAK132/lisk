#ifndef LISK_EVAL_HPP
#	define LISK_EVAL_HPP

#	include "lisk/environment.hpp"

#	define LISK_EXPRESSION_FORWARD_ONLY
#	include "lisk/expression.hpp"

#	include "lisk/functor.hpp"

#	define LISK_POINTER_FORWARD_ONLY
#	include "lisk/pointer.hpp"

#	define LISK_SHARED_LIST_FORWARD_ONLY
#	include "lisk/shared_list.hpp"

#	include "lisk/string.hpp"

#	include <lak/tuple.hpp>

namespace lisk
{
	struct lambda;

	lak::pair<lisk::shared_list, size_t> eval_all(lisk::shared_list l,
	                                              lisk::environment &e,
	                                              bool allow_tail_eval);

	lisk::expression eval(const lisk::expression &exp,
	                      lisk::environment &e,
	                      bool allow_tail_eval);

	namespace impl
	{
		template<typename... TYPES>
		bool get_or_eval_arg_as(lisk::shared_list in_list,
		                        lisk::environment &e,
		                        bool allow_tail,
		                        lisk::exception &exc,
		                        lak::tuple<TYPES...> &out_arg);
	}

	template<typename T>
	struct list_reader_traits
	{
		// Default to allowing both get-ing and eval-ing, this results in less work
		// to get user defined types working with lisk. If the user wants to modify
		// this behaviour, they can still override these values (see
		// specialisations below)
		static constexpr bool allow_get  = true;
		static constexpr bool allow_eval = true;
	};

	template<>
	struct list_reader_traits<lisk::shared_list>
	{
		static constexpr bool allow_get  = false;
		static constexpr bool allow_eval = true;
	};

	template<>
	struct list_reader_traits<lisk::eval_shared_list>
	{
		static constexpr bool allow_get  = false;
		static constexpr bool allow_eval = true;
	};

	template<>
	struct list_reader_traits<lisk::expression>
	{
		static constexpr bool allow_get  = false;
		static constexpr bool allow_eval = true;
	};

	template<>
	struct list_reader_traits<lisk::eval_expr>
	{
		static constexpr bool allow_get  = false;
		static constexpr bool allow_eval = true;
	};

	template<>
	struct list_reader_traits<lisk::uneval_shared_list>
	{
		static constexpr bool allow_get  = true;
		static constexpr bool allow_eval = false;
	};

	template<>
	struct list_reader_traits<lisk::uneval_expr>
	{
		static constexpr bool allow_get  = true;
		static constexpr bool allow_eval = false;
	};

	struct list_reader
	{
		lisk::shared_list list;
		lisk::environment env;
		bool allow_tail_eval;

		list_reader(lisk::shared_list l, lisk::environment e, bool allow_tail)
		: list(l), env(e), allow_tail_eval(allow_tail)
		{
		}

		inline operator bool() const { return list; }

		template<typename T>
		bool operator>>(T &out);
	};
}

#	define LISK_EVAL_HPP_FINISHED
#endif

#ifdef LISK_EVAL_FORWARD_ONLY
#	undef LISK_EVAL_FORWARD_ONLY
#else
#	if defined(LISK_EVAL_HPP_FINISHED) && !defined(LISK_EVAL_HPP_IMPL)
#		define LISK_EVAL_HPP_IMPL
#		include "eval.inl"
#	endif
#endif
