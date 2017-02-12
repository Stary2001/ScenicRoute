#include <3ds.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "proc.h"
#include "dma.h"
#include "debug.h"

scenic_process *current_process;

scenic_process* proc_open(u32 pid, int flags)
{
	scenic_process *p = malloc(sizeof(scenic_process));
	memset(p, 0, sizeof(scenic_process));

	if(pid == (u32)-1)
	{
		svcGetProcessId(&pid, 0xffff8001);
	}

	p->pid = pid;

	Result r = svcOpenProcess(&p->handle, pid);
	if(r != 0)
	{
		free(p);
		return NULL;
	}

	if(flags & FLAG_DEBUG)
	{
		proc_debug(p);
	}

	return p;
}

void proc_close(scenic_process *p)
{
	if(p->frozen) debug_resume(p);
	if(p->handle) svcCloseHandle(p->handle);
	if(p->debug) svcCloseHandle(p->debug);
	if(p->threads) free(p->threads);
	free(p);
}

int proc_debug(scenic_process *p)
{
	if(!p) return -1;

	Result r = 0;
	if((r = svcDebugActiveProcess(&p->debug, p->pid)) != 0)
	{
		printf("svcDebugActiveProcess failed with %08lx for %lu %08lx\n", (u32)r, p->pid, p->debug);
		return -1;
	}

	debug_sink_events(p);

	return 0;
}

u32 branch(u32 base, u32 target)
{
	s32 off = (s32)(target - base);
	off -= 8; // arm is 2 instructions ahead (8 bytes)
	off /= 4; // word offset vs byte offset

	u32 ins = 0xea000000; // branch without link
	ins |= *(u32*)&off;
	return ins;
}

// storage = where to place in the remote proc. scan automatically??
// loc = branch insertion location
// hook_code/len = code to branch to.

bool proc_hook(scenic_process *p, u32 loc, u32 storage, u32 *hook_code, u32 hook_len)
{
	if (dma_protect(p, (void*)(storage & (~0xfff)), 0x1000) != 0)
	{
		return false;
	}

	if (dma_copy_from_self(p, (void*)storage, hook_code, hook_len) != 0)
	{
		return false;
	}

	u32 br = branch(loc, storage);

	if (dma_protect(p, (void*)(loc & (~0xfff)), 0x1000) != 0)
	{
		return false;
	}

	if (dma_copy_from_self(p, (void*)loc, &br, 4) != 0)
	{
		return false;
	}

	return true;
}

int proc_get_all_threads(scenic_process *p)
{
	u32 tids[MAX_THREAD];
	s32 n_tids = 32;

	Result r = svcGetThreadList(&n_tids, tids, n_tids, p->handle);
	if(r < 0)
	{
		printf("svcGetThreadList failed with %08lx!\n", r);
		return -1;
	}

	printf("got %li threads\n", n_tids);
	p->threads = malloc(n_tids * sizeof(scenic_thread));
	p->num_threads = n_tids;

	memset(p->threads, 0, n_tids * sizeof(scenic_thread));
	for(int i = 0; i < n_tids; i++)
	{
		p->threads[i].tid = tids[i];
		p->threads[i].proc = p;
	}

	return n_tids;
}

scenic_thread *proc_get_thread(scenic_process *p, int tid)
{
	if(!p) return NULL;
	if(!p->debug) proc_debug(p);
	if(!p->threads) proc_get_all_threads(p);

	for(int i = 0; i < p->num_threads; i++)
	{
		if(p->threads[i].tid == tid)
		{
			return &p->threads[i];
		}
	}

	return NULL;
}