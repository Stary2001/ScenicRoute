typedef struct scenic_kthread
{
	struct scenic_kproc *process;
	void *ptr;
	void *ctx_ptr;
	int index;
} scenic_kthread;

#define MAX_THREADS 8
#define MAX_PROCS 0x30

typedef struct scenic_kproc
{
	u32 pid;
	void *ptr;
	void *codeset_ptr;

	scenic_kthread *main_thread;

	scenic_kthread *thread_table[MAX_THREADS];
	int used_index;
} scenic_kproc;

struct scenic_kproc_svc
{
	char flags[0x10];
};

typedef int (*find_cb_t)(u32, void*);

scenic_kproc *kproc_find_by_id(u32 pid);
scenic_kproc *kproc_find_by_name(char *name);
scenic_kproc *kproc_find_by_tid(u64 tid);

scenic_kproc *kproc_find(find_cb_t callback, void *dat);
void kproc_close(scenic_kproc *p);

scenic_kthread *kproc_get_main_thread(scenic_kproc *p);
scenic_kthread *kproc_get_list_head(scenic_kproc *p);

int kproc_get_flags(scenic_kproc *p, u32 *out);
int kproc_get_svc_mask(scenic_kproc *p, char *buf);
int kproc_set_flags(scenic_kproc *p, u32 new_flags);
int kproc_set_svc_mask(scenic_kproc *p, char *buf);

int kthread_get_ctx(scenic_kthread *t);
int kthread_get_svc_access(scenic_kthread *t, char *buf);

scenic_kthread *kthread_next(scenic_kthread *t);
scenic_kthread *kthread_prev(scenic_kthread *t);