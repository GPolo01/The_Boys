//Programa principal do projeto "The Boys- 2024/2"
//Autor: Gabriel Henrique Polo, GRR 20241558

// #Includes e Defines Utilizados
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "math.h"
#include "fprio.h"
#include "lista.h"
#include "conjunto.h"

#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS (N_HABILIDADES * 5)
#define N_BASES (N_HEROIS / 5)
#define N_MISSOES (T_FIM_DO_MUNDO / 100)
// minimize o uso de variáveis globais
typedef struct fprio_t LEF;
LEF *LEF_global = NULL;

//||||||||||||||||||||||||||||Estruturas do mundo||||||||||||||||||||||||||||||

// Define os tipos de eventos
typedef enum {
	CHEGA,
	ESPERA,
	DESISTE,
	AVISA,
	ENTRA,
	SAI,
	VIAJA,
	MISSAO,
	MORRE,
	FIM
} TipoEvento;

struct heroi {
	int ID;
	int exp;
	int pac;
	int vel;
	struct cjto_t *hab;
	int IDbase;
	int vivo;
};

struct local {
	int X;
	int Y;
};

struct base {
	int IDB;
	struct local loc;
	int lot;
	struct cjto_t *pres;
	struct lista_t *esp;
	int tam; //tamanho para lista de esp;
	int missões;
	int FilaMax; //tamanho max da lista de esp;
	struct cjto_t *hab;
};

struct missao {
	int IDM;
	struct cjto_t *hab;
	int per;
	struct local locM;
	int tentativas;
};

struct mundo {
	int NHerois;
	int NBases;
	int NMissoes;
	int NHabilidades;
	struct local tamanhoMundo;
	int Relogio; //1 segundo do programa = 1 minuto de tempo real
	struct heroi *herois;
	struct base *bases;
	struct missao *missoes;
};

typedef struct Evento {
	TipoEvento tipo;
	// Tempo do evento
	int prio;	
	int heroi_id;
	int base_id;
	// -1 se não for missão
	int missao_id;
} Evento;

//||||||||||||||||||||||Funções Auxiliares:||||||||||||||||||||||||||||||||||| 

int aleat (int min, int max) {
	return min + (rand() % (max - min + 1));
}

void limpa_habilidades_base(struct base *base) {
	struct cjto_t *novo;
	
	novo = cjto_cria(base->hab->cap);
    cjto_destroi(base->hab);
    base->hab = novo;
}

int base_aptidao(struct base *base, struct heroi *herois,
        struct missao *missao) {
    struct heroi *heroi;
    int i, j, achados, presentes;

	i = achados = 0;
	presentes = cjto_card(base->pres);
    //enquanto não achei todos os herois da base
    while (achados < presentes) {
	//verifico se ele pertence
        if (cjto_pertence(base->pres, i)) {
            heroi = &herois[i];
            // Insere todas as habilidades do herói no conjunto da base
            for (j = 0; j < N_HABILIDADES; j++) {
                if (cjto_pertence(heroi->hab, j))
                    cjto_insere(base->hab, j);
            }
			achados++;
        }
		i++;
    }
    //verifico se o conjunto das habilidades é suficiente ou igual
    if (cjto_contem(base->hab, missao->hab) || cjto_iguais(base->hab,
		missao->hab))
            return 1;
    return 0;
}

struct base *base_mais_proxima(struct missao *missao, struct base *bases,
        struct heroi *herois, int NBases, double *menorDistancia, int T) {
    struct base *bmp, *base;
	struct heroi *heroi;
    double distancia;
	int i;
    //vetor para colocar as bases de acordo com proximidade

	//maior distancia do ponto (0,0) para o (20000,20000)
    *menorDistancia = sqrt(pow(0 - N_TAMANHO_MUNDO, 2) +
                                pow(0 - N_TAMANHO_MUNDO, 2));
    bmp = NULL;
    //vou verificar todas as bases, a que for apta e mais perto recebe a missao
    for (i = 0; i < NBases; i++) {
        base = &bases[i];
        distancia = sqrt(pow(base->loc.X - missao->locM.X, 2) +
                                pow(base->loc.Y - missao->locM.Y, 2));
		//bmp receberá o endereço da base se ela for a mais perta e apta
        if (base_aptidao(base, herois, missao) && 
				distancia < *menorDistancia) {
            *menorDistancia = distancia;
            bmp = base;
        }
    }
	//se houve base apta 
	if (bmp) {
        printf("%6d: MISSAO %d, BASE %d DIST %.0f HEROIS [ ", T, missao->IDM,
			bmp->IDB, distancia);
        cjto_imprime(bmp->pres);
        printf(" ]\n");
		for (i = 0; i < N_HEROIS; i++) {
			if (cjto_pertence(bmp->pres, i)) {
            	heroi = &herois[i];
            	printf("%6d: MISSAO %d HAB HEROI %d: [ ", T, missao->IDM,
					heroi->ID);
            	cjto_imprime(heroi->hab);
            	printf(" ]\n");
			}

		}
		printf("%6d: MISSAO %d UNIAO HAB BASE %d: [ ", T, missao->IDM,
			       		bmp->IDB);
    	cjto_imprime(bmp->hab);
    	printf(" ]\n");
    }

    return bmp;
}

