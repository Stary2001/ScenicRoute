#include <3ds.h>
#include "proc.h"

u32 dma_protect(scenic_process *p, void* addr, u32 size)
{
	return svcControlProcessMemory(p->handle, (u32)addr, (u32)addr, size, 6, 7);
}

u32 dma_copy(scenic_process *dst, void* dst_p, scenic_process *src, void* src_p, u32 size)
{
	static u32 done_state = 0;

	u32 ret, i, state;
	u32 dmaConfig[20] = { 0 };
	Handle hDma;

	ret = svcFlushProcessDataCache(src->handle, src_p, size);
	ret = svcFlushProcessDataCache(dst->handle, dst_p, size);
	ret = svcStartInterProcessDma(&hDma, dst->handle, dst_p, src->handle, src_p, size, dmaConfig);
	state = 0;

	if (done_state == 0)
	{
		ret = svcGetDmaState(&state, hDma);
		svcSleepThread(1000000000);
		ret = svcGetDmaState(&state, hDma);
		done_state = state;
	}

	for (i = 0; i < 10000; i++)
	{
		state = 0;
		ret = svcGetDmaState(&state, hDma);
		if (state == done_state)
		{
			break;
		}
		svcSleepThread(1000000);
	}

	if (i >= 10000)
	{
		return 1; // error
	}

	svcCloseHandle(hDma);
	ret = svcInvalidateProcessDataCache(dst->handle, dst_p, size);
	if (ret != 0)
	{
		return ret;
	}

	return 0;
}

s32 kill_cache_k()
{
	__asm__ volatile("cpsid aif"); // Interrupts OFF
	__asm__ volatile("mcr p15, 0, r0, c7, c5, 0"); // icache
	__asm__ volatile("mcr p15, 0, r0, c7, c14, 0"); // dcache
	return 0;
}

void dma_kill_cache()
{
	svcBackdoor(kill_cache_k);
}
