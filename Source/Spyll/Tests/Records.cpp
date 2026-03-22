#include "Records.h"

TEST_F(RecordsTest, RootNamespace_Reflect_Types_With_Annotation)
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
