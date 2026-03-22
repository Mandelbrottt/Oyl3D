#include <string>

#define __REFLECT__ [[clang::annotate("reflect")]]

__REFLECT__
void
Free_ReturnVoid_NoParams();

__REFLECT__
void
Free_ReturnVoid_VoidParam(void);

__REFLECT__
int
Free_ReturnInt_VoidPtrParam(void* voidPtrParam);

__REFLECT__
std::string
Free_ReturnString_NoParams();

class __REFLECT__ MemberFunctions
{
public:
	void
	ReturnVoid_NoParams();

	int
	ReturnInt_StringParam_Const(std::string stdStringParam) const;

	static
	void
	Static_ReturnVoid_IntParam(int intParam);
};
