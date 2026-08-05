#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TARVBM_trab.h"
#include "TG.h"
#include "func_questoes.h"

struct FilmeCount {
    char nome[150];
    int d, p, e;
};

// =========================================================================
// FUNÇÃO AUXILIAR
// =========================================================================

int acha_folha_mais_esq(FILE *arq_indices, int offset_raiz, int t) {
    if (offset_raiz == -1) return -1;
    
    int offset_atual = offset_raiz;
    TARVBM *no = le_no(arq_indices, offset_atual, t);
    
    while (no && !no->folha) {
        int prox_offset = no->filho[0];
        libera_no(no, t);
        offset_atual = prox_offset;
        no = le_no(arq_indices, offset_atual, t);
    }
    
    if (no) libera_no(no, t);
    return offset_atual;
}

// =========================================================================
// FUNÇÕES DE CAMPEÕES / PODIO
// =========================================================================

void acha_campeao(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *tipo_aresta, int buscar_maior, int *recorde, char *nome_campeao) {
    int offset_folha = acha_folha_mais_esq(arq_indices, offset_raiz, t);
    
    while (offset_folha != -1) {
        TARVBM *no = le_no(arq_indices, offset_folha, t);
        if (!no) break;

        char nome_arq_folha[50];
        sprintf(nome_arq_folha, "bin/folha_%d.bin", no->filho[0]);
        FILE *arq_dados = fopen(nome_arq_folha, "rb");

        if (arq_dados) {
            dados d;
            for (int i = 0; i < no->nchaves; i++) {
                fseek(arq_dados, i * sizeof(dados), SEEK_SET);
                fread(&d, sizeof(dados), 1, arq_dados);

                if (strcmp(d.tipo, "Person") == 0 && d.offset_prim_viz != -1) {
                    int contagem = 0;
                    long offset_atual = d.offset_prim_viz;
                    Relacionamentos rel; 

                    while (offset_atual != -1) {
                        fseek(arq_grafo, offset_atual, SEEK_SET);
                        fread(&rel, sizeof(Relacionamentos), 1, arq_grafo);

                        if (strcmp(rel.tipo_relacao, tipo_aresta) == 0) {
                            contagem++;
                        }
                        offset_atual = rel.offset_prox_viz;
                    }

                    if (contagem > 0) {
                        if (buscar_maior && contagem > *recorde) {
                            *recorde = contagem;
                            strcpy(nome_campeao, d.nome);
                        } 
                        else if (!buscar_maior && contagem < *recorde) {
                            *recorde = contagem;
                            strcpy(nome_campeao, d.nome);
                        }
                    }
                }
            }
            fclose(arq_dados);
        }
        
        offset_folha = no->prox; 
        libera_no(no, t);
    }
}

void relatorio_podio(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *tipo_aresta, int buscar_maior) {
    int recorde;
    if (buscar_maior)
        recorde = -1;
    else
        recorde = 999999;
    char nome_campeao[150] = "";

    acha_campeao(arq_indices, offset_raiz, arq_grafo, t, tipo_aresta, buscar_maior, &recorde, nome_campeao);

    if (strlen(nome_campeao) > 0) {
        if (buscar_maior) {
            printf("O %s que mais trabalhou foi %s são %d vezes!\n", tipo_aresta, nome_campeao, recorde);
        } else {
            printf("O %s que menos trabalhou foi %s são %d vezes!\n", tipo_aresta, nome_campeao, recorde);
        }
        
        brincar_com_grafo(arq_indices, offset_raiz, nome_campeao, t);
        
    } else {
        printf("Nenhum trabalho encontrado para a categoria %s.\n", tipo_aresta);
    }
}

// =========================================================================
// FUNÇÕES DE BUSCA BÁSICA
// =========================================================================

