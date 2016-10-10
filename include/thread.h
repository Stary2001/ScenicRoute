#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct scenic_thread
{
	struct scenic_process *proc;
	int tid;
} scenic_thread;

#ifdef __cplusplus
}
#endif
