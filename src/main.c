#include <3ds.h>
#include <stdio.h>
#include "proc.h"
#include "debug.h"

int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	printf("enabling debug\n");
	//debug_enable();
	printf("enabled debug\n");
	scenic_process *p = proc_open(0x10, FLAG_DEBUG);
	printf("proc %08x has handle %08x and debug %08x\n", p, p->handle, p->debug);

	debug_freeze(p);
	printf("HID frozen! try it out!\n");

	while(aptMainLoop())
	{
		hidScanInput();
		u32 k = hidKeysDown();
		if(k & KEY_START) break;
	}

	proc_close(p);
	gfxExit();

	return 0;
}