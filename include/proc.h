#pragma once
#include <stdbool.h>

typedef struct scenic_process
{
	u32 pid;
	Handle handle;
	Handle debug;
	bool frozen;
} scenic_process;

#define FLAG_NONE 0
#define FLAG_DEBUG 1

scenic_process* proc_open(u32 pid, int flags);
void proc_close(scenic_process *p);
int proc_debug(scenic_process *p);
bool proc_hook(scenic_process *p, u32 loc, u32 storage, u32 *hook_code, u32 hook_len);

extern scenic_process *current_process;