//passo por todos os conjuntos e listas
void destroi_mundo(struct mundo *mundo) {
    for (int i = 0; i < mundo->NHerois; i++) 
        cjto_destroi(mundo->herois[i].hab);

    for (int i = 0; i < mundo->NBases; i++) {
        cjto_destroi(mundo->bases[i].pres);
		cjto_destroi(mundo->bases[i].hab);
        lista_destroi(mundo->bases[i].esp);
    }
    for (int i = 0; i < mundo->NMissoes; i++) 
        cjto_destroi(mundo->missoes[i].hab);

    //depois libero as estruturas
    free(mundo->herois);
    free(mundo->bases);
    free(mundo->missoes);
    free(mundo);
}

//|||||||||||||||||Funções que envolvem a LEF e os Eventos|||||||||||||||||||||

void inicializa_lef() {
    LEF_global = fprio_cria();
    if (!LEF_global) {
        fprintf(stderr, "Erro ao criar LEF.\n");
        exit(1);
    }
}

Evento *cria_evento(int T, TipoEvento tipo, int heroi_id, int base_id,
        int missao_id) {
	Evento *novo;
	if (!(novo = (Evento *)malloc(sizeof(Evento))))
		return NULL;
	novo->tipo = tipo;
    //Prioridade é tempo
	novo->prio = T; 
	novo->heroi_id = heroi_id;
	novo->base_id = base_id;
	novo->missao_id = missao_id;

	return novo;
}

void insere_evento(LEF *lef, int T, TipoEvento tipo, int heroi_id,
        int base_id, int missao_id) {
    Evento *novo;
    if (!(novo = cria_evento(T, tipo, heroi_id, base_id, missao_id))) {
        fprintf(stderr, "ERRO AO CRIAR EVENTO.\n");
        return;
    }
    if (fprio_insere(lef, novo, tipo, T) == -1) {
        fprintf(stderr, "ERRO PARA INSERIR NA LEF.\n");
        free(novo);  // Evita vazamento de memória
    }
}

Evento *retira_evento(LEF *lef) {
	int tipo, prio;
    Evento *evento;
    evento = (Evento *)fprio_retira(lef, &tipo, &prio);
    return evento;
}

void destroi_evento(Evento *evento) {
    if (evento != NULL)
        free(evento);
}

void destroi_lef(LEF *lef) {
    Evento *evento;
    while ((evento = retira_evento(lef)) != NULL)
        destroi_evento(evento); // Libera cada evento retirado
    fprio_destroi(lef); // Libera a estrutura da LEF
}

//|||||||||||||||||||||||||Funções para os eventos:|||||||||||||||||||||||||||||

void chega(int T, struct heroi *heroi, struct base *base) {
    int espera;

    // Atualiza a base atual do herói
    heroi->IDbase = base->IDB;
    // Imprime a chegada do herói na base
    printf("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) ", T, heroi->ID, base->IDB,
           cjto_card(base->pres), base->lot);
	
	espera = 0;
    // Decide se o herói irá esperar ou desistir
    if ((base->lot > cjto_card(base->pres) && base->tam == 0) ||
		(heroi->pac > (10 * base->tam))){
		espera = 1;
	}
    // Decide qual dos dois eventos ocorrera
    if (espera) {
        printf("ESPERA\n");
        insere_evento(LEF_global, T, ESPERA, heroi->ID, base->IDB, -1);
    } else {
        printf("DESISTE\n");
        insere_evento(LEF_global, T, DESISTE, heroi->ID, base->IDB, -1);
    }
}

