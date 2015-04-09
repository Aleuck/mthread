/*
 * mthread.h: arquivo de inclusão com os protótipos das funções a serem
 *            implementadas na realização do trabalho.
 *
 * NÃO MODIFIQUE ESTE ARQUIVO.
 *
 * VERSÃO 1 - 24/03/2015
 */
#ifndef __mthread__
#define __mthread__

typedef struct mutex {
	int	flag; 					// indica se o mutex foi adquirido ou não (0: livre; 1: ocupado)
	struct TCB	*first, *last; 	// ponteiros para lista de threads bloqueadas no mutex
} mmutex_t;

int mcreate (int prio, void (*start)(void*), void *arg);
int myield(void);
int mwait(int tid);
int mmutex_init(mmutex_t *mtx);
int mlock (mmutex_t *mtx);
int munlock (mmutex_t *mtx);

#endif