void atores_mais(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *req1, char *req2) {
    printf("Atores:\n");
    int achou = 0;
    int offset_folha = acha_folha_mais_esq(arq_indices, offset_raiz, t);

    while (offset_folha != -1) {
        TARVBM *no = le_no(arq_indices, offset_folha, t);
        if (!no) break;

        char nome_arq_folha[50];
        sprintf(nome_arq_folha, "bin/folha_%d.bin", no->filho[0]);
        FILE *arq_dados = fopen(nome_arq_folha, "rb");

        if (arq_dados) {
            dados d;
            for (int i = 0; i < no->nchaves; i++) {
                fseek(arq_dados, i * sizeof(dados), SEEK_SET);
                fread(&d, sizeof(dados), 1, arq_dados);

                if (strcmp(d.tipo, "Person") == 0 && d.offset_prim_viz != -1) {
                    int tem_req1 = 0, tem_req2 = 0;
                    long offset_atual = d.offset_prim_viz;
                    Relacionamentos rel; 

                    while (offset_atual != -1) {
                        fseek(arq_grafo, offset_atual, SEEK_SET);
                        fread(&rel, sizeof(Relacionamentos), 1, arq_grafo);

                        if (strcmp(rel.tipo_relacao, req1) == 0) tem_req1 = 1;
                        if (strcmp(rel.tipo_relacao, req2) == 0) tem_req2 = 1;

                        offset_atual = rel.offset_prox_viz;
                    }

                    if (tem_req1 && tem_req2) {
                        printf("%s\n", d.nome);
                        achou = 1;
                    }
                }
            }
            fclose(arq_dados);
        }
        offset_folha = no->prox;
        libera_no(no, t);
    }

    if (!achou) {
        printf("Não foi possivel encontrar atores.\n");
    }
}

void filmes_mais(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, int exige_escreveu){
    int offset_folha = acha_folha_mais_esq(arq_indices, offset_raiz, t);

    while (offset_folha != -1) {
        TARVBM *no = le_no(arq_indices, offset_folha, t);
        if (!no) break;

        char nome_arq_folha[50];
        sprintf(nome_arq_folha, "bin/folha_%d.bin", no->filho[0]);
        FILE *arq_dados = fopen(nome_arq_folha, "rb");

        if (arq_dados) {
            dados d;
            for (int i = 0; i < no->nchaves; i++) {
                fseek(arq_dados, i * sizeof(dados), SEEK_SET);
                fread(&d, sizeof(dados), 1, arq_dados);

                if (strcmp(d.tipo, "Person") == 0 && d.offset_prim_viz != -1) {
                    struct FilmeCount filmes[150];
                    int qtd = 0;
                    long offset_atual = d.offset_prim_viz;
                    Relacionamentos rel; 

                    while (offset_atual != -1) {
                        fseek(arq_grafo, offset_atual, SEEK_SET);
                        fread(&rel, sizeof(Relacionamentos), 1, arq_grafo);

                        int idx = -1;
                        for (int j = 0; j < qtd; j++) {
                            if (strcmp(filmes[j].nome, rel.nome_destino) == 0) {
                                idx = j; 
                                break;
                            }
                        }
                        
                        if (idx == -1) {
                            idx = qtd;
                            strcpy(filmes[idx].nome, rel.nome_destino);
                            filmes[idx].d = 0; filmes[idx].p = 0; filmes[idx].e = 0;
                            qtd++;
                        }

                        if (strcmp(rel.tipo_relacao, "DIRECTED") == 0) filmes[idx].d = 1;
                        if (strcmp(rel.tipo_relacao, "PRODUCED") == 0) filmes[idx].p = 1;
                        if (strcmp(rel.tipo_relacao, "WROTE") == 0) filmes[idx].e = 1;

                        offset_atual = rel.offset_prox_viz;
                    }

                    for (int j = 0; j < qtd; j++) {
                        if (exige_escreveu && filmes[j].d && filmes[j].p && filmes[j].e) {
                            printf("%s escreveu, dirigiu e produziu %s\n", d.nome,filmes[j].nome);
                        } else if (!exige_escreveu && filmes[j].d && filmes[j].p) {
                            printf("%s dirigiu e produziu %s\n", d.nome,filmes[j].nome);
                        }
                    }
                }
            }
            fclose(arq_dados);
        }
        offset_folha = no->prox;
        libera_no(no, t);
    }
}

// =========================================================================
// FUNÇÕES DE COLETA E COMPARAÇÕES DE DATAS
// =========================================================================

