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

// PRIVATE STRUCTURES
typedef struct TCBqueue {
	TCB_t *first, *last;
} TCB_queue;

typedef struct TCBwaiting_queue {
	int tid;
	TCB_queue waiting_threads;
	struct TCBwaiting_queue *next;
} TCB_waiting_queue;

// PRIVATE GLOBAL VARIABLES
TCB_t main_thread = { 0, 2, 0, {0}, NULL, NULL };
ucontext_t end_context = {0};
TCB_t *running = NULL;
TCB_queue mqueues[3] = { { NULL, NULL }, { NULL, NULL }, { NULL, NULL } };
TCB_waiting_queue *waiting_threads = NULL;


// PRIVATE FUNCTIONS

static int add_to_queue(TCB_t *thread) {
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
	return 0;
}

static TCB_t *remove_from_queue() {
	TCB_t *thread = NULL;
	int i;
	for (i = 0; i < 3; ++i) {
		if (mqueues[i].first != NULL) {
			thread = mqueues[i].first;
			mqueues[i].first = thread->next;
			thread->next = NULL;
			mqueues[i].first->prev = NULL;
			return thread;
		}
	}
	return thread;
}

static int run_next() {
	TCB_t *thread = running;
	running = remove_from_queue();
	if (thread != NULL) {
		running->state = STATE_RUNNING;
		swapcontext(&(thread->context), &(running->context));
	}
	return -1;
}

static void on_thread_exit() {
	// check if blocked
	run_next();
}

// PUBLIC FUNCTIONS

int mcreate(int prio, void (*start)(void*), void *arg) {

	static int last_tid = 0;
	TCB_t* thread;

	if (prio < 0 || prio > 2)
		return -1; // ERROU

	if (running == NULL) {
		running = &main_thread;
		makecontext(&end_context, on_thread_exit, 0);
	}

	thread = malloc(sizeof(TCB_t));
	if (thread == NULL) {
		// could not allocate thread;
		return -1;
	}
	thread->tid   = ++last_tid;
	thread->state = STATE_CREATION;
	thread->prio  = prio;
	//getcontext(&(thread->context));
	makecontext(&(thread->context), (void (*)(void)) start, 1, arg);
	add_to_queue(thread);
	return last_tid;
}
int myield(void) {
	if (running == NULL) {
		running = &main_thread;
		//makecontext(&end_context, on_thread_exit, 0);
	}
	add_to_queue(running);
	run_next();
	return -1; // ERROU
}
int mwait(int tid) {
	running->state = STATE_BLOCKED;
	getcontext(&(running->context));
	return -1; // ERROU
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
