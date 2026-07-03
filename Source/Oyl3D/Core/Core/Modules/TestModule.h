#pragma once

#include "Core/Application/Module.h"
#include "Core/Common.h"
#include "Core/Reflection/Attribute.h"
#include "Core/Reflection/TypeId.h"

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

	class OYL_CORE_API TestModule : public Module
	{
	public:
		constexpr Reflection::TypeId
		GetTypeId() const override
		{
			return Reflection::GetTypeId<decltype(*this)>();
		}

		constexpr std::string_view
		GetName() const override
		{
			return NameOf<decltype(*this)>();
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
		Attr(TypeAttribute(typeidof(TestModule)))
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
