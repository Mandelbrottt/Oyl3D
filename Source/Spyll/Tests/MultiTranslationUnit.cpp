#include <Spyll/ClangTool/ReflectionGenerator.h>

#include "ToolPreTest.h"

extern const std::string g_translationUnit1 = std::filesystem::absolute("MultiTranslationUnit_1.cpp").string();
extern const std::string g_translationUnit2 = std::filesystem::absolute("MultiTranslationUnit_2.cpp").string();

class MultiTranslationUnit : public ToolPreTest<MultiTranslationUnit>
{
protected:
	MultiTranslationUnit()
		: ToolPreTest(std::vector { g_translationUnit1, g_translationUnit2 }) {}

	using TypesList = std::vector<Spyll::TypeDescriptor>;

	TypesList::const_iterator
	GetIteratorForTypeWithName(
		const std::vector<Spyll::TypeDescriptor>& a_types,
		std::string_view a_name,
		const TypesList::const_iterator* a_begin = nullptr
	)
	{
		auto begin = a_types.begin();

		if (a_begin)
			begin = *a_begin;

		return std::find_if(
			begin,
			a_types.end(),
			[a_name](const Spyll::TypeDescriptor& a_type)
			{
				return a_type.name == a_name;
			}
		);
	}
};

TEST_F(MultiTranslationUnit, BaseAndDerived_IsInTranslationUnit1)
{
	const auto& reflectionDescriptor =
		tool->GetReflectionGeneratorMap()
			.at(g_translationUnit1)
			->GetReflectionDescriptor();

	const auto& types = reflectionDescriptor.types;

	{
		auto baseIter = GetIteratorForTypeWithName(types, "Base");

		EXPECT_NE(baseIter, types.end());
	}
	{
		auto derivedIter = GetIteratorForTypeWithName(types, "Derived");

		EXPECT_NE(derivedIter, types.end());
	}
}

TEST_F(MultiTranslationUnit, BaseAndDerived_IsInTranslationUnit2)
{
	const auto& reflectionDescriptor =
		tool->GetReflectionGeneratorMap()
			.at(g_translationUnit2)
			->GetReflectionDescriptor();

	const auto& types = reflectionDescriptor.types;

	{
		auto baseIter = GetIteratorForTypeWithName(types, "Base");

		EXPECT_NE(baseIter, types.end());
	}
	{
		auto derivedIter = GetIteratorForTypeWithName(types, "Derived");

		EXPECT_NE(derivedIter, types.end());
	}
}

TEST_F(MultiTranslationUnit, BaseAndDerived_IsInMergedDescriptorOnce)
{
	const auto& reflectionDescriptor = tool->GetMergedReflectionDescriptor();

	const auto& types = reflectionDescriptor.types;

	{
		auto iter = GetIteratorForTypeWithName(types, "Base");
		EXPECT_NE(iter, types.end());

		++iter;
		auto secondIter = GetIteratorForTypeWithName(types, "Base", &iter);
		EXPECT_EQ(secondIter, types.end());
	}
	{
		auto iter = GetIteratorForTypeWithName(types, "Derived");
		EXPECT_NE(iter, types.end());

		++iter;
		auto secondIter = GetIteratorForTypeWithName(types, "Derived", &iter);
		EXPECT_EQ(secondIter, types.end());
	}
}
