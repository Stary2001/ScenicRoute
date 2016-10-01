#pragma once

typedef struct scenic_thread
{
	struct scenic_process *proc;
	int tid;
} scenic_thread;