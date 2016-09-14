#include <3ds.h>
#include <stdio.h>
#include "proc.h"
#include "debug.h"
#include "kmem.h"

int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	debug_enable();
	printf("enabled debug\n");
	scenic_process *p = proc_open(0x10, FLAG_DEBUG);
	printf("proc %08x has handle %08x and debug %08x\n", p, p->handle, p->debug);

	debug_freeze(p);
	printf("HID frozen! try it out!\n");

	int counter = 3000000;

	while(aptMainLoop())
	{
		hidScanInput();
		u32 k = hidKeysDown();
		if(k & KEY_START) break;
		if(counter-- == 0)
		{
			debug_thaw(p);
			printf("HID thawed!\n");
		}
	}

	proc_close(p);
	gfxExit();

	return 0;
}