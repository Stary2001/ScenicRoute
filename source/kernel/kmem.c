#include <3ds.h>
#include <stdio.h>
#include "kernel/kmem.h"

u8 *kcpy_src = NULL;
u8 *kcpy_dst = NULL;
u32 kcpy_sz = 0;

u32 *ksearch_src = NULL;
u32 ksearch_sz = 0;
u32 ksearch_magic = 0;
u32 ksearch_addr = 0;

s32 kmem_copy_k()
{
	__asm__ volatile("cpsid aif"); // Interrupts OFF

	while(kcpy_sz != 0)
	{
		*kcpy_dst++ = *kcpy_src++;
		kcpy_sz--;
	}
	return 0;
}

void kmem_copy(void *dst, void *src, u32 sz)
{
	kcpy_src = src;
	kcpy_dst = dst;
	kcpy_sz = sz;

	svcBackdoor(kmem_copy_k);
}

s32 kmem_search_k()
{
	__asm__ volatile("cpsid aif"); // Interrupts OFF

	while(ksearch_sz != 0)
	{
		if(*ksearch_src++ == ksearch_magic)
		{
			ksearch_addr = (u32)ksearch_src - 4;
			return 0;
		}
		ksearch_sz--;
	}
	ksearch_addr = 0;
	return 0;
}

u32 kmem_search(void *addr, u32 sz, u32 magic)
{
	ksearch_src = addr;
	ksearch_sz = sz / 4;
	ksearch_magic = magic;

	svcBackdoor(kmem_search_k);

	return ksearch_addr;
}

void kmem_dump(void *addr, u32 sz)
{
	kmem_dump_off((u32)addr, sz, 0);
}

// internal pls?
void kmem_dump_off(u32 addr, u32 sz, u32 off)
{
	char dump[8];

	u32 orig_addr = addr;

	for(; addr < orig_addr + sz; addr+=8)
	{
		printf("+%04lx: ", addr - orig_addr + off);
		kmem_copy(&dump, (void*)addr, 8);
		for(int i = 0; i < 8; i++)
		{
			printf("%02x ", dump[i]);
		}
		printf("\n");
	}
}