void coleta_equipe(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *filme, char nomes[][150], int *qtd) {
    int offset_folha = acha_folha_mais_esq(arq_indices, offset_raiz, t);

    while (offset_folha != -1) {
        TARVBM *no = le_no(arq_indices, offset_folha, t);
        if (!no) break;

        char nome_arq_folha[50];
        sprintf(nome_arq_folha, "bin/folha_%d.bin", no->filho[0]);
        FILE *arq_dados = fopen(nome_arq_folha, "rb");

        if (arq_dados) {
            dados d;
            for (int i = 0; i < no->nchaves; i++) {
                fseek(arq_dados, i * sizeof(dados), SEEK_SET);
                fread(&d, sizeof(dados), 1, arq_dados);

                if (strcmp(d.tipo, "Person") == 0 && d.offset_prim_viz != -1) {
                    long offset_atual = d.offset_prim_viz;
                    Relacionamentos rel;
                    int trabalhou = 0;

                    while (offset_atual != -1) {
                        fseek(arq_grafo, offset_atual, SEEK_SET);
                        fread(&rel, sizeof(Relacionamentos), 1, arq_grafo);

                        if (strcmp(rel.nome_destino, filme) == 0) {
                            trabalhou = 1;
                            break;
                        }
                        offset_atual = rel.offset_prox_viz;
                    }

                    if (trabalhou) {
                        strcpy(nomes[*qtd], d.nome);
                        (*qtd)++;
                    }
                }
            }
            fclose(arq_dados);
        }
        offset_folha = no->prox;
        libera_no(no, t);
    }
}

void coleta_filmes(FILE *arq_indices, int offset_raiz, int t, char nomes[][150], int *anos, int *qtd) {
    int offset_folha = acha_folha_mais_esq(arq_indices, offset_raiz, t);
 
    while (offset_folha != -1) {
        TARVBM *no = le_no(arq_indices, offset_folha, t);
        if (!no) break;
 
        char nome_arq_folha[50];
        sprintf(nome_arq_folha, "bin/folha_%d.bin", no->filho[0]);
        FILE *f = fopen(nome_arq_folha, "rb");
        
        if (f) {
            dados d;
            for (int i = 0; i < no->nchaves; i++) {
                fseek(f, i * sizeof(dados), SEEK_SET);
                fread(&d, sizeof(dados), 1, f);
                
                if (strcmp(d.tipo, "Movie") == 0) {
                    strcpy(nomes[*qtd], d.nome);
                    anos[*qtd] = d.ano;
                    (*qtd)++;
                }
            }
            fclose(f);
        }
        
        offset_folha = no->prox;
        libera_no(no, t);
    }
}

void coleta_pessoas(FILE *arq_indices, int offset_raiz, int t, char nomes[][150], int *anos, int *qtd) {
    int offset_folha = acha_folha_mais_esq(arq_indices, offset_raiz, t);
 
    while (offset_folha != -1) {
        TARVBM *no = le_no(arq_indices, offset_folha, t);
        if (!no) break;
 
        char nome_arq_folha[50];
        sprintf(nome_arq_folha, "bin/folha_%d.bin", no->filho[0]);
        FILE *f = fopen(nome_arq_folha, "rb");
        
        if (f) {
            dados d;
            for (int i = 0; i < no->nchaves; i++) {
                fseek(f, i * sizeof(dados), SEEK_SET);
                fread(&d, sizeof(dados), 1, f);
                
                if (strcmp(d.tipo, "Person") == 0 && d.ano > 0) {
                    strcpy(nomes[*qtd], d.nome);
                    anos[*qtd] = d.ano;
                    (*qtd)++;
                }
            }
            fclose(f);
        }
        offset_folha = no->prox;
        libera_no(no, t);
    }
}

int retira_equipe_filme(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *filme) {
    char nomes[500][150];
    int qtd = 0;

    coleta_equipe(arq_indices, offset_raiz, arq_grafo, t, filme, nomes, &qtd);

    int novo_offset = offset_raiz;
    for (int i = 0; i < qtd; i++) {
        novo_offset = TARVBM_retira(arq_indices, novo_offset, nomes[i], t);
        printf("%s removido(a) do banco.\n", nomes[i]);
    }

    if (qtd == 0) {
        printf("Nenhum membro da equipe encontrado para o filme %s.\n", filme);
    } else {
        printf("Total: %d pessoas removidas.\n", qtd);
    }

    return novo_offset;
}

