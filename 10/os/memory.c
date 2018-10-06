#include "defines.h"
#include "kozos.h"
#include "lib.h"
#include "memory.h"

// header of memory area
typedef struct _kzmem_block {
	struct _kz_mem_block *next;
	int size;
} kzmem_block;

// get memory pool
typedef struct _kzmem_pool {
	int size;
	int num;
	kzmem_block *free;
} kzmem_pool;

// define memory pool: 16bytes ,32bytes and 64bytes
static kzmem_pool pool[] = {
	{16, 8, NULL}, {32, 8, NULL}, {64, 4, NULL},
};

// number of kind of memory pool
#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

static int kzmem_init_pool(kzmem_pool *p)
{
	int i;
	kzmem_block *mp;
	kzmem_block **mpp;
	extern char freearea; // area defined link script
	static char *area = &freearea;

	mp = (kzmem_block *)area;

	// connect each area to all freed link lists
	mpp = &p->free;
	for (i = 0; i < p->num; i++) {
		*mpp = mp;
		memset(mp, 0, sizeof(*mp));
		mp->size = p->size;
		mpp = &(mp->next);
		mp = (kzmem_block *)((char *)mp + p->size);
		area += p->size;
	}
	return 0;
}

// init dynamic memory
int kzmem_init(void)
{
	int i;
	for (i = 0; i < MEMORY_AREA_NUM; i++) {
		kzmem_init_pool(&pool[i]);
	}
	return 0;
}

// get dynamic memory
void *kzmem_alloc(int size)
{
	int i;
	kzmem_block *mp;
	kzmem_pool *p;

	for (i = 0; i < MEMORY_AREA_NUM; i++) {
		p = &pool[i];

		if (size <= p->size - sizeof(kzmem_block)) {
			if (p->free == NULL) { // no freed area
				kz_sysdown();
				return NULL;
			}
			// get area in freed link lists
			mp = p->free;
			p->free = p->free->next;
			mp->next = NULL;

			return mp + 1;
		}
	}

	kz_sysdown();
	return NULL;
}

void kzmem_free(void *mem)
{
	int i;
	kzmem_block *mp;
	kzmem_pool *p;

	mp = ((kzmem_block *)mem - 1);

	for (i = 0; i < MEMORY_AREA_NUM; i++) {
		p = &pool[i];
		if (mp->size == p->size) {
			mp->next = p->free;
			p->free = mp;
			return;
		}
	}

	kz_sysdown();
}
