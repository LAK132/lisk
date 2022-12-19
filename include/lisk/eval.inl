#include "eval.hpp"

#include "lisk/expression.hpp"
#include "lisk/shared_list.hpp"

template<typename T>
bool lisk::list_reader::operator>>(T &out)
{
	static_assert(lisk::list_reader_traits<T>::allow_get ||
	                lisk::list_reader_traits<T>::allow_eval,
	              "Type must be get-able or eval-able");

	if (!list) return false;

	if constexpr (lisk::list_reader_traits<T>::allow_get &&
	              lisk::list_reader_traits<T>::allow_eval)
	{
		if (list.value() >> out ||
		    lisk::eval(list.value(), env, allow_tail_eval) >> out)
		{
			++list;
			return true;
		}
	}
	else if constexpr (lisk::list_reader_traits<T>::allow_get)
	{
		if (list.value() >> out)
		{
			++list;
			return true;
		}
	}
	else if constexpr (lisk::list_reader_traits<T>::allow_eval)
	{
		if (lisk::eval(list.value(), env, allow_tail_eval) >> out)
		{
			++list;
			return true;
		}
	}
	return false;
}

template<typename... TYPES>
bool lisk::impl::get_or_eval_arg_as<TYPES...>(lisk::shared_list in_list,
                                              lisk::environment &e,
                                              bool allow_tail,
                                              lisk::exception &exc,
                                              lak::tuple<TYPES...> &out_arg)
{
	auto _get_or_eval_arg_as =
	  []<typename... TYPES, size_t... I>(lisk::shared_list in_list,
	                                     lisk::environment & e,
	                                     bool allow_tail,
	                                     lisk::exception &exc,
	                                     lak::tuple<TYPES...> &out_arg,
	                                     lak::index_sequence<I...>)
	    ->bool
	{
		lak::tuple<lak::remove_cv_t<TYPES>...> result;

		lisk::list_reader reader(in_list, e, allow_tail);

		[[maybe_unused]] auto _get_or_eval = [&](auto &&element, size_t i) -> bool
		{
			if (!(reader >> element))
			{
				exc.message = "Failed to evaluate element " + std::to_string(i) +
				              " '" + to_string(reader.list.value()) + "' of '" +
				              to_string(reader.list) + "', expected type '" +
				              type_name(element) + "'";
				return false;
			}
			return true;
		};
		if ((_get_or_eval(result.template get<I>(), I) && ...))
		{
			out_arg = result;
			return true;
		}
		else
			return false;
	};

	if constexpr (sizeof...(TYPES) == 0)
		return true;
	else
		return _get_or_eval_arg_as(in_list,
		                           e,
		                           allow_tail,
		                           exc,
		                           out_arg,
		                           lak::index_sequence_for<TYPES...>{});
}
