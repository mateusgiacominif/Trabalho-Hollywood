//estruturas auxiliares para questao de guerra
typedef struct {
    char nome[150];
    char relacao[20];
} PessoaFilme;

typedef struct par_freq {
    char p1[150];
    char p2[150];
    int decada;
    int contagem;
    struct par_freq *prox;
} ParFreq;

// questao l, maior frequencia por decada
typedef struct ind_freq {
    char nome[150];
    int decada;
    int contagem;
    struct ind_freq *prox;
} IndFreq;

void relatorio_podio(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *tipo_aresta, int buscar_maior);
void atores_mais(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *req1, char *req2);
void filmes_mais(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, int exige_escreveu);
int retira_equipe_filme(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *filme);
void filmes_continuacoes(FILE *arq_indices, int offset_raiz, int t);
void atores_mesmo_ano(FILE *arq_indices, int offset_raiz, int t);
void atores_nascidos_ano_filme(FILE *arq_indices, int offset_raiz, int t);
void relatorio_duplas(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, int tipo_filtro);
void relatorio_decadas(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, int tipo_filtro);
void relatorio_podio_decadas(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *tipo_aresta, int buscar_maior);

