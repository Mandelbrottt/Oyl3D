#include "Spyll/Tool.h"

int main(int argc, const char** argv)
{
	(void) argc;
	(void) argv;
	
	Spyll::Tool tool(argc, argv);
	return tool.Run();
}
