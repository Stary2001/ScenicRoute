#pragma once

void debug_enable();
int debug_freeze(scenic_process *p);
#define debug_thaw debug_resume
int debug_resume(scenic_process *p);

// Internal. Ish.

void debug_sink_events(scenic_process *p);