void filmes_continuacoes(FILE *arq_indices, int offset_raiz, int t) {
    char nomes[200][150];
    int anos[200];
    int qtd = 0;
 
    coleta_filmes(arq_indices, offset_raiz, t, nomes, anos, &qtd);
 
    printf("Filmes que são continuações:\n");
 
    for (int i = 0; i < qtd; i++) {
        int len_i = strlen(nomes[i]);
        int tam = 0;
        for (int j = 0; j < qtd; j++)
            if (strncmp(nomes[j], nomes[i], len_i) == 0) tam++;

        if (tam < 2) continue;

        for (int j = 0; j < qtd; j++)
            if (strncmp(nomes[j], nomes[i], len_i) == 0)
                printf("%s (%d)\n", nomes[j], anos[j]);
        printf("\n");
    }
}

void atores_mesmo_ano(FILE *arq_indices, int offset_raiz, int t) {
    char nomes[500][150];
    int anos[500];
    int qtd = 0;
 
    coleta_pessoas(arq_indices, offset_raiz, t, nomes, anos, &qtd);
 
    int ja_visto[500] = {0};
 
    printf("Atores que nasceram no mesmo ano:\n");
    int achou = 0;
 
    for (int i = 0; i < qtd; i++) {
        if (ja_visto[i]) continue;
 
        int tem_par = 0;
        for (int j = i + 1; j < qtd; j++) {
            if (anos[j] == anos[i]) { 
                tem_par = 1;
                break; 
            }
        }
        if (!tem_par) continue;

        printf("Ano %d:\n", anos[i]);
        ja_visto[i] = 1;
        for (int j = i; j < qtd; j++) {
            if (anos[j] == anos[i]) {
                printf("%s\n", nomes[j]);
                ja_visto[j] = 1;
            }
        }
        printf("\n");
        achou = 1;
    }
 
    if (!achou)
        printf("Nenhum par encontrado.\n");
}

void atores_nascidos_ano_filme(FILE *arq_indices, int offset_raiz, int t) {
    char nomes_filmes[200][150];
    int anos_filmes[200];
    int qtd_filmes = 0;
 
    char nomes_pessoas[500][150];
    int anos_pessoas[500];
    int qtd_pessoas = 0;
 
    coleta_filmes(arq_indices, offset_raiz, t, nomes_filmes, anos_filmes, &qtd_filmes);
    coleta_pessoas(arq_indices, offset_raiz, t, nomes_pessoas, anos_pessoas, &qtd_pessoas);
 
    printf("Atores nascidos no ano de lançamento de um filme:\n");
    int achou = 0;
 
    for (int i = 0; i < qtd_filmes; i++) {
        int primeiro = 1;
        for (int j = 0; j < qtd_pessoas; j++) {
            if (anos_pessoas[j] == anos_filmes[i]) {
                if (primeiro) {
                    printf("Em %d foi lançado %s e %s", anos_filmes[i], nomes_filmes[i], nomes_pessoas[j]);
                    primeiro = 0;
                } else {
                    printf(", %s", nomes_pessoas[j]);
                }
                achou = 1;
            }
        }
        if (!primeiro) printf(" nasceram\n");
    }
 
    if (!achou)
        printf("Nenhum ator nasceu no ano de lançamento de algum filme.\n");
}

// =========================================================================
// FUNÇÕES DE DÉCADAS (PARES)
// =========================================================================

ParFreq* insere_ou_soma_par(ParFreq *lista, char *nome1, char *nome2, int decada) {
    char a[150], b[150];
    if (strcmp(nome1, nome2) < 0) {
        strcpy(a, nome1); strcpy(b, nome2);
    } else {
        strcpy(a, nome2); strcpy(b, nome1);
    }

    ParFreq *atual = lista;
    while (atual) {
        if (atual->decada == decada && strcmp(atual->p1, a) == 0 && strcmp(atual->p2, b) == 0) {
            atual->contagem++; 
            return lista;
        }
        atual = atual->prox;
    }

    ParFreq *novo = (ParFreq*)malloc(sizeof(ParFreq));
    strcpy(novo->p1, a);
    strcpy(novo->p2, b);
    novo->decada = decada;
    novo->contagem = 1;
    novo->prox = lista;
    return novo; 
}

