#pragma once
#include <stdbool.h>
#include "thread.h"

typedef struct scenic_process scenic_process;
#include "debug.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_BREAK 32

typedef struct scenic_process
{
	u32 pid;
	Handle handle;
	Handle debug;
	bool frozen;

	u32 num_threads;
	struct scenic_thread *threads;
	struct debug_bkpt breakpoints[MAX_BREAK];
} scenic_process;

#define FLAG_NONE 0
#define FLAG_DEBUG 1

#define MAX_THREAD 32

scenic_process* proc_open(u32 pid, int flags);
void proc_close(scenic_process *p);
int proc_debug(scenic_process *p);
bool proc_hook(scenic_process *p, u32 loc, u32 storage, u32 *hook_code, u32 hook_len);

// \/ uses debug
scenic_thread *proc_get_thread(scenic_process *p, int tid);
int proc_get_all_threads(scenic_process *p);

extern scenic_process *current_process;

#ifdef __cplusplus
}
#endif
