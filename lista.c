#include <stdlib.h>
#include <stdio.h>

struct item_t {
	int valor;
    	struct item_t *ant;
    	struct item_t *prox;
};

struct lista_t {
	struct item_t *prim;
    	struct item_t *ult;
    	int tamanho;
};

struct lista_t *lista_cria() {
    	struct lista_t *lst;

	//verificando memória alocada
    	if (!(lst = malloc(sizeof(struct lista_t))))
	    	return NULL;

    	lst->prim = NULL;
    	lst->ult = NULL;
    	lst->tamanho = 0;
    	return lst;
}

struct lista_t *lista_destroi (struct lista_t *lst) {
	struct item_t *atual, *prox;

    	atual = lst->prim;
    	while (atual != NULL) {
		//atualizo o proximo
		prox = atual->prox;
        	free(atual);
		//atualizo o atual
        	atual = prox;
        }
    	free(lst);
	return NULL;
}

int lista_insere (struct lista_t *lst, int item, int pos) {
	if (lst == NULL)
		return -1;

	struct item_t *atual, *aux;
	int i;
	if (!(atual = malloc(sizeof(struct item_t))))
	       return -1;
	atual->valor = item;

	//inserção na primeira posição
	if (pos == 0) {
		//Depois analisamos se a lista está vazia ou não
		if (lst->tamanho == 0){
			lst->ult = atual;
			atual->prox = NULL;
		}
		else {
			lst->prim->ant = atual;
			atual->prox = lst->prim;
		}
		lst->prim = atual;
		atual->ant = NULL;
		return lst->tamanho++;
	} 
	//inserindo no final se for negativo ou última posição
	else if (pos == lst->tamanho - 1 || pos == -1) {
		atual->prox = NULL;
		atual->ant = lst->ult;
		if (lst->ult != NULL)
			lst->ult->prox = atual;
		lst->ult = atual;
		if (lst->tamanho == 0)
			lst->prim = atual;
	}
	//inserindo no meio
        else if (pos < lst->tamanho && pos > 0) {
		aux = lst->prim;
		for (i = 0; i < pos; ++i) {
			aux = aux->prox;
		}
		atual->prox = aux;
		atual->ant = aux->ant;
		aux->ant->prox = atual;
		aux->ant = atual;
	}
	aux = NULL;
	return lst->tamanho++;
}

int lista_retira (struct lista_t *lst, int *item, int pos) {
	if (lst == NULL)
		return -1;	
	if (pos >= lst->tamanho || lst->tamanho == 0)
		return -1;

	struct item_t * aux;

	//retirar no inicio
	aux = lst->prim;
	if (pos == 0) {
		//depois verifico se a lista ficará vazia ou não
		*item = aux->valor;
		lst->prim = aux->prox;
		//nesse caso fica vazia e o ponteiro de ultimo vai pra NULL
		if (lst->tamanho == 1)
			lst->ult = NULL;
		else
			lst->prim->ant = NULL;
		free(aux);
		return lst->tamanho--;
	}
	//retirar no fim
	if (pos == lst->tamanho - 1 || pos == -1){
		aux = lst->ult;
		*item = aux->valor;
		lst->ult = aux->ant;
		lst->ult->prox = NULL;
	}
	//retirar no meio
	else { 
		while (pos > 0) {
			aux = aux->prox;
			pos--;
		}
		*item = aux->valor;
		aux->ant->prox = aux->prox;
		aux->prox->ant = aux->ant;
	}
	free(aux);
	return lst->tamanho--;
}

int lista_consulta (struct lista_t *lst, int *item, int pos) {
	if (lst == NULL)
		return -1;
	if (lst->tamanho <= pos)
		return -1;

	struct item_t *aux;

	aux = lst->prim;
	//consulta no final se última posição ou negativo
	if (pos == -1 || pos == lst->tamanho -1) {
		*item = lst->ult->valor;
		aux = NULL;
		return lst->tamanho;
	}
	//consulta no inicio
	if (pos == 0){
		*item = lst->prim->valor;
		aux = NULL;
		return lst->tamanho;
	}
	//consulta no meio
	while (pos > 0 && aux->prox != NULL) {
		aux = aux->prox;
		pos--;
	}
	*item = aux->valor;
	aux = NULL;
	return lst->tamanho;
}

int lista_procura (struct lista_t *lst, int valor) {
	int pos;
	struct item_t *atual;

	//se lista vazia retorna -1
	if (lst == NULL)
		return -1;

    	atual = lst->prim;
	pos = -1;
	//percorre a lista inteira até achar ou não o valor
    	while (atual != NULL) {
		pos++; 
		if (atual->valor == valor) {
			atual = NULL;
			return pos;
		}
		else
			atual = atual->prox;
        }
	return -1;
}

int lista_tamanho (struct lista_t *lst) {
    	//uso de operador ternário
	return lst ? lst->tamanho : -1;
}

void lista_imprime (struct lista_t *lst) {
	struct item_t *atual;

	//verificação para lista vazia
	if (!(atual = lst->prim))
		return;

	printf("%d", atual->valor);
	atual = atual->prox;

	//nessa impressão não tem como ficar espaço extra no final
	while(atual != NULL) {
		printf(" %d", atual->valor);
		atual = atual->prox;
	}
}

