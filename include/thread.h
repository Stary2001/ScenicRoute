#pragma once

typedef struct scenic_thread
{
	struct scenic_process *proc;
	int tid;
} scenic_thread;

typedef struct scenic_thread_ctx
{
	u32 unk[50];
} scenic_thread_ctx;

int thread_get_ctx(scenic_thread *t, scenic_thread_ctx *ctx);