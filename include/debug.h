#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

void debug_enable();
int debug_freeze(scenic_process *p);
#define debug_thaw debug_resume
int debug_resume(scenic_process *p);

typedef struct scenic_thread_ctx
{
	u32 regs[16];
	u32 unk[34];
} scenic_debug_thread_ctx;

int debug_get_thread_ctx(scenic_thread *t, scenic_debug_thread_ctx *ctx);

// Internal. Ish.

void debug_sink_events(scenic_process *p);

#ifdef __cplusplus
}
#endif
