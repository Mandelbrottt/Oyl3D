#include "Spyll/Tool.h"

int main(int argc, const char** argv)
{
	Spyll::CmdTool tool(argc, argv);
	return tool.Run();
}
