
#include <stdio.h>

#include "NativeClass.h"


int main(int argc, char** argv)
{
	if (NativeClass().TrueFromCS())
		printf("Hello World\n");
	else
		printf("something went terribly wrong");

	return 0;
}