int espera(int T, struct heroi *heroi, struct base *base) {
    // Adiciona o herói à fila no final (o tamanho é aumentado)
    if (lista_insere(base->esp, heroi->ID, -1) == -1) {
        fprintf(stderr, "ERRO PARA ADICIONAR A FILA DE ESPERA.\n");
        return -1;
    }
	base->tam++;
	//atualizo Fila Max se o tam atual for maior
	if (base->tam > base->FilaMax)
		base->FilaMax = base->tam;

    printf("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", T, heroi->ID, base->IDB,
        base->tam);
    insere_evento(LEF_global, T, AVISA, heroi->ID, base->IDB, -1);
    
    return 0;
}

int avisa(int T, struct base *base, struct heroi *herois) {
    struct heroi *heroi;
    int heroi_id;
    // Verifica se o ponteiro para a fila de espera não é nulo
    if (!base || !base->esp) {
        fprintf(stderr, "ERRO: PONTEIRO PARA FILA NULA.\n");
        return -1;
    }

    printf("%6d: AVISA PORTEIRO BASE %d (%2d/%2d) FILA [ ",
           T, base->IDB, cjto_card(base->pres), base->lot);
    lista_imprime(base->esp);
    printf(" ]\n");

	//Enquanto houver vagas na base e heróis na fila de espera
    while (base->lot > cjto_card(base->pres) && base->tam > 0) {
		// Retira o primeiro herói da fila de espera
        lista_retira(base->esp, &heroi_id, 0);
		base->tam--;
        // Valida o índice do herói retirado
        if (heroi_id < 0 || heroi_id >= N_HEROIS) {
            fprintf(stderr, "ERRO: ID de herói INVÁLIDA: %d\n"
            ,heroi_id);
            continue; // Ignora indices inválidos
        }
        heroi = &herois[heroi_id];
	// Adiciona o herói ao conjunto de presentes da base
	if (cjto_insere(base->pres, heroi->ID) == -1) {
		fprintf(stderr, "ERRO: AO TENTAR ADICIONAR HEROI.\n");
		return -1;
	}

        // Adiciona evento "ENTRA" para o herói
        printf("%6d: AVISA PORTEIRO BASE %d ADMITE %d.\n", T, base->IDB,
			heroi_id);
        insere_evento(LEF_global, T, ENTRA, heroi->ID, base->IDB, -1);
    }
    return 0;
}

void desiste (int T, struct heroi *heroi, struct base *base, 
        int NBases) {
	// Nova base decidida de forma aleatória, e viaja para ela
	int nova_base;

	nova_base = aleat(0, NBases - 1);
	//loop para evitar ser a mesma base
	while (nova_base == base->IDB) 
		nova_base = aleat(0, NBases - 1);
    printf("%6d: DESISTE HEROI %2d BASE %d\n", T, heroi->ID, base->IDB);
	insere_evento(LEF_global, T, VIAJA, heroi->ID, nova_base, -1);
}

//cria e insere na LEF o evento CHEGA (agora + duração, H, D)
void viaja (int T, struct heroi *heroi, struct base *baseDestino,
        struct base *bases) {
	struct base *origem;
	double distancia;
	int duracao;

	distancia = 0;
	duracao = 0;
	// Base de origem do herói
    origem = &bases[heroi->IDbase];
	// Calculo da distância euclidiana entre bases
	distancia = sqrt(pow(baseDestino->loc.X - origem->loc.X, 2) +
		pow(baseDestino->loc.Y - origem->loc.Y, 2)); 

    if (distancia == 0)
        insere_evento(LEF_global, T + 0, CHEGA, heroi->ID, baseDestino->IDB,
            -1);
    else {
        // Duração da viagem arredondada
	    duracao = ceil(distancia / heroi->vel);
		// Inclui o evento chega
        insere_evento(LEF_global, T + duracao, CHEGA, heroi->ID,
            baseDestino->IDB, -1);
    }
    printf("%6d: VIAJA  HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n",
		T, heroi->ID, origem->IDB, baseDestino->IDB, (int)distancia, 
		heroi->vel, T + duracao);
}

