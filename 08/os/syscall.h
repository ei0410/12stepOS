#ifndef _KOZOS_SYSCALL_H_INCLUDED_
#define _KOZOS_SYSCALL_H_INCLUDED_

#include "defines.h"

typedef enum {
	KZ_SYSCALL_TYPE_RUN = 0,
	KZ_SYSCALL_TYPE_EXIT,
} kz_syscall_type_t;

typedef struct {
	union {
		struct { // param for kz_run
			kz_func_t func; // main function
			char *name; // thread name
			int stacksize;
			int argc;
			char **argv;
			kz_thread_id_t ret; // return value of kz_run()
		} run;
		struct {
			int dummy;
		} exit;
	} un;
} kz_syscall_param_t;

#endif
