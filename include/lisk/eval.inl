#include "eval.hpp"

namespace lisk
{
	template<typename...>
	struct _false : std::bool_constant<false>
	{
	};

	template<typename T>
	bool list_reader::operator>>(T &out)
	{
		static_assert(list_reader_traits<T>::allow_get ||
		                list_reader_traits<T>::allow_eval,
		              "Type must be get-able or eval-able");

		if (!list) return false;

		if constexpr (list_reader_traits<T>::allow_get &&
		              list_reader_traits<T>::allow_eval)
		{
			if (list.value() >> out ||
			    eval(list.value(), env, allow_tail_eval) >> out)
			{
				++list;
				return true;
			}
		}
		else if constexpr (list_reader_traits<T>::allow_get)
		{
			if (list.value() >> out)
			{
				++list;
				return true;
			}
		}
		else if constexpr (list_reader_traits<T>::allow_eval)
		{
			if (eval(list.value(), env, allow_tail_eval) >> out)
			{
				++list;
				return true;
			}
		}
		return false;
	}

	template<typename... TYPES, size_t... I>
	inline bool _get_or_eval_arg_as(shared_list in_list,
	                                environment &e,
	                                bool allow_tail,
	                                exception &exc,
	                                std::tuple<TYPES...> &out_arg,
	                                std::index_sequence<I...>)
	{
		std::tuple<std::remove_cv_t<TYPES>...> result;

		list_reader reader(in_list, e, allow_tail);

		[[maybe_unused]] auto _get_or_eval = [&](auto &&element, auto i) -> bool
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
		if ((_get_or_eval(std::get<I>(result), I) && ...))
		{
			out_arg = result;
			return true;
		}
		else
			return false;
	}

	template<typename... TYPES>
	bool get_or_eval_arg_as(shared_list in_list,
	                        environment &e,
	                        bool allow_tail,
	                        exception &exc,
	                        std::tuple<TYPES...> &out_arg)
	{
		return _get_or_eval_arg_as(in_list,
		                           e,
		                           allow_tail,
		                           exc,
		                           out_arg,
		                           std::index_sequence_for<TYPES...>{});
	}

	inline bool get_or_eval_arg_as(
	  shared_list, environment &, bool, exception &, std::tuple<> &)
	{
		return true;
	}
}
