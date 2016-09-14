#include <3ds.h>
#include <stdio.h>
#include "proc.h"
#include "debug.h"

s32 debug_enable_k()
{
	__asm__ volatile("cpsid aif"); // Interrupts OFF
    volatile u8* debug_mode = ((volatile u8*)0xFFF2D00A);
    *debug_mode = 1;
    return 0;
}

void debug_enable()
{
	svcBackdoor(debug_enable_k);
}

int debug_freeze(scenic_process *p)
{
	if(!p) { return -1; }
	if(p->debug == -1)
	{
		proc_debug(p);
	}

	Result r;
	if((r = svcBreakDebugProcess(p->debug)) != 0)
	{
		printf("svcBreakDebugProcess failed with %08x\n", r);
		return -1;
	}

	p->frozen = true;

	return 0;
}

int debug_resume(scenic_process *p)
{
	if(!p) { return -1; }
	if(p->debug == -1 || !p->frozen) { return 0; } // process is already running if we aren't debugging it...

	debug_sink_events(p);
	p->frozen = false;
	return 0;
}

void debug_sink_events(scenic_process *p)
{
	DebugEventInfo info;
	Result r;

	while(true)
	{
		if((r = svcGetProcessDebugEvent(&info, p->debug)) != 0)
		{
			if(r == 0xd8402009)
			{
				// Would block! we are done.
				break;
			}
			printf("svcGetProcessDebugEvent failed with %08x.\n", r);
		}
		else
		{
			printf("type %i\n", info.type);
		}
		svcContinueDebugEvent(p->debug, 3);
	}
}