ParFreq* varre_filmes_decada(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, int tipo_filtro, ParFreq *lista) {
    int offset_folha = acha_folha_mais_esq(arq_indices, offset_raiz, t);

    while (offset_folha != -1) {
        TARVBM *no = le_no(arq_indices, offset_folha, t);
        if (!no) break;

        char arq_folha[50];
        sprintf(arq_folha, "bin/folha_%d.bin", no->filho[0]);
        FILE *arq_dados = fopen(arq_folha, "rb");

        if (arq_dados) {
            dados d_filme;
            for (int i = 0; i < no->nchaves; i++) {
                fseek(arq_dados, i * sizeof(dados), SEEK_SET);
                fread(&d_filme, sizeof(dados), 1, arq_dados);

                if (strcmp(d_filme.tipo, "Movie") == 0 && d_filme.offset_prim_viz != -1) {
                    
                    int decada = (d_filme.ano / 10) * 10; 
                    long offset_aresta = d_filme.offset_prim_viz;
                    Relacionamentos rel;
                    
                    PessoaFilme elenco[200]; 
                    int qtd = 0;

                    while (offset_aresta != -1 && qtd < 200) {
                        fseek(arq_grafo, offset_aresta, SEEK_SET);
                        fread(&rel, sizeof(Relacionamentos), 1, arq_grafo);
                        
                        strcpy(elenco[qtd].nome, rel.nome_destino);
                        strcpy(elenco[qtd].relacao, rel.tipo_relacao);
                        qtd++;
                        
                        offset_aresta = rel.offset_prox_viz;
                    }

                    for (int x = 0; x < qtd; x++) {
                        for (int y = x + 1; y < qtd; y++) {
                            int valido = 0;

                            if (tipo_filtro == 1) {
                                if (strcmp(elenco[x].relacao, "ACTED_IN") == 0 && strcmp(elenco[y].relacao, "ACTED_IN") == 0) {
                                    valido = 1;
                                }
                            }
                            else if (tipo_filtro == 2) {
                                if ((strcmp(elenco[x].relacao, "ACTED_IN") == 0 && strcmp(elenco[y].relacao, "DIRECTED") == 0) ||
                                    (strcmp(elenco[x].relacao, "DIRECTED") == 0 && strcmp(elenco[y].relacao, "ACTED_IN") == 0)) {
                                    valido = 1;
                                }
                            }

                            if (valido) {
                                lista = insere_ou_soma_par(lista, elenco[x].nome, elenco[y].nome, decada);
                            }
                        }
                    }
                }
            }
            fclose(arq_dados);
        }
        offset_folha = no->prox;
        libera_no(no, t);
    }
    return lista;
}

void relatorio_decadas(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, int tipo_filtro) {
    ParFreq *lista_pares = NULL;
    lista_pares = varre_filmes_decada(arq_indices, offset_raiz, arq_grafo, t, tipo_filtro, lista_pares);

    if (!lista_pares) {
        printf("\nNenhum dado encontrado para gerar o relatorio.\n");
        return;
    }

    printf("\n\n");
    if (tipo_filtro == 1) printf(" (d) ATORES QUE MAIS ATUARAM JUNTOS POR DECADA\n");
    else if (tipo_filtro == 2) printf(" (e) ATORES E DIRETORES JUNTOS POR DECADA\n");
    printf("\n\n");

    for (int d = 1900; d <= 2050; d += 10) {
        int max_contagem = 0;
        
        ParFreq *atual = lista_pares;
        while (atual) {
            if (atual->decada == d && atual->contagem > max_contagem) {
                max_contagem = atual->contagem;
            }
            atual = atual->prox;
        }

        if (max_contagem > 0) {
            printf("\n--- DECADA DE %d ---\n", d);
            atual = lista_pares;
            while (atual) {
                if (atual->decada == d && atual->contagem == max_contagem) {
                    printf("- %s & %s (Trabalharam juntos %d vezes!)\n", atual->p1, atual->p2, atual->contagem);
                }
                atual = atual->prox;
            }
        }
    }
    printf("\n\n");

    while (lista_pares) {
        ParFreq *temp = lista_pares;
        lista_pares = lista_pares->prox;
        free(temp);
    }
}

// =========================================================================
// FUNÇÕES DE DÉCADAS (INDIVIDUAIS)
// =========================================================================

IndFreq* insere_ou_soma_ind(IndFreq *lista, char *nome, int decada);

