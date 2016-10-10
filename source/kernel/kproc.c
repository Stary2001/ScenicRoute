#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kernel/kproc.h"
#include "kernel/kmem.h"


u32 kproc_magic = 0xfff2e888;
u32 kthread_magic = 0;

u32 kproc_svc_offset = 0x90;
u32 kproc_flags_offset = 0xb0;
u32 kproc_codeset_offset = 0xb8;
u32 kproc_pid_offset = 0xbc;
u32 kproc_main_thread_offset = 0xc8;

u32 kthread_ctx_offset = 0x8c;
u32 kthread_prev_offset = 0xa0;
u32 kthread_next_offset = 0xa4;
u32 kthread_stolen_list_head_offset = 0xa8;

u32 kcodeset_name_offset = 0x50;
u32 kcodeset_tid_offset = 0x5c;

scenic_kproc *kproc_cache[MAX_PROCS] = {0};

int kProcInit(void) {
    u8 isN3ds;
	u32 ret;
	
	ret = aptInit();
	if(ret != 0) return 1;
    ret = APT_CheckNew3DS_System(&isN3ds);
	if(ret != 0) return 1;
	aptExit();
	
	if(!isN3ds) {
	    kproc_magic = 0xfff2d888;
		kproc_svc_offset = 0x88;
        kproc_flags_offset = 0xa8;
        kproc_codeset_offset = 0xb0;
        kproc_pid_offset = 0xb4;
        kproc_main_thread_offset = 0xc0;
	}
    return 0;
}

