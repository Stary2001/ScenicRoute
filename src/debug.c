#include <3ds.h>
#include <stdio.h>
#include "proc.h"
#include "debug.h"
#include "kmem.h"

void debug_enable()
{
	u32 kproc;
	u32 flags;
	kmem_copy(&kproc, (void*)0xFFFF9004, 4);

	u32 kflags_offset;
	bool is_n3ds;

	Result r;
	if((r = APT_CheckNew3DS(&is_n3ds)) != 0)
	{
		return;
	}

	if(is_n3ds)
	{
		kflags_offset = 0xb0;
	}
	else
	{
		kflags_offset = 0xa8;
	}

	kmem_copy(&flags, (void*)(kproc + kflags_offset), 4);
	flags |= 1 << 1; // Force debug.
	kmem_copy((void*)(kproc + kflags_offset), &flags, 4);
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
			// todo: better handling of debug events..
			//printf("type %i\n", info.type);
		}
		svcContinueDebugEvent(p->debug, 3);
	}
}