IndFreq* insere_ou_soma_ind(IndFreq *lista, char *nome, int decada) {
    IndFreq *atual = lista;
    while (atual) {
        if (atual->decada == decada && strcmp(atual->nome, nome) == 0) {
            atual->contagem++;
            return lista;
        }
        atual = atual->prox;
    }
    
    IndFreq *novo = (IndFreq*)malloc(sizeof(IndFreq));
    strcpy(novo->nome, nome);
    novo->decada = decada;
    novo->contagem = 1;
    novo->prox = lista;
    return novo;
}

IndFreq *varre_filmes_ind(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *tipo_aresta, IndFreq *lista) {
    int offset_folha = acha_folha_mais_esq(arq_indices, offset_raiz, t);

    while (offset_folha != -1) {
        TARVBM *no = le_no(arq_indices, offset_folha, t);
        if (!no) break;

        char arq_folha[50];
        sprintf(arq_folha, "bin/folha_%d.bin", no->filho[0]);
        FILE *arq_dados = fopen(arq_folha, "rb");

        if (arq_dados) {
            dados d_filme;
            for (int i = 0; i < no->nchaves; i++) {
                fseek(arq_dados, i * sizeof(dados), SEEK_SET);
                fread(&d_filme, sizeof(dados), 1, arq_dados);

                if (strcmp(d_filme.tipo, "Movie") == 0 && d_filme.offset_prim_viz != -1) {
                    int decada = (d_filme.ano / 10) * 10;
                    long offset_aresta = d_filme.offset_prim_viz;
                    Relacionamentos rel;

                    PessoaFilme elenco[200];
                    int qtd = 0;

                    while (offset_aresta != -1 && qtd < 200) {
                        fseek(arq_grafo, offset_aresta, SEEK_SET);
                        fread(&rel, sizeof(Relacionamentos), 1, arq_grafo);
                        
                        strcpy(elenco[qtd].nome, rel.nome_destino);
                        strcpy(elenco[qtd].relacao, rel.tipo_relacao);
                        qtd++;
                        
                        offset_aresta = rel.offset_prox_viz;
                    }

                    for (int x = 0; x < qtd; x++) {
                        if (strcmp(elenco[x].relacao, tipo_aresta) == 0) {
                            lista = insere_ou_soma_ind(lista, elenco[x].nome, decada);
                        }
                    }
                }
            }
            fclose(arq_dados);
        }
        offset_folha = no->prox;
        libera_no(no, t);
    }
    return lista;
}

void relatorio_podio_decadas(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, char *tipo_aresta, int buscar_maior) {
    IndFreq *lista_ind = NULL;
    lista_ind = varre_filmes_ind(arq_indices, offset_raiz, arq_grafo, t, tipo_aresta, lista_ind);

    if (!lista_ind) {
        printf("\nNenhum registro encontrado para processar o podio.\n");
        return;
    }

    printf("\n\n");
    printf(" MAIOR FREQUENCIA POR DECADA: %s (%s VALOR)\n", tipo_aresta, buscar_maior ? "MAIOR" : "MENOR");
    printf("\n");

    for (int d = 1900; d <= 2050; d += 10) {
        int recorde = buscar_maior ? -1 : 999999;
        int teve_dados = 0;

        IndFreq *atual = lista_ind;
        while (atual) {
            if (atual->decada == d) {
                teve_dados = 1;
                if (buscar_maior && atual->contagem > recorde) {
                    recorde = atual->contagem;
                } else if (!buscar_maior && atual->contagem < recorde) {
                    recorde = atual->contagem;
                }
            }
            atual = atual->prox;
        }

        if (teve_dados && ((buscar_maior && recorde > 0) || (!buscar_maior && recorde < 999999))) {
            printf("\n--- DECADA DE %d (Recorde: %d registros) ---\n", d, recorde);
            atual = lista_ind;
            while (atual) {
                if (atual->decada == d && atual->contagem == recorde) {
                    printf(" %s com %d participacoes!\n", atual->nome, atual->contagem);
                }
                atual = atual->prox;
            }
        }
    }
    printf("\n\n");

    while (lista_ind) {
        IndFreq *temp = lista_ind;
        lista_ind = lista_ind->prox;
        free(temp);
    }
}

// =========================================================================
// FUNÇÕES DE DUPLAS (A, B e C)
// =========================================================================

