#include	"../include/mthread.h"
#include	<stdio.h>
#include	<stdlib.h>

#define NUMTHRDS 5

mmutex_t mtx;

void *func(void *arg) {
	int i;
	printf("thread %c running\n", (char) arg);
	for (i = 0; i < 5; i++) {
		printf("thread %c yeld on safe block\n", (char) arg);
		myield();
	}
	mlock(&mtx);
	for (i = 0; i < 5; i++) {
		printf("thread %c yeld on LOCKED block\n", (char) arg);
		myield();
	}
	munlock(&mtx);
	for (i = 0; i < 5; i++) {
		printf("thread %c yeld on safe block\n", (char) arg);
		myield();
	}
}

int main(int argc, char *argv) {
	int threads[NUMTHRDS];
	int i;
	mmutex_init(&mtx);
	for (i = 0; i < NUMTHRDS; ++i) {
		threads[i] = mcreate(i%3, func, (void *) '0' + i) ;
	}
	for (i = 0; i < NUMTHRDS; ++i) {
		mwait(i);
	}
}