int id_callback(u32 candidate, void *dat)
{
	u32 pid = *(u32*)dat;
	u32 pid2;
	kmem_copy(&pid2, (void*)(candidate + kproc_pid_offset), 4);
	if(pid == pid2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

scenic_kproc *kproc_find_by_id(u32 pid)
{
	scenic_kproc *p;

	if(pid == (u32)-1)
	{
		p = malloc(sizeof(scenic_kproc));
		svcGetProcessId(&p->pid, 0xffff8001);
		kmem_copy(&p->ptr, (void*)0xFFFF9004, 4);
		kmem_copy(&p->codeset_ptr, (void*)((u32)p->ptr + kproc_codeset_offset), 4);

		return p;
	}
	else if(kproc_cache[pid])
	{
		u32 test; // Make sure it hasn't gone away. unlikely, but still..
		kmem_copy(&test, (void*) (kproc_cache[pid]->pid), 4);
		if(test == kproc_magic)
		{
			return kproc_cache[pid];
		}
		else
		{
			free(kproc_cache[pid]);
			kproc_cache[pid] = NULL;
		}
	}

	p = kproc_find(id_callback, &pid);
	kproc_cache[pid] = p;
	return p;
}

int name_callback(u32 candidate, void *dat)
{
	char buf[8];
	const char *name = (const char *)dat;

	u32 codeset;
	kmem_copy(&codeset, (void*)(candidate + kproc_codeset_offset), 4);
	kmem_copy(buf, (void*)(codeset + kcodeset_name_offset), 8);

	if(strncmp(buf, name, 8) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

scenic_kproc *kproc_find_by_name(char *name)
{
	return kproc_find(name_callback, name);
}

int tid_callback(u32 candidate, void *dat)
{
	u64 tid = *(u64*)dat;
	u64 tid2;

	u32 codeset;
	kmem_copy(&codeset, (void*)(candidate + kproc_codeset_offset), 4);
	kmem_copy(&tid2, (void*)(codeset + kcodeset_tid_offset), 8);

	if(tid == tid2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

scenic_kproc *kproc_find_by_tid(u64 tid)
{
	return kproc_find(tid_callback, &tid);
}

scenic_kproc *kproc_find(find_cb_t callback, void *target)
{
	scenic_kproc *p = malloc(sizeof(scenic_kproc));
	memset(p, 0, sizeof(scenic_kproc));

	u32 base = 0xfff70000;
	u32 end = 0xfffb0000; // 1 after the last addr to search

	while(true)
	{
		u32 res = kmem_search((void*)base, end-base, kproc_magic);
		if(res == 0)
		{
			break;
		}
		else
		{
			if(callback(res, target))
			{
				kmem_copy(&p->pid, (void*) (res + kproc_pid_offset), 4);
				p->ptr = (void*) res;
				kmem_copy(&p->codeset_ptr, (void*)((u32)p->ptr + kproc_codeset_offset), 4);

				return p;
			}

			base = res + 4;
		}
	}

	free(p);
	return NULL;
}

scenic_kthread *kproc_get_main_thread(scenic_kproc *p)
{
	if(!p) { return NULL; }
	if(p->main_thread) { return p->main_thread; }

	scenic_kthread *t = malloc(sizeof(scenic_kthread));
	memset(t, 0, sizeof(scenic_kthread));
	t->process = p;
	p->main_thread = t;

	u32 o = (u32)p->ptr + kproc_main_thread_offset;
	kmem_copy(&p->main_thread->ptr, (void*)o, 4);
	return p->main_thread;
}

scenic_kthread *kthread_create(scenic_kproc *p, void *ptr)
{
	scenic_kthread *t = malloc(sizeof(scenic_kthread));
	memset(t, 0, sizeof(scenic_kthread));
	t->process = p;
	t->ptr = ptr;
	return t;
}

scenic_kthread *kthread_search_tab(scenic_kproc *p, void *kptr)
{
	for(int i = 0; i < p->used_index; i++)
	{
		if(p->thread_table[i]->ptr == kptr)
		{
			return p->thread_table[i];
		}
	}

	return NULL;
}

scenic_kthread *kthread_create_or_search(scenic_kproc *p, void *kaddr)
{
	scenic_kthread *t = kthread_search_tab(p, kaddr);
	if(!t)
	{
		t = p->thread_table[p->used_index++] = kthread_create(p, kaddr);
	}
	return t;
}

scenic_kthread *kthread_prev(scenic_kthread *t)
{
	scenic_kproc *p = t->process;

	u32 o = (u32)t->ptr + kthread_prev_offset;

	u32 out;
	kmem_copy(&out, (void*)o, 4);
	if(out == 0) return NULL;

	return kthread_create_or_search(p, (void*)out);
}

scenic_kthread *kthread_next(scenic_kthread *t)
{
	scenic_kproc *p = t->process;

	u32 o = (u32)t->ptr + kthread_next_offset;
	u32 out;
	kmem_copy(&out, (void*)o, 4);
	printf("thr %08lx next = %08lx\n", (u32)t->ptr, out);
	if(out == 0) return NULL;

	return kthread_create_or_search(p, (void*)out);
}

scenic_kthread *kproc_get_list_head(scenic_kproc *p)
{
	if(!p) { return NULL; }
	if(!p->main_thread) { kproc_get_main_thread(p); }

	scenic_kthread *t = p->main_thread;
	u32 o = (u32)t->ptr + kthread_stolen_list_head_offset;
	u32 out;

	kmem_copy(&out, (void*)o, 4);
	printf("%08lx list head = %08lx\n", (u32)p, (u32)out);
	if(out == 0) return t; // good enough

	scenic_kthread *tt = kthread_search_tab(p, (void*)out);
	if(!tt)
	{
		tt = p->thread_table[p->used_index++] = kthread_create(p, (void*)out);
	}

	return tt;
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

void kproc_close(scenic_kproc *p)
{
	for(int i = 0; i < MAX_THREADS; i++)
	{
		if(p->thread_table[i] != NULL)
		{
			free(p->thread_table[i]);
		}
	}

	if(p->main_thread) free(p->main_thread);

	if(kproc_cache[p->pid] != NULL)
	{
		kproc_cache[p->pid] = NULL;
	}
	free(p);
}
