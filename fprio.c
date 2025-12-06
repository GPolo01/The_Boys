#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

struct fpnodo_t {
	void *item;
  	int   tipo;
  	int   prio;
  	struct fpnodo_t *prox;
};

struct fprio_t {
  	struct fpnodo_t *prim;
 	int num;
};

struct fprio_t *fprio_cria () {
	struct fprio_t *f;

	//verificando memória alocada
    	if (!(f = malloc(sizeof(struct fprio_t))))
	    	return NULL;
	f->prim = NULL;
	f->num = 0;

	return f;
}

struct fprio_t *fprio_destroi (struct fprio_t *f) {
	struct fpnodo_t *atual, *prox;

    	atual = f->prim;
    	while (atual != NULL) {
		//atualizo o proximo
		prox = atual->prox;
        	free(atual);
		//atualizo o atual
        	atual = prox;
        }
	prox = NULL;
    	free(f);
	return NULL;
}

int fprio_insere (struct fprio_t *f, void *item, int tipo, int prio) {
	if (f == NULL || item == NULL)
		return -1;

	struct fpnodo_t *novo, *aux;

	aux = f->prim;
	//verifico se tem ponteiro repetido
	while (aux != NULL) {
		if (aux->item == item) { 
			free(item);
			return -1;
		}
		aux = aux->prox;
	}

	if (!(novo = malloc(sizeof(struct fpnodo_t))))
	       return -1;
	novo->item = item;
	novo->tipo = tipo;
	novo->prio = prio;

	aux = f->prim;
	//inserção na primeira posição (lista vazia)
	if (f->num == 0 || f->prim->prio > prio) {
		novo->prox = f->prim;
		f->prim = novo;
	}
	else {
		//inserindo no meio ou no final
        	while (aux->prox != NULL && aux->prox->prio <= prio)
			aux = aux->prox;
		novo->prox = aux->prox;
		aux->prox = novo;
	}
	return f->num++;
}

void *fprio_retira (struct fprio_t *f, int *tipo, int *prio) {
	// verificando se item e tipo inválidos
	if (f == NULL || f->prim == NULL || tipo == NULL || prio == NULL) {
		tipo = NULL;
		prio = NULL;
		return NULL;
	}

	struct fpnodo_t *aux;
	void *item;

	aux = f->prim;
	f->prim = aux->prox;

	item = aux->item;
	*tipo = aux->tipo;
	*prio = aux->prio;
	
	free(aux);
	f->num--;

	return item;
}

int fprio_tamanho (struct fprio_t *f) {
	//uso de operador ternário
	return f ? f->num : -1;
}

void fprio_imprime (struct fprio_t *f) {
	struct fpnodo_t *atual;
	//verificação para lista vazia
	if (f == NULL || f->prim == NULL)
		return;

	atual = f->prim;
	printf("(%d %d)", atual->tipo, atual->prio);
	atual = atual->prox;

	//nessa impressão não tem como ficar espaço extra no final
	while(atual != NULL) {
		printf(" (%d %d)", atual->tipo, atual->prio);
		atual = atual->prox;
	}
}

