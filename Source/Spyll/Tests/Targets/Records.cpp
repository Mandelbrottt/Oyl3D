#define __REFLECT__ [[clang::annotate("reflect")]]

class RootNamespaceClass;

class __REFLECT__ RootNamespaceClass
{
public:
	int a;
	bool b;

	void
	Foo() {}

	int
	Bar() { return 0; }
};

struct __REFLECT__ RootNamespaceStruct
{
	int a;
	bool b;

	void
	Foo() {}

	int
	Bar() { return 0; }
};
