/*
 * mdata.h: arquivo de inclusão de uso apenas na geração da libmthread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida.
 *
 */
#ifndef __mdata__
#define __mdata__

/* NÃO ALTERAR ESSA struct */
typedef struct TCB { 
	int  tid;		// identificador da thread 
	int  state;	// estado em que a thread se encontra 
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término 
	int  prio;		// prioridade da thread (0:alta; 1: média, 2:baixa) 
	ucontext_t   context;	// contexto de execução da thread (SP, PC, GPRs e recursos) 
	struct TCB   *prev;		// ponteiro para o TCB anterior da lista 
	struct TCB   *next;		// ponteiro para o próximo TCB da lista 
} TCB_t; 

#endif
