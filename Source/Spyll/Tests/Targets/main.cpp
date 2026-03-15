#include <stdio.h>

int main() {
	printf("Hello world!\n");
	#if __TEST__
	printf("Inside Options Parser!\n");
	#endif
	return 0;
}