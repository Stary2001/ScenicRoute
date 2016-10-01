
Result svcGetDebugThreadContext(scenic_debug_thread_ctx* context, Handle debug, u32 threadId, u32 controlFlags);
Result svcSetDebugThreadContext(scenic_debug_thread_ctx* context, Handle debug, u32 threadId, u32 controlFlags);
Result svcGetThreadList(s32* threadCount, u32* threadIds, s32 threadIdMaxCount, Handle domain);
