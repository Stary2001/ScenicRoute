void kmem_copy(void *dst, void *src, u32 sz);
void kmem_dump(void *addr, u32 sz);
void kmem_dump_off(u32 addr, u32 sz, u32 off);
u32 kmem_search(void *addr, u32 sz, u32 magic);