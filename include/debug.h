#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum debug_event_type
{
	BREAKPOINT,
	THREAD_NEW,
	PROCESS_GONE,
	THREAD_GONE,
} debug_event_type;

typedef struct debug_thread_new_event
{
} debug_thread_new_event;

typedef struct debug_thread_gone_event
{
} debug_thread_gone_event;

typedef struct debug_process_gone_event
{
} debug_process_gone_event;

typedef struct debug_breakpoint_event
{
	u32 addr;
} debug_breakpoint_event;

typedef struct debug_event
{
	debug_event_type t;
	union
	{
		debug_breakpoint_event bkpt;
		debug_thread_new_event thread_new;
		debug_thread_gone_event thread_gone;
		debug_process_gone_event process_gone;
	};
} debug_event;

typedef void (*debug_event_cb)(debug_event*);

typedef struct debug_bkpt
{
	u32 addr;
	u32 orig_instr;
} debug_bkpt;

void debug_enable();
int debug_freeze(scenic_process *p);
#define debug_thaw debug_resume
int debug_resume(scenic_process *p);

int debug_get_thread_ctx(scenic_thread *t, ThreadContext *ctx);
int debug_set_thread_ctx(scenic_thread *t, ThreadContext *ctx);

// Debug events.
int debug_handle_events(scenic_process *p);
int debug_register_event_cb(scenic_process *p, debug_event_type ev, debug_event_cb cb);

// Software breakpoints.
int debug_add_breakpoint(scenic_process *p, u32 addr);
int debug_remove_breakpoint(scenic_process *p, u32 addr);

// Internal. Ish.

void debug_sink_events(scenic_process *p);

#ifdef __cplusplus
}
#endif
