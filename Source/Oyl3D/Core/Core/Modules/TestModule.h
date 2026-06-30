#pragma once

#include "Core/Application/Module.h"
#include "Core/Common.h"

#if defined(__REFLECT_GENERATE__) || defined (__INTELLISENSE__)
#	define Attr(...) [[clang::annotate("__ATTR__", __VA_ARGS__)]]
#else
#	define Attr(...)
#endif

#include "Core/Reflection/TypeId.h"
#include "Core/Reflection/Attribute.h"

namespace Oyl
{
	struct SliderAttribute : Reflection::Attribute
	{
		constexpr
		SliderAttribute(float a_min, float a_max)
			: min(a_min),
			  max(a_max) {}

		float min;
		float max;
	};

	struct AnnotateAttribute : Reflection::Attribute
	{
		constexpr
		explicit
		AnnotateAttribute(std::string_view a_view)
			: annotation(a_view) {}

		std::string_view annotation;
	};

	struct Bruh
	{
		int a;
		int b;
	};

	struct Bro : Reflection::Attribute
	{
		constexpr
		explicit
		Bro(int a, Bruh b)
			: a(a),
			  b(b) {}

		int a;
		Bruh b;
	};

	struct TypeAttribute : Reflection::Attribute
	{
		constexpr
		explicit
		TypeAttribute(Reflection::TypeId a_type)
			: type(a_type) {}

		Reflection::TypeId type;
	};

	template<typename T>
	constexpr
	Reflection::TypeId
	TypeIdWrapper()
	{
		return Reflection::TypeId::Null;
	}

	namespace Reflection::Internal
	{
		constexpr
		::Oyl::Reflection::TypeId
		ConstExprType(std::string_view a_typename)
		{
			uint32_t hash = 5381;
			for (const auto& c : a_typename)
			{
				hash = (33 * hash) ^ c;
			}
			return static_cast<::Oyl::Reflection::TypeId>(hash);
		}
	}

	#define typeof(_type_) Oyl::Reflection::Internal::ConstExprType(type_name<_type_>())

	class OYL_CORE_API TestModule : public Module
	{
	public:
		Reflection::TypeId
		GetTypeId() override
		{
			return Reflection::GetTypeId<decltype(*this)>();
		}

		std::string_view
		GetName() const override
		{
			return "TestModule";
		}

		void
		OnInit() override
		{
			printf("%d %f", m_private, m_slider);
		}

		void
		OnUpdate() override {}

		void
		OnShutdown() override {}

	private:
		Attr(TypeAttribute(TypeIdWrapper<TestModule>()))
		int m_private = 0;

		static constexpr float MAX = 10.0f;

		Attr(SliderAttribute(0, MAX))
		Attr(AnnotateAttribute("Arbitrary String"))
		Attr(Bro(5, Bruh { 6, 7 }))
		float m_slider = 0.0f;
	};

	OYL_CORE_API
	inline
	void
	SomeFunction() {}

	OYL_CORE_API
	int
	g_bruh = 0;

	enum TestEnum
	{
		One,
		Two,
		Three
	};
}
