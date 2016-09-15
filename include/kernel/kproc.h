typedef struct scenic_kthread
{
	struct scenic_kproc *process;
	void *ptr;
	void *ctx_ptr;
} scenic_kthread;

typedef struct scenic_kproc
{
	u32 pid;
	void *ptr;

	scenic_kthread *main_thread;
} scenic_kproc;

struct scenic_kproc_svc
{
	char flags[0x10];
};

scenic_kproc *kproc_find(u32 pid);
scenic_kthread *kproc_get_main_thread(scenic_kproc *p);

int kproc_get_flags(scenic_kproc *p, u32 *out);
int kproc_get_svc_mask(scenic_kproc *p, char *buf);
int kproc_set_flags(scenic_kproc *p, u32 new_flags);
int kproc_set_svc_mask(scenic_kproc *p, char *buf);

int kthread_get_ctx(scenic_kthread *t);
int kthread_get_svc_access(scenic_kthread *t, char *buf);