void listar_duplas(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, int tipo_filtro) {
    int offset_folha = acha_folha_mais_esq(arq_indices, offset_raiz, t);

    while (offset_folha != -1) {
        TARVBM *no = le_no(arq_indices, offset_folha, t);
        if (!no) break;

        char nome_arq_folha[50];
        sprintf(nome_arq_folha, "bin/folha_%d.bin", no->filho[0]);
        FILE *arq_dados = fopen(nome_arq_folha, "rb");

        if (arq_dados) {
            dados d_pessoa;
            for (int i = 0; i < no->nchaves; i++) {
                fseek(arq_dados, i * sizeof(dados), SEEK_SET);
                fread(&d_pessoa, sizeof(dados), 1, arq_dados);

                if (strcmp(d_pessoa.tipo, "Person") == 0 && d_pessoa.offset_prim_viz != -1) {
                    long offset_filme = d_pessoa.offset_prim_viz;
                    Relacionamentos rel_filme;

                    while (offset_filme != -1) {
                        fseek(arq_grafo, offset_filme, SEEK_SET);
                        fread(&rel_filme, sizeof(Relacionamentos), 1, arq_grafo);

                        int passa_filtro_1 = 0;
                        if (tipo_filtro == 1) passa_filtro_1 = 1; 
                        else if (tipo_filtro == 2 && (strcmp(rel_filme.tipo_relacao, "ACTED_IN") == 0 || strcmp(rel_filme.tipo_relacao, "DIRECTED") == 0)) passa_filtro_1 = 1; 
                        else if (tipo_filtro == 3 && strcmp(rel_filme.tipo_relacao, "ACTED_IN") == 0) passa_filtro_1 = 1; 

                        if (passa_filtro_1) {
                            TARVBM *no_filme = TARVBM_busca(arq_indices, offset_raiz, rel_filme.nome_destino, t);
                            if (no_filme) {
                                int j = 0;
                                while (j < no_filme->nchaves && strcmp(no_filme->chave[j], rel_filme.nome_destino) != 0) j++;

                                char arq_folha_f[50];
                                sprintf(arq_folha_f, "bin/folha_%d.bin", no_filme->filho[0]);
                                FILE *f_folha_filme = fopen(arq_folha_f, "rb");

                                if (f_folha_filme) {
                                    dados d_filme;
                                    fseek(f_folha_filme, j * sizeof(dados), SEEK_SET);
                                    fread(&d_filme, sizeof(dados), 1, f_folha_filme);
                                    fclose(f_folha_filme);

                                    long offset_colega = d_filme.offset_prim_viz;
                                    Relacionamentos rel_colega;

                                    while (offset_colega != -1) {
                                        fseek(arq_grafo, offset_colega, SEEK_SET);
                                        fread(&rel_colega, sizeof(Relacionamentos), 1, arq_grafo);

                                        int passa_filtro_2 = 0;
                                        if (tipo_filtro == 1) passa_filtro_2 = 1; 
                                        else if (tipo_filtro == 2 && (strcmp(rel_colega.tipo_relacao, "ACTED_IN") == 0 || strcmp(rel_colega.tipo_relacao, "DIRECTED") == 0)) passa_filtro_2 = 1; 
                                        else if (tipo_filtro == 3 && strcmp(rel_colega.tipo_relacao, "ACTED_IN") == 0) passa_filtro_2 = 1; 

                                        if (passa_filtro_2 && strcmp(d_pessoa.nome, rel_colega.nome_destino) < 0) {
                                            printf(" -> %s e %s (em %s)\n", d_pessoa.nome, rel_colega.nome_destino, rel_filme.nome_destino);
                                        }

                                        offset_colega = rel_colega.offset_prox_viz;
                                    }
                                }
                                libera_no(no_filme, t);
                            }
                        }
                        offset_filme = rel_filme.offset_prox_viz;
                    }
                }
            }
            fclose(arq_dados);
        }
        offset_folha = no->prox;
        libera_no(no, t);
    }
}

void relatorio_duplas(FILE *arq_indices, int offset_raiz, FILE *arq_grafo, int t, int tipo_filtro) {
    if (tipo_filtro == 1) printf(" (a) Todos que trabalham juntos\n");
    else if (tipo_filtro == 2) printf(" (b) Atores e diretores que trabalham juntos\n");
    else if (tipo_filtro == 3) printf(" (c) Atores que atuaram juntos\n");

    listar_duplas(arq_indices, offset_raiz, arq_grafo, t, tipo_filtro);
}