int sai(int T, struct heroi *heroi, struct base *base, int NBases) {
    int nova_base;
    // Remove o herói do conjunto de presentes
    if (cjto_retira(base->pres, heroi->ID) == -1) {
        fprintf(stderr, "ERRO AO REMOVER HEROI DA BASE.\n");
        return -1;
    }
	printf("%6d: SAI HEROI %2d BASE %d (%2d/%2d)\n", T, heroi->ID, base->IDB,
		cjto_card(base->pres), base->lot);
    // Escolhe uma base aleatória para o herói
    nova_base = aleat(0, NBases - 1);
    insere_evento(LEF_global, T, VIAJA, heroi->ID, nova_base, -1);
    // Insere o evento AVISA para a base
    insere_evento(LEF_global, T, AVISA, heroi->ID, base->IDB, -1);

    return 0;
}

int entra (int T, struct heroi *heroi, struct base *base) {
    int tpb;

    if (cjto_insere(base->pres, heroi->ID) == -1) {
	fprintf(stderr, "ERRO PARA ADICIONAR O HEROI AO CONJUNTO BASE.\n");
        return -1;
    }
    // Calcula o tempo de permanência na base, insere o evento SAI 
    tpb = 15 + heroi->pac * aleat(1, 20);
	printf("%6d: ENTRA  HEROI %2d BASE %d (%2d/%2d) SAI %d\n", T, heroi->ID,
		base->IDB, cjto_card(base->pres), base->lot, T + tpb);
    insere_evento(LEF_global, T + tpb, SAI, heroi->ID, base->IDB, -1);
	
	return 0;
}

//retira H do conjunto de heróis presentes em B; muda o status de H para morto
int morre (int T, struct heroi *heroi, struct base *base, int missao_id) {
    if (!heroi || !base) {
        fprintf(stderr, "PONTEIRO NULO NA FUNÇÃO MORRE.\n");
        return -1;
    }
	// Remover o herói do conjunto de presentes na base
    if (cjto_retira(base->pres, heroi->ID) == -1) {
        fprintf(stderr, "ERRO PARA REMOVER O HEROI.\n");
        return -1;
    }
	// Atualizar o status do herói para morto
    heroi->vivo = 0;
    printf("%6d: MORRE HEROI %2d MISSAO %d", T, heroi->ID, missao_id);

	return 0;
}


//se não puder concluida marcada como impossivel e adiada 24horas
int missao(int T, struct missao *missao, struct base *bases, int NBases, 
        struct heroi *herois) {
    if (!missao || !bases || !herois) {
        fprintf(stderr, "Erro: Ponteiro para missão ou bases nulo.\n");
        return 0;
    }

    printf("%d: MISSAO %d TENT %d HAB REQ: [ ", T, missao->tentativas,
		missao->IDM);
    cjto_imprime(missao->hab);
    printf(" ]\n");
    //ba = base mais proxima apta
    struct base *bmp, *base;
    struct heroi *heroi;
    double menorDistancia;
	float risco;
    int i, achados, presentes;

    // Encontrar a base apta
    bmp = base_mais_proxima(missao, bases, herois, NBases, &menorDistancia, 
            T);
    
    // Se nenhuma base puder cumprir a missão, reagenda
    if (!bmp) {
        insere_evento(LEF_global, T + 24 * 60, MISSAO, -1, -1, missao->IDM);
        printf("%6d: MISSAO %d IMPOSSIVEL\n\n", T, missao->IDM);
		missao->tentativas++;
        return 0;
    }
	//Agora deixamos todas as bases com o conjunto de habilidades zerado
	for (i = 0; i < NBases; i++) {
		if (i != bmp->IDB){
			base = &bases[i];
			limpa_habilidades_base(base);
		}
	}
	// Processa a missão
    printf("%6d: MISSAO %d CUMPRIDA BASE: %d\n\n", T, missao->IDM, bmp->IDB);
	bmp->missões++;
    
    // Itera sobre os heróis presentes na base
	presentes = cjto_card(bmp->pres);
	i = achados = 0;
    while (achados < presentes) {
        if (cjto_pertence(bmp->pres, i)) {
            heroi = &herois[i];
			if (!heroi) {
                fprintf(stderr, "ERRO PONTEIRO DE HEROI NULO.\n");
                continue;
            }
			/*retirando as habilidades, para que em futuras missões
			o conjunto esteja vazio, para recalcular as habilidades da base*/
			limpa_habilidades_base(bmp);

			risco = missao->per / (heroi->pac + heroi->exp + 1);
            // Verifica se o herói morre
            if (risco > aleat(0, 30)) {
                insere_evento(LEF_global, T, MORRE, heroi->ID, bmp->IDB, 
					missao->IDM);
            } else {
                heroi->exp++;
                printf("%6d: HEROI %d GANHOU EXPERIENCIA. EXP ATUAL: %d\n", T,
                    heroi->ID, heroi->exp);
            }
			achados++;
        }
		i++;
    }
	printf("\n");
    return 1;
}

