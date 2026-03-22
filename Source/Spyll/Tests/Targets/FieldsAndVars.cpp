#define __REFLECT__ [[clang::annotate("reflect")]]

struct TestRecord
{
	int a;
	bool b;
};

class __REFLECT__ TestClass
{
public:
	int public_lvalue_int;

protected: 
	int protected_lvalue_int;

private:   
	int private_lvalue_int;

public:
	int&       public_lvalue_reference_int;
	int*       public_lvalue_pointer_int;
	static int public_static_lvalue_reference_int;

protected:
	int&       protected_lvalue_reference_int;
	int*       protected_lvalue_pointer_int;
	static int protected_static_lvalue_reference_int;

private:
	int&       private_lvalue_reference_int;
	int*       private_lvalue_pointer_int;
	static int private_static_lvalue_reference_int;

public:
	TestRecord public_lvalue_testRecord;
};

int TestClass::public_static_lvalue_reference_int;
int TestClass::protected_static_lvalue_reference_int;
int TestClass::private_static_lvalue_reference_int;

__REFLECT__ int  global_lvalue_int;
__REFLECT__ int& global_lvalue_reference_int = global_lvalue_int;
__REFLECT__ int* global_lvalue_pointer_int;

__REFLECT__ TestRecord global_lvalue_testClass;

int  global_no_reflect_lvalue_int;
int& global_no_reflect_lvalue_reference_int = global_no_reflect_lvalue_int;
int* global_no_reflect_lvalue_pointer_int;