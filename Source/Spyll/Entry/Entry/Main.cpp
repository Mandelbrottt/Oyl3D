#include "Spyll/Tool.h"

int main(int argc, const char** argv)
{
	(void) argc;
	(void) argv;
	
	Spyll::Tool tool("main.cpp", "-x c++");
	return tool.Run();
}
