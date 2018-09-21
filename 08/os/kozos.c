#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "lib.h"

#define THREAD_NUM 6 
#define THREAD_NAME_SIZE 15 // max name of thread

// thread context
typedef struct _kz_context {
	uint32 sp; // stack pointer
} kz_context;

// task cotrol block(TCB)
typedef struct _kz_thread {
	struct _kz_thread *next;
	char name[THREAD_NAME_SIZE + 1]; // thread name
	char *stack;

	struct { // param for thread init
		kz_func_t func; // main
		int argc;
		char **argv;
	} init;

	struct { // buffer for system call
		kz_syscall_type_t type;
		kz_syscall_param_t *param;
	} syscall;

	kz_context context;
} kz_thread;

// ready queue of thread
static struct {
	kz_thread *head;
	kz_thread *tail;
} readyque;

static kz_thread *current;
static kz_thread threads[THREAD_NUM]; // TCB
static kz_handler_t handlers[SOFTVEC_TYPE_NUM];

void dispatch(kz_context *context);

static int getcurrent(void) // pull current thread from ready queue
{
	if (current == NULL) {
		return -1;
	}

	// current thread is on head, so pull head
	readyque.head = current->next;
	if (readyque.head == NULL) {
		readyque.tail = NULL;
	}
	current->next = NULL; // clear next pointer, because pull entry from link list

	return 0;
}

// connect readyque and current thread
static int putcurrent(void)
{
	if (current == NULL) {
		return -1;
	}

	// connect tail of ready queue
	if (readyque.tail) {
		readyque.tail->next = current;
	} else {
		readyque.head = current;
	}
	readyque.tail = current;

	return 0;
}

static void thread_end(void)
{
	kz_exit();
}

static void thread_init(kz_thread *thp)
{
	// call main of thread
	thp->init.func(thp->init.argc, thp->init.argv);
	thread_end();
}

// run thread
static kz_thread_id_t thread_run(kz_func_t func, char *name, int stacksize, int argc, char *argv[])
{
	int i;
	kz_thread *thp;
	uint32 *sp;
	extern char userstack; // stack area defined by link script
	static char *thread_stack = &userstack;

	// search for void TCB
	for (i = 0; i < THREAD_NUM; i++) {
		thp = &threads[i]
		if (!thp->init.func) // find out
			break;
	}
	if (i == THREAD_NUM) // not find
		return -1;

	memset(thp, 0, sizeof(*thp)); // zero clear TCB

	// set TCB param
	strcpy(thp->name, name);
	thp->next = NULL;

	thp->init.func = func;
	thp->init.argc = argc;
	thp->init.argv = argv;

	// get stack area
	memset(thread_stack, 0, stacksize);
	thread_stack += stacksize;

	thp->stack = thread_stack; //set stack

	//init stack
	sp = (uint32 *)thp->stack;
	*(--sp) = (uint32)thread_end;

	// set PC
	*(--sp) = (uint32)thread_init;

	*(--sp) = 0; /* ER6 */
	*(--sp) = 0; /* ER5 */
	*(--sp) = 0; /* ER4 */
	*(--sp) = 0; /* ER3 */
	*(--sp) = 0; /* ER2 */
	*(--sp) = 0; /* ER1 */

	// start up thread
	*(--sp) = (uint32)thp; /* ER0 */

	thp->context.sp = (uint32)sp; // save stack pointer as context

	putcurrent();

	current = thp;

	putcurrent();

	return (kz_thread_id_t)current;
}

static int thread_exit(void)
{
	puts(current->name);
	puts(" EXIT.\n");
	memset(current, 0, sizeof(*current)); // clear TCB
	return 0;
}
