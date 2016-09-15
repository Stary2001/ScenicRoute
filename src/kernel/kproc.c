#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include "kernel/kproc.h"
#include "kernel/kmem.h"

// Set to n3DS values. Init somewhere. :(

u32 kproc_magic = 0xfff2e888;
u32 kthread_magic = 0;

u32 kproc_svc_offset = 0x90;
u32 kproc_flags_offset = 0xb0;
u32 kproc_pid_offset = 0xbc;
u32 kproc_main_thread_offset = 0xc8;

u32 kthread_ctx_offset = 0;

scenic_kproc *kproc_find(u32 pid)
{
	if(pid == 0) { return NULL; }

	scenic_kproc *p = malloc(sizeof(scenic_kproc));
	if(pid == (u32)-1)
	{
		kmem_copy(&p->ptr, (void*)0xFFFF9004, 4);
		u32 magic;
		kmem_copy(&magic, p->ptr, 4);
		printf("ye. %08x\n", magic);

		return p;
	}
	else
	{
		printf("no kproc searching yet\n");
		free(p);
		return NULL;
	}

	free(p);
	return NULL;
}

scenic_kthread *kproc_get_main_thread(scenic_kproc *p)
{
	if(!p) { return NULL; }
	if(p->main_thread) { return p->main_thread; }

	scenic_kthread *t = malloc(sizeof(scenic_kthread));
	t->process = p;
	p->main_thread = t;

	u32 o = (u32)p->ptr + kproc_main_thread_offset;
	kmem_copy(&p->main_thread->ptr, (void*)o, 4);
	return p->main_thread;
}

int kproc_get_flags(scenic_kproc *p, u32 *out)
{
	if(!p || !p->ptr) { return -1; }
	u32 o = (u32)p->ptr + kproc_flags_offset;
	kmem_copy(out, (void*)o, 4);
	return 0;
}

int kproc_get_svc_access(scenic_kproc *p, char *buf)
{
	if(!p || !p->ptr) { return -1; }
	u32 o = (u32)p->ptr + kproc_svc_offset;
	kmem_copy(buf, (void*)o, 0x10);
	return 0;
}

int kthread_get_ctx(scenic_kthread *t)
{
	if(!t || !t->ptr) { return -1; }
	u32 o = (u32)t->ptr + kthread_ctx_offset;
	kmem_copy(t->ctx_ptr, (void*)o, 4);
	return 0;
}

int kthread_get_svc_access(scenic_kthread *t, char *buf)
{
	if(!t || !t->ptr) { return -1; }
	if(!t->ctx_ptr && kthread_get_ctx(t)) { return -1; }
	// todo: needs ctx
	return -1;
}

int kproc_set_flags(scenic_kproc *p, u32 new_flags)
{
	if(!p || !p->ptr) { return -1; }
	u32 o = (u32)p->ptr + kproc_flags_offset;
	kmem_copy((void*)o, &new_flags, 4);
	return 0;
}

int kproc_set_svc_access(scenic_kproc *p, char *buf)
{
	if(!p || !p->ptr) { return -1; }
	u32 o = (u32)p->ptr + kproc_svc_offset;
	kmem_copy((void*)o, buf, 0x10);
	return 0;
}

int kthread_set_svc_access(scenic_kthread *t, char *buf)
{
	if(!t || !t->ptr) { return -1; }
	if(!t->ctx_ptr && kthread_get_ctx(t)) { return -1; }
	// todo: needs ctx
	return -1;
}