#include <3ds.h>
#include <stdio.h>
#include "proc.h"
#include "debug.h"
#include "kernel/kproc.h"
#include "custom_svc.h"

void debug_enable()
{
	scenic_kproc *p = kproc_find_by_id((u32)-1);
	u32 flags;
	kproc_get_flags(p, &flags);
	flags |= 1 << 1; // Force debug.
	kproc_set_flags(p, flags);
	kproc_close(p);
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
		printf("svcBreakDebugProcess failed with %08lx\n", (u32)r);
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
			printf("svcGetProcessDebugEvent failed with %08lx.\n", (u32)r);
		}
		else
		{
			// todo: better handling of debug events..
			//printf("type %i\n", info.type);
		}
		svcContinueDebugEvent(p->debug, 3);
	}
}

int debug_get_thread_ctx(scenic_thread *t, scenic_debug_thread_ctx *ctx)
{
	if(!t || !ctx) { return -1; }
	Result r = svcGetDebugThreadContext(ctx, t->proc->debug, t->tid, 3); // todo: 3?
	if(r < 0)
	{
		return -1;
	}
	return 0;
}

int debug_set_thread_ctx(scenic_thread *t, scenic_debug_thread_ctx *ctx)
{
	if(!t || !ctx) { return -1; }
	Result r = svcSetDebugThreadContext(ctx, t->proc->debug, t->tid, 1); // todo: 3?
	if(r < 0)
	{
		return -1;
	}
	return 0;
}