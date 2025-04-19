#include <stdlib.h> //exit()
#include <signal.h> //signal()
#include "EPD_1in54_V2_test.h"

UBYTE *BlackImage;

int main(void)
{
	signal(SIGINT, Handler);
	signal(SIGHUP, Handler);
	signal(SIGKILL, Handler);
	signal(SIGQUIT, Handler);
	signal(SIGTERM, Handler);

	EPD_1in54_V2_test();

	return 0;
}
