#include <stdlib.h>
#include <ucontext.h>
#include "../include/mthread.h"
#include "../include/mdata.h"

// PRIVATE CONSTANTS
#define PRIORITY_HIGH 0
#define PRIORITY_MEDIUM 1
#define PRIORITY_LOW 2
#define STATE_CREATION 0
#define STATE_READY 1
#define STATE_RUNNING 2
#define STATE_BLOCKED 3
#define STATE_END 4
#define STACK_SIZE 100

// PRIVATE STRUCTURES
typedef struct TCBqueue {
	TCB_t *first, *last;
} TCB_queue;

typedef struct TCBwaiting_queue {
	int tid;
	TCB_t *first;
	struct TCBwaiting_queue *next;
} TCB_waiting_queue;

// PRIVATE GLOBAL VARIABLES
static TCB_t main_thread = { 0, 2, 0, {0}, NULL, NULL };
static ucontext_t end_context = {0};
static TCB_t *running = NULL;
static TCB_queue mqueues[3] = { { NULL, NULL }, { NULL, NULL }, { NULL, NULL } };
static TCB_waiting_queue *waiting_queue = NULL;


// PRIVATE FUNCTIONS

static void add_to_queue(TCB_t *thread) {
	TCB_queue *queue;
	queue = &(mqueues[thread->prio]);
	thread->next = NULL;
	if (queue->last != NULL) {
		thread->prev = queue->last;
		queue->last->next = thread;
		queue->last = thread;
	} else {
		thread->prev = NULL;
		queue->last = thread;
		queue->first = thread;
	}
	thread->state = STATE_READY;
	return;
}

static TCB_t *remove_from_queue() {
	TCB_t *thread = NULL;
	int i;
	for (i = 0; i < 3; ++i) {
		if (mqueues[i].first != NULL) {
			thread = mqueues[i].first;
			mqueues[i].first = thread->next;
			thread->next = NULL;
			if (mqueues[i].first != NULL)
				mqueues[i].first->prev = NULL;
			else
				mqueues[i].last = NULL;
			return thread;
		}
	}
	return thread;
}

static int run_next() {
	TCB_t *thread = running;
	running = remove_from_queue();
	if (running != NULL) {
		if (thread->state != STATE_END) {
			running->state = STATE_RUNNING;
			return swapcontext(&(thread->context), &(running->context));
		} else {
			free(thread->context.uc_stack.ss_sp);
			free(thread);
			return setcontext(&(running->context));
		}
	}
	return -1;
}

static int create_waiting_list(int tid) {
	TCB_waiting_queue *new_waiting_queue = malloc(sizeof(TCB_waiting_queue));
	if (new_waiting_queue == NULL)
		return -1;
	new_waiting_queue->tid = tid;
	new_waiting_queue->first = NULL;
	new_waiting_queue->next = waiting_queue;
	waiting_queue = new_waiting_queue;
	return 0;
}

// return -1 when tid does not exist (already finished?)
static int add_to_waiting(int tid, TCB_t *thread) {
	TCB_waiting_queue *pos = waiting_queue;
	while (pos != NULL) {
		if (pos->tid == tid) {
			thread->next = pos->first;
			pos->first = thread;
			thread->state = STATE_BLOCKED;
			return 0;
		}
		pos = pos->next;
	}
	return -1;
}

static void on_thread_exit() {
	TCB_waiting_queue *pos = waiting_queue;
	TCB_waiting_queue *prev = NULL;
	TCB_t *waiting_thread;

	running->state = STATE_END;

	while (pos != NULL) {
		if (pos->tid == running->tid) {
			while (pos->first != NULL) {
				waiting_thread = pos->first;
				pos->first = waiting_thread->next;
				add_to_queue(waiting_thread);
			}
			if (pos == waiting_queue) {
				waiting_queue = pos->next;
			} else {
				prev->next = pos->next;
			}
			free(pos);
			break;
		}
		prev = pos;
		pos = pos->next;
	}
	run_next();
}

int init_() {
	static char stack[SIGSTKSZ];
	getcontext(&main_thread.context);
	running = &main_thread;
	if (getcontext(&end_context) != 0) {
		return -1;
	};
	end_context.uc_stack.ss_sp = stack;
	end_context.uc_stack.ss_size = sizeof(stack);
	makecontext(&end_context, on_thread_exit, 0);
	return 0;
}

// PUBLIC FUNCTIONS

int mcreate(int prio, void (*start)(void*), void *arg) {

	static int last_tid = 0;

	char *stack;
	TCB_t* thread;

	if (prio < 0 || prio > 2)
		return -1; // ERROU

	if (running == NULL) {
		if (init_() == -1) return -1;
	}

	thread = malloc(sizeof(TCB_t));
	if (thread == NULL) {
		// could not allocate thread;
		return -1;
	}

	stack = malloc(sizeof(char) * SIGSTKSZ);
	if (stack == NULL) {
		// could not allocate stack;
		free(thread);
		return -1;
	}

	thread->tid   = ++last_tid;
	thread->state = STATE_CREATION;
	thread->prio  = prio;

	if (create_waiting_list(last_tid) != 0) {
		free(thread);
		free(stack);
		--last_tid;
		return -1;
	}
	getcontext(&(thread->context));
	thread->context.uc_link = &end_context;
	thread->context.uc_stack.ss_sp = stack;
	thread->context.uc_stack.ss_size = sizeof(char) * SIGSTKSZ;
	makecontext(&(thread->context), (void (*)(void)) start, 1, arg);
	add_to_queue(thread);
	return last_tid;
}
int myield(void) {
	if (running == NULL) {
		if (init_() == -1) return -1;
	}
	add_to_queue(running);
	return run_next();
}
int mwait(int tid) {
	if (running == NULL) {
		if (init_() == -1) return -1;
	}
	if (add_to_waiting(tid,running) != 0) {
		add_to_queue(running);
	}
	return run_next();
}
int mmutex_init(mmutex_t *mtx) {
	return -1; // ERROU
}
int mlock (mmutex_t *mtx) {
	return -1; // ERROU
}
int munlock (mmutex_t *mtx) {
	return -1; // ERROU
}
