#pragma once

#include <type_traits>

namespace Rearm::Reflection
{
	template<typename T, int N, typename Tuple>
	struct tuple_element_index_helper;

	template<typename T, int N>
	struct tuple_element_index_helper<T, N, std::tuple<>>
	{
		static constexpr std::size_t value = 0;
	};

	template<typename T, typename... Rest>
	struct tuple_element_index_helper<T, 0, std::tuple<T, Rest...>>
	{
		using RestTuple = std::tuple<Rest...>;
		static constexpr std::size_t value = 0;
	};

	template<typename T, int N, typename... Rest>
	struct tuple_element_index_helper<T, N, std::tuple<T, Rest...>>
	{
		using RestTuple = std::tuple<Rest...>;
		static constexpr std::size_t value = 1 + tuple_element_index_helper<T, N - 1, RestTuple>::value;
	};

	template<typename T, int N, typename First, typename... Rest>
	struct tuple_element_index_helper<T, N, std::tuple<First, Rest...>>
	{
		using RestTuple = std::tuple<Rest...>;
		static constexpr std::size_t value = 1 + tuple_element_index_helper<T, N, RestTuple>::value;
	};

	template<typename T, typename Tuple, int N = 0>
	struct tuple_element_index
	{
		static constexpr std::size_t value = tuple_element_index_helper<T, N, Tuple>::value;
		static_assert(N >= 0, "must provide a positive integer index");
		static_assert(value < std::tuple_size_v<Tuple>, "type does not appear in tuple");
	};

	template<typename T, typename Tuple, int N = 0>
	constexpr std::size_t tuple_element_index_v = tuple_element_index<T, Tuple, N>::value;

	/**
	 * \brief Return a value from a variable list of parameters with the given type T
	 * \tparam T The type to retrieve
	 * \tparam N If there are multiple of the same type, which to return in order of occurrence
	 * \tparam TArgs The types of the variable list of parameters
	 * \param args The variable list of parameters
	 * \return 
	 */
	template<typename T, int N = 0, typename... TArgs>
	constexpr T Pick(TArgs ...args) noexcept
	{
		std::tuple<TArgs...> tuple = std::make_tuple<TArgs...>(std::forward<TArgs>(args)...);

		constexpr std::size_t index = tuple_element_index_v<T, decltype(tuple), N>;

		return std::get<index>(tuple);
	}

	// inspired by https://devblogs.microsoft.com/oldnewthing/20190711-00/?p=102682
	
	template<typename, typename = void>
	struct is_complete_type : std::false_type {};
	
	template<typename T>
	struct is_complete_type<T, std::void_t<decltype(sizeof(T))>> : std::true_type {};

	template<typename T>
	constexpr bool is_complete_type_v = is_complete_type<T>::value;
}