//encerra simulação, apresenta estatisticas herois, bases e das missoes 
int fim (int T, long int eventos, float media, int concluidas,
 		float missoes_concl, struct heroi *herois, struct base *bases) {
    struct base *base;
	struct heroi *heroi;
	float taxa_mortalidade, mortos;
	int i;

	mortos = 0;
	printf("\nFim da simulação %d.\n", T);
	printf("\n=== Estatísticas ===\n");
	for (i = 0; i < N_HEROIS; i++) {
		heroi = &herois[i];
		printf("HEROI %2d VIVO %3s PAC %3d VEL %4d EXP %4d HABS [", heroi->ID,
			heroi->vivo ? "SIM" : "NÃO", heroi->pac, heroi->vel, heroi->exp);
		cjto_imprime(heroi->hab);
		printf("]\n");
		if (!(heroi->vivo))
			mortos++;
	}
	taxa_mortalidade = 100 * mortos / N_HEROIS;
	for (i = 0; i < N_BASES; i++) {
        base = &bases[i];
        printf("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", base->IDB, 
			base->lot, base->FilaMax, base->missões);
    }
	printf("EVENTOS TRATADOS: %ld\n", eventos);
	printf("MISSÕES CUMPRIDAS: %d/%d (%.1f%%)\n", concluidas ,N_MISSOES, 
		missoes_concl);
	printf("TENTATIVAS/MISSAO: %.1f\n", media);
	printf("TAXA MORTALIDADE: %.1f%%\n", taxa_mortalidade);
    return 0;
}

//||||||||||||||||||||Funções que inicializam as estruturas:|||||||||||||||||||

void inicializa_heroi (struct heroi *heroi, int id) {
	heroi->ID = id;
	heroi->exp = 0;
	heroi->pac = aleat(0, 100);
	heroi->vel = aleat(50, 5000);
	heroi->hab = cjto_aleat(aleat(1, 3), N_HABILIDADES);
	heroi->IDbase = -1;
	heroi->vivo = 1;
}

void inicializa_base (struct base *base, int id) {
	base->IDB = id;
	base->loc.X = aleat(0, N_TAMANHO_MUNDO -1);
	base->loc.Y = aleat(0, N_TAMANHO_MUNDO -1);
	base->lot = aleat(3, 10);
	base->pres = cjto_cria(N_HEROIS);
	base->hab = cjto_cria(N_HABILIDADES);
	base->esp = lista_cria();
	base->tam = 0;
	base->FilaMax = 0;
	base->missões = 0;
}

void inicializa_missao (struct missao *missao, int id) {
	missao->IDM = id;
	missao->locM.X = aleat(0, N_TAMANHO_MUNDO -1);
	missao->locM.Y = aleat(0, N_TAMANHO_MUNDO -1);
	missao->hab = cjto_aleat(aleat(6, 10), N_HABILIDADES);
	missao->per = aleat(0, 100);
	missao->tentativas = 0;
}

void inicializa_mundo(struct mundo *mundo) {
	int i;

	// Configurações gerais do mundo
	mundo->NHerois = N_HEROIS;
	mundo->NBases = N_BASES;
	mundo->NMissoes = N_MISSOES;
	mundo->NHabilidades = N_HABILIDADES;
	mundo->tamanhoMundo.X = N_TAMANHO_MUNDO;
	mundo->tamanhoMundo.Y = N_TAMANHO_MUNDO;
	mundo->Relogio = T_INICIO;

	// Aloca memória para os vetores
	mundo->herois = malloc(sizeof(struct heroi) * N_HEROIS);
	mundo->bases = malloc(sizeof(struct base) * N_BASES);
	mundo->missoes = malloc(sizeof(struct missao) * N_MISSOES);

	if (!mundo->herois || !mundo->bases || !mundo->missoes) {
        fprintf(stderr, "Erro ao alocar memória para o mundo.\n");
        exit(1);
    }

	// Inicializando heróis, bases e missões
	for (i = 0; i < N_HEROIS; i++)
		inicializa_heroi(&mundo->herois[i], i);
	for (i = 0; i < N_BASES; i++) 
		inicializa_base(&mundo->bases[i], i);
	for (i = 0; i < N_MISSOES; i++)
		inicializa_missao(&mundo->missoes[i], i);
}

