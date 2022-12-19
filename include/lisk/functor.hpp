#ifndef LISK_FUNCTOR_HPP
#define LISK_FUNCTOR_HPP

#define LISK_SHARED_LIST_FORWARD_ONLY
#include "lisk/shared_list.hpp"

#include <lak/tuple.hpp>
#include <lak/type_traits.hpp>
#include <lak/variant.hpp>

namespace lisk
{
	struct expression;
	struct environment;

	template<typename FUNC>
	struct function_signature;

	template<typename R, typename... ARGS>
	struct function_signature<R (*)(ARGS...)>
	{
		using return_type = R;
		using arguments   = lak::tuple<ARGS...>;
	};

	template<typename R, typename... ARGS>
	struct function_signature<R(ARGS...)>
	{
		using return_type = R;
		using arguments   = lak::tuple<ARGS...>;
	};

	template<typename FUNC>
	using function_return_t = typename function_signature<FUNC>::return_type;

	template<typename FUNC>
	using function_arguments_t = typename function_signature<FUNC>::arguments;

	template<typename TUPLE>
	struct as_functor_arguments;

	template<typename... ARGS>
	struct as_functor_arguments<lak::tuple<environment &, bool, ARGS...>>
	{
		using arguments = lak::tuple<ARGS...>;
	};

	template<typename TUPLE>
	using as_functor_arguments_t =
	  typename as_functor_arguments<TUPLE>::arguments;

	typedef lak::pair<lisk::expression, size_t> (*functor)(
	  lisk::basic_shared_list<lisk::expression>, lisk::environment &, bool);

#define LISK_FUNCTOR_WRAPPER(F)                                               \
	static_cast<lisk::functor>(                                                 \
	  [](lisk::shared_list l,                                                   \
	     lisk::environment &e,                                                  \
	     bool allow_tail) -> lak::pair<lisk::expression, size_t>                \
	  {                                                                         \
			static_assert(                                                          \
			  lak::is_same_v<                                                       \
			    lak::tuple_element_t<0, lisk::function_arguments_t<decltype(F)>>,   \
			    lisk::environment &>);                                              \
                                                                              \
			static_assert(                                                          \
			  lak::is_same_v<                                                       \
			    lak::tuple_element_t<1, lisk::function_arguments_t<decltype(F)>>,   \
			    bool>);                                                             \
                                                                              \
			static_assert(lak::is_same_v<lisk::function_return_t<decltype(F)>,      \
			                             lisk::expression>);                        \
                                                                              \
			using arguments_t = lisk::as_functor_arguments_t<                       \
			  lisk::function_arguments_t<decltype(F)>>;                             \
			static_assert(lak::is_tuple_v<arguments_t>);                            \
                                                                              \
			arguments_t args;                                                       \
			lisk::exception exc;                                                    \
                                                                              \
			if (!lisk::impl::get_or_eval_arg_as(l, e, allow_tail, exc, args))       \
			{                                                                       \
				return lak::pair<lisk::expression, size_t>(exc, 0);                   \
			}                                                                       \
			else                                                                    \
			{                                                                       \
				lisk::expression exp{lak::apply(                                      \
				  F, lak::tuple_cat(lak::forward_as_tuple(e, allow_tail), args))};    \
                                                                              \
				return lak::pair<lisk::expression, size_t>(                           \
				  lak::move(exp), lak::tuple_size_v<arguments_t>);                    \
			}                                                                       \
	  })

	lisk::string to_string(lisk::functor f);
	const lisk::string &type_name(const lisk::functor &);

	struct expression;
}

bool operator>>(const lisk::expression &arg, lisk::functor &out);

#endif
