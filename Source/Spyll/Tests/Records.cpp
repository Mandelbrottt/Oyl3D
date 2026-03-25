#include "Records.h"

const char* RecordsTest::s_compileTarget = "Records.cpp";

TEST_F(RecordsTest, No_Namespace_Reflect_Types_With_Annotation)
{
	const Spyll::TypeDescriptor* root_namespace_reflected_class =
		TryGetTypeWithName("RootNamespace_Reflected_Class");

	EXPECT_NE(root_namespace_reflected_class, nullptr);

	const Spyll::TypeDescriptor* root_namespace_reflected_struct =
		TryGetTypeWithName("RootNamespace_Reflected_Struct");

	EXPECT_NE(root_namespace_reflected_struct, nullptr);

	const Spyll::TypeDescriptor* root_namespace_non_reflected_class =
		TryGetTypeWithName("RootNamespace_NonReflected_Class");

	EXPECT_EQ(root_namespace_non_reflected_class, nullptr);
}

TEST_F(RecordsTest, Namespace_Reflect_Types_With_Annotation)
{
	const Spyll::TypeDescriptor* root_namespace_reflected_class =
		TryGetTypeWithName("Namespace::Reflected_Class");

	EXPECT_NE(root_namespace_reflected_class, nullptr);

	const Spyll::TypeDescriptor* root_namespace_reflected_struct =
		TryGetTypeWithName("Namespace::Reflected_Struct");

	EXPECT_NE(root_namespace_reflected_struct, nullptr);

	const Spyll::TypeDescriptor* root_namespace_non_reflected_class =
		TryGetTypeWithName("Namespace::NonReflected_Class");

	EXPECT_EQ(root_namespace_non_reflected_class, nullptr);
}