//||||||||||||||||||||||||||PROGRAMA PRINCIPAL ||||||||||||||||||||||||||||||||
int main () {
	// Semente para números aleatórios
	srand(0);
	// iniciar o mundo
	struct mundo *mundo;
	mundo = malloc(sizeof(struct mundo));
	if (!mundo) {
        fprintf(stderr, "Erro ao alocar memória para o mundo.\n");
        exit(1);
	}
    inicializa_lef();
	inicializa_mundo(mundo);

	long int eventos, tentativas;
	int concluidas;
	int  i, tempo, base_id;
	float missoes_concl, media;

	// Inicializa o mundo e as entidades
	concluidas = 0;
	eventos = tentativas = 0;
	// executar o laço de simulação

	// Agendar eventos iniciais
    for (i = 0; i < mundo->NHerois; i++) {
        tempo = aleat(0, 4320); // Três primeiros dias
        base_id = aleat(0, mundo->NBases - 1);
        insere_evento(LEF_global, tempo, CHEGA, i, base_id, -1);
		//print("%d %d %d", heroi->id, base->IDB, tempo); //
    }
	for (i = 0; i < mundo->NMissoes; i++) {
        tempo = aleat(0, T_FIM_DO_MUNDO);
        insere_evento(LEF_global, tempo, MISSAO, -1, -1, i);
    }
	
	// Agendar evento final
    insere_evento(LEF_global, T_FIM_DO_MUNDO, FIM, -1, -1, -1);
 
	// Laço principal da simulação
	Evento *evento;
	while (mundo->Relogio < T_FIM_DO_MUNDO) {
		// Atualiza o relógio
		evento = retira_evento(LEF_global);
		mundo->Relogio = evento->prio;
		eventos++;
		// Processa o evento
		switch(evento->tipo) {
			case CHEGA:
				chega(evento->prio, &mundo->herois[evento->heroi_id],
					&mundo->bases[evento->base_id]);
	   		        break;
			case ESPERA:
				espera(evento->prio, &mundo->herois[evento->heroi_id],
					&mundo->bases[evento->base_id]);
				break;
			case DESISTE:
				desiste(evento->prio, &mundo->herois[evento->heroi_id]
					,&mundo->bases[evento->base_id], mundo->NBases);
				break;
			case AVISA:
				avisa(evento->prio, &mundo->bases[evento->base_id],
					mundo->herois);
				break;
			case ENTRA:
				entra(evento->prio, &mundo->herois[evento->heroi_id]
					,&mundo->bases[evento->base_id]);
				break;
			case SAI:
				if (!mundo->herois[evento->heroi_id].vivo) {
					printf("Ignorando evento para herói morto: HEROI %d\n",
					evento->heroi_id);
					eventos--;
					break;
				}
				sai(evento->prio, &mundo->herois[evento->heroi_id],
					&mundo->bases[evento->base_id], mundo->NBases);
				break;
			case VIAJA:
				viaja(evento->prio, &mundo->herois[evento->heroi_id],
					&mundo->bases[evento->base_id], mundo->bases);
				break;
			case MISSAO:
				tentativas++;
				concluidas+=missao(evento->prio, 
					&mundo->missoes[evento->missao_id], mundo->bases,
					mundo->NBases, mundo->herois);
				break;
			case MORRE:
				morre(evento->prio, &mundo->herois[evento->heroi_id],
					&mundo->bases[evento->base_id], evento->missao_id);
				break;
			case FIM:
				missoes_concl = 100 * concluidas / N_MISSOES; 
				media = tentativas / N_MISSOES;
				fim(evento->prio, eventos, media, concluidas, missoes_concl,
					mundo->herois, mundo->bases);
				break;
		}
		// Libera memória do evento
		free(evento);
	}
	// destruir o mundo
	destroi_mundo(mundo);
	destroi_lef(LEF_global);

	return (0) ;
}
