#include "pch.h"

template<int N, typename TFirst, typename... TRest>
struct num_arguments_impl;

template<int N, typename TFirst>
struct num_arguments_impl<N, TFirst>
{
	constexpr static int value = N;
};

template<int N, typename TFirst, typename... TRest>
struct num_arguments_impl
{
	constexpr static int value = num_arguments_impl<N + 1, TRest...>::value;
};

template<typename... TRest>
struct num_arguments
{
	constexpr static int value = num_arguments_impl<1, TRest...>::value;
};

template<typename... TRest>
constexpr int num_arguments_v = num_arguments<TRest...>::value;

template<int N, typename TFirst, typename... TRest>
struct emplace_name_in_array;

template<int N, typename TFirst>
struct emplace_name_in_array<N, TFirst>
{
	static void
	call(const char* a_array[N], int a_position = 0)
	{
		a_array[a_position] = typeid(TFirst).name();
	}
};

template<int N, typename TFirst, typename... TRest>
struct emplace_name_in_array
{
	static void
	call(const char* a_array[N], int a_position = 0)
	{
		a_array[a_position] = typeid(TFirst).name();
		emplace_name_in_array<N, TRest...>::call(a_array, a_position + 1);
	}
};

template<int N, typename TFirst, typename... TRest>
void
EmplaceTypeNamesInArray(const char* a_array[N], int a_position = 0)
{
	emplace_name_in_array<N, TFirst, TRest...>::call(a_array, a_position);
}

class Something
{
public:
	int
	Foo(double a_v, Something& a_s) const
	{
		return 0;
	}
};

template<typename T, typename TReturn, typename... TArgs>
void
GetMethod(TReturn (T::*a_method)(TArgs ...))
{
	//constexpr int numArguments = num_arguments_v<TArgs...>;
	//printf("%d", numArguments);
	printf("%s, %s, %s", typeid(T).name(), typeid(TReturn).name(), typeid(TArgs).name());
}

template<typename T, typename TReturn, typename... TArgs>
void
GetMethod(TReturn (T::*a_method)(TArgs ...) const)
{
	constexpr int numArguments = num_arguments_v<TArgs...>;
	const char*   argNames[numArguments];
	EmplaceTypeNamesInArray<numArguments, TArgs...>(argNames);

	printf("%s, %s", typeid(T).name(), typeid(TReturn).name());
	for (const char* str : argNames)
	{
		printf(", %s", str);
	}

	printf("\n");
}

void
Testing()
{
	GetMethod(&Something::Foo);

	class Base
	{
	public:
		void Foo() {}

		static void Bar() {}
	};

	static_assert(std::is_member_function_pointer_v<decltype(&Base::Foo)>);
	static_assert(!std::is_member_function_pointer_v<decltype(&Base::Bar)>);
}
