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
#define STATE_EXECUTION 2
#define STATE_BLOCKED 3

// PRIVATE STRUCTURES
typedef struct TCPqueue {
	TCB_t *first, *last;
} TCB_queue;


// PRIVATE GLOBAL VARIABLES
TCB_t main_thread = (struct TCB) { 0, 2, -1, {0}, NULL, NULL };
TCB_t *execution = &main_thread;
TCB_queue mqueues[4] = { { NULL, NULL }, { NULL, NULL }, { NULL, NULL }, { NULL, NULL } };


// PRIVATE FUNCTIONS

int add_to_queue(TCB_t *thread) {
	TCB_queue *queue;
	if (thread->prio < 0 || thread->prio > 2) {
		return -1;
	}
	queue = &mqueues[thread->prio];
	thread->prev = NULL;
	if (queue->last != NULL) {
		thread->next = queue->last;
		queue->last->prev = thread;
		queue->last = thread;
	} else {
		thread->next = NULL;
		queue->last = thread;
		queue->first = thread;
	}
	thread->state = 1;
	return 0;
}


// PUBLIC FUNCTIONS

int mcreate (int prio, void (*start)(void*), void *arg) {
	static unsigned int last_tid = 0;
	TCB_t* thread;

	if (prio < 0 || prio > 2)
		return -1; // ERROU

	thread = malloc(sizeof(TCB_t));
	if (thread == NULL) {
		// could not allocate thread;
		return -1;
	}
	thread->tid   = ++last_tid;
	thread->state = STATE_CREATION;
	thread->prio  = prio;
	//getcontext(&(thread->context));
	makecontext(&(thread->context), start, 1, arg);
	return add_to_queue(thread);
}
int myield(void) {
	add_to_queue(execution);
	//TODO: EXECUTE NEXT
	return -1; // ERROU
}
int mwait(int tid) {
	execution->state = STATE_BLOCKED;
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
