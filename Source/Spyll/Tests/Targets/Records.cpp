#define __REFLECT__ [[clang::annotate("reflect")]]

class __REFLECT__ RootNamespace_Reflected_Class
{
	int private_field;

	void
	Private_Method() {}

public:
	int public_field;

	void
	Public_Method() {}
};

struct __REFLECT__ RootNamespace_Reflected_Struct
{
	int public_field;

	void
	Public_Method() {}

private:
	int private_field;

	void
	Private_Method();
};

class RootNamespace_NonReflected_Class {};

namespace Namespace
{
	class __REFLECT__ Reflected_Class
	{
		int private_field;

		void
		Private_Method() {}

	public:
		int public_field;

		void
		Public_Method() {}
	};

	struct __REFLECT__ Reflected_Struct
	{
		int public_field;

		void
		Public_Method() {}

	private:
		int private_field;

		void
		Private_Method();
	};

	class NonReflected_Class {};
}
