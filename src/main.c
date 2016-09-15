#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include "proc.h"
#include "kernel/kproc.h"
#include "kernel/kmem.h"

int spin_pls = 1;
void spin_loop(void *p)
{
	while(spin_pls)
	{
		svcSleepThread(0); // yield
	}
}

int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	//debug_enable();

	scenic_kproc *p = kproc_find((u32)-1);
	if(p)
	{
		printf("current kproc: %08x %08x\n", p, p->ptr);
		scenic_kthread *t = kproc_get_main_thread(p);
		if(t)
		{
			printf("%08x %08x\n", t, t->ptr);
			kmem_dump(t->ptr, 0xb0);
		}
		while(!(hidKeysDown() & KEY_A)) hidScanInput();

		Thread spin = threadCreate(spin_loop, NULL, 0x100, 0x30, 0, 1);

		if(t)
		{
			printf("%08x %08x\n", t, t->ptr);
			kmem_dump(t->ptr, 0xb0);
		}

		spin_pls = 0;
	}

	/*printf("enabled debug\n");
	scenic_process *p = proc_open(0x10, FLAG_DEBUG);
	printf("proc %08x has handle %08x and debug %08x\n", p, p->handle, p->debug);

	debug_freeze(p);
	printf("HID frozen! try it out!\n");

	int counter = 3000000;*/

	while(aptMainLoop())
	{
		hidScanInput();
		u32 k = hidKeysDown();
		if(k & KEY_START) break;
		/*if(counter-- == 0)
		{
			debug_thaw(p);
			printf("HID thawed!\n");
		}*/
	}

	//proc_close(p);
	gfxExit();

	return 0;
}