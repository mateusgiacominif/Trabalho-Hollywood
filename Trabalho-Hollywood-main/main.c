#include "TARVBM_trab.h"
#include "TG.h"
#include "func_questoes.h"
#define N 1000

int main(void){
	int t, offset;
	
	
	FILE *arq_indices = fopen("bin/indices","r+b");
	if(!arq_indices){
		printf("Informe o grau minimo t da arvore: ");
    	scanf("%d", &t);
	
		TARVBM_cria("bin/indices", t);
		arq_indices = fopen("bin/indices","r+b");
		if(!arq_indices) exit(1);
		
		fread(&t, sizeof(int), 1, arq_indices);
    	fread(&offset, sizeof(int), 1, arq_indices);
		
		FILE *arq_nodes = fopen("data/Nodes.txt","r");
		if(!arq_nodes) exit(1);
		
		FILE *arq_rel = fopen("data/Relationships.txt", "r");
		FILE *arq_grafo = fopen("bin/relacionamentos.bin", "w+b");
		
		char *tipo, *nome, *ano, *frase_filme;
		char *linha = (char*)malloc(sizeof(char)*N);
		if(!linha) exit(1);
		
		while(fgets(linha, N, arq_nodes)){
			char *ponteiro_linha = linha;
			
			dados dado = {0};
			dado.offset_prim_viz = -1;
		
			tipo = divide_string(&ponteiro_linha);
			if(!tipo || strlen(tipo) == 0){
				continue;
			}
			strcpy(dado.tipo, tipo);
			
			if(strcmp(tipo,"Person") == 0){
				nome = divide_string(&ponteiro_linha);
				ano = divide_string(&ponteiro_linha);

				if(!nome || !ano) continue;
				
				strcpy(dado.nome, nome);
				dado.ano = atoi(ano);
				
				offset = TARVBM_insere(arq_indices, offset, nome, dado, t);
			}else{
				nome = divide_string(&ponteiro_linha);
				ano = divide_string(&ponteiro_linha);
				if(!nome || !ano) continue;
				frase_filme = divide_string(&ponteiro_linha);
				
				if(!nome || !ano || !frase_filme) continue;
				
				strcpy(dado.nome, nome);
				dado.ano = atoi(ano);
				if(!ano || !isdigit(ano[0]))continue;
				strcpy(dado.frase, frase_filme);
				
				offset = TARVBM_insere(arq_indices, offset, nome, dado, t);
			}
		}

		while(fgets(linha, N, arq_rel)){
			char *ponteiro_linha = linha;
			char *lixo = divide_string(&ponteiro_linha); // start person ou start movie joga fora

			char *nome_origem = divide_string(&ponteiro_linha);
			char *relacao = divide_string(&ponteiro_linha);
			
			lixo = divide_string(&ponteiro_linha); //pular end movie/person

			char *nome_destino = divide_string(&ponteiro_linha);
			if(!nome_origem || !nome_destino) continue;

			char *papel = ponteiro_linha; // pode ser NULL se não houver mais campos
			char papel_copia[150] = {0}; // sempre usa cópia para não depender de linha

			if(papel && strncmp(papel, "role: ", 6) == 0){
				papel += 6;
				char *nl = strchr(papel, '\n');
				if(nl) *nl = '\0';
				char *cr = strchr(papel, '\r');
				if(cr) *cr = '\0';
				strncpy(papel_copia, papel, 149);
			} else {
				strcpy(papel_copia, ""); //nao tem papel
			}
			
			insere_aresta_disco(arq_indices, arq_grafo, offset, nome_origem, nome_destino, relacao, papel_copia, t);
		}

		fseek(arq_indices, sizeof(int), SEEK_SET);
		fwrite(&offset, sizeof(int), 1, arq_indices);
		fclose(arq_nodes);
		fclose(arq_rel);
		fclose(arq_grafo);
		free(linha);
	}else{
		int id;
		fread(&t, sizeof(int), 1, arq_indices);
    	fread(&offset, sizeof(int), 1, arq_indices);
    	fread(&id, sizeof(int), 1, arq_indices);
    	printf("Arvore já existe com t=%d\n", t);
	}
    
	int opcao, sub_opcao;
	printf("(1) - Inserir atores, diretores, produtores ou filmes\n");
	printf("(2) - Retirar atores, diretores, produtores ou filmes\n");
	printf("(3) - Buscar nome\n");
	printf("(4) - Imprimir árvore\n");
	printf("(5) - Questões\n");
	printf("(6) - Sair e Salvar\n");
	printf("Escolha: ");
	
	scanf("%d", &opcao);

	while(opcao != 6) {
		if (opcao == 1 || opcao == 2) {
			printf("(1) - Ator, Diretor, Produtor\n");
			printf("(2) - Filme\n");
			printf("Escolha: ");
			scanf("%d", &sub_opcao);

			char nome_input[150];
			int ano_input;
			char frase_input[240] = {0};
			char tipo_input[15] = {0};

			if (sub_opcao == 1) {
				strcpy(tipo_input, "Person");
			} else if (sub_opcao == 2) {
				strcpy(tipo_input, "Movie");
			} else {
				printf("Opção invalida!\n");
				continue;
			}

			if (opcao == 1) {
				getchar(); // limpa o buffer
			
				printf("Digite o nome: ");
				fgets(nome_input, 150, stdin); 
				nome_input[strcspn(nome_input, "\n")] = '\0';
				
				printf("Digite o ano: ");
				scanf("%d", &ano_input);

				dados novo_dado = {0};
				novo_dado.offset_prim_viz = -1; // Nasce sem relacionamentos
				strcpy(novo_dado.nome, nome_input);
				strcpy(novo_dado.tipo, tipo_input);
				novo_dado.ano = ano_input;

				if (sub_opcao == 2) {
					getchar();
					
					printf("Digite a frase do filme: ");
					fgets(frase_input, 240, stdin);
					frase_input[strcspn(frase_input, "\n")] = '\0';					
					strcpy(novo_dado.frase, frase_input);
				}

				offset = TARVBM_insere(arq_indices, offset, nome_input, novo_dado, t);
				printf("%s foi inserido no banco.\n", nome_input);

			} else if (opcao == 2) {
				getchar();
			
				printf("Digite o nome para retirar: ");
				fgets(nome_input, 150, stdin); 
				nome_input[strcspn(nome_input, "\n")] = '\0';
				
				offset = TARVBM_retira(arq_indices, offset, nome_input, t);
				printf("%s foi retirado do banco.\n", nome_input);
			} 
		} else if (opcao == 3){
			getchar();
		
			printf("Digite o nome para buscar: ");
				char nome_input[150];
				fgets(nome_input, 150, stdin); 
				nome_input[strcspn(nome_input, "\n")] = '\0';
				
				TARVBM *no = TARVBM_busca(arq_indices, offset, nome_input, t);
			
			if (!no) {
				printf("%s nao foi encontrado no banco.\n", nome_input);
			} else {
				int i = 0;
				while (i < no->nchaves && strcmp(no->chave[i], nome_input) != 0) {
					i++;
				}
				
				if (i < no->nchaves && strcmp(no->chave[i], nome_input) == 0) {
					char nome_arq_folha[50];
					sprintf(nome_arq_folha, "bin/folha_%d.bin", no->filho[0]);
					FILE *f_folha = fopen(nome_arq_folha, "rb");
					
					if (f_folha) {
						dados d;
						fseek(f_folha, i * sizeof(dados), SEEK_SET);
						fread(&d, sizeof(dados), 1, f_folha);
						fclose(f_folha);
						
						printf("Tipo:  %s\n", d.tipo);
						printf("Nome:  %s\n", d.nome);
						printf("Ano:   %d\n", d.ano);
						if (strcmp(d.tipo, "Movie") == 0) {
							printf("Frase: %s\n", d.frase);
						}
					}
				} else {
					printf("%s nao foi encontrado no banco.\n", nome_input);
				}

				libera_no(no, t);
			}
		} else if (opcao == 4) {
			TARVBM_imprime(arq_indices, offset,t);
		} else if (opcao == 5) {
			int op_letra;
			printf("\n");
			printf(" (1) - (a) Todos que trabalharam juntos\n");
			printf(" (2) - (b) Atores e diretores que trabalharam juntos\n");
			printf(" (3) - (c) Atores que atuaram juntos\n");
			printf(" (4) - (d) Atores que mais atuaram juntos por decada\n");
			printf(" (5) - (e) Atores e diretores juntos por decada\n");
			printf(" (6) - (f) Atores que mais atuaram\n");
			printf(" (7) - (g) Atores que menos atuaram\n");
			printf(" (8) - (h) Diretores que mais dirigiram\n");
			printf(" (9) - (i) Diretores que menos dirigiram\n");
			printf("(10) - (j) Produtores mais produziram\n");
			printf("(11) - (k) Produtores menos produziram\n");
			printf("(12) - (l) Questoes de (f) a (k) por decada\n");
			printf("(13) - (m) Filmes que sao continuacoes\n");
			printf("(14) - (n) Atores que nasceram no mesmo ano\n");
			printf("(15) - (o) Atores que ja dirigiram\n");
			printf("(16) - (p) Atores que ja produziram\n");
			printf("(17) - (q) Retirar todos de um filme\n");
			printf("(18) - (r) Mesma pessoa escreveu, dirigiu e produziu o filme\n");
			printf("(19) - (s) Mesma pessoa dirigiu e produziu o filme\n");
			printf("(20) - (t) Atores que nasceram no ano de lancamento do filme\n");
			printf("Escolha o numero da opcao (1 a 20): ");
			scanf("%d",&op_letra);
			printf("\n");
			
			FILE *arq_grafo_leitura = fopen("bin/relacionamentos.bin", "rb");
			if (!arq_grafo_leitura) {
				printf("Erro ao abrir arquivo de relacionamentos!\n");
				continue;
			}

			switch (op_letra) {
				case 1: // (a)
					relatorio_duplas(arq_indices, offset, arq_grafo_leitura, t, 1);
					printf("\n");
					break;
				case 2: // (b)
					relatorio_duplas(arq_indices, offset, arq_grafo_leitura, t, 2);
					printf("\n");
					break;
				case 3: // (c)
					relatorio_duplas(arq_indices, offset, arq_grafo_leitura, t, 3);
					printf("\n");
					break;
				case 4: // (d)
					relatorio_decadas(arq_indices, offset, arq_grafo_leitura, t, 1);
					printf("\n");
					break;
				case 5: // (e)
					relatorio_decadas(arq_indices, offset, arq_grafo_leitura, t, 2);
					printf("\n");
					break;
				case 6: // (f)
					relatorio_podio(arq_indices, offset, arq_grafo_leitura, t, "ACTED_IN", 1);
					printf("\n");
					break;
				case 7: // (g)
					relatorio_podio(arq_indices, offset, arq_grafo_leitura, t, "ACTED_IN", 0);
					printf("\n");
					break;
				case 8: // (h)
					relatorio_podio(arq_indices, offset, arq_grafo_leitura, t, "DIRECTED", 1);
					printf("\n");
					break;
				case 9: // (i)
					relatorio_podio(arq_indices, offset, arq_grafo_leitura, t, "DIRECTED", 0);
					printf("\n");
					break;
				case 10: // (j)
					relatorio_podio(arq_indices, offset, arq_grafo_leitura, t, "PRODUCED", 1);
					printf("\n");
					break;
				case 11: // (k)
					relatorio_podio(arq_indices, offset, arq_grafo_leitura, t, "PRODUCED", 0);
					printf("\n");
					break;
				case 12: // (l)
					printf("Escolha qual filtrar por decada\n");
					printf(" (6) Atores que mais atuaram\n");
					printf(" (7) Atores que menos atuaram\n");
					printf(" (8) Diretores que mais dirigiram\n");
					printf(" (9) Diretores que menos dirigiram\n");
					printf("(10) Produtores mais atuantes\n");
					printf("(11) Produtores menos atuantes\n");
					printf("Escolha a sub-opcao: ");
					int sub_op;
					scanf("%d", &sub_op);
					if (sub_op == 6)       relatorio_podio_decadas(arq_indices, offset, arq_grafo_leitura, t, "ACTED_IN", 1);
					else if (sub_op == 7)  relatorio_podio_decadas(arq_indices, offset, arq_grafo_leitura, t, "ACTED_IN", 0);
					else if (sub_op == 8)  relatorio_podio_decadas(arq_indices, offset, arq_grafo_leitura, t, "DIRECTED", 1);
					else if (sub_op == 9)  relatorio_podio_decadas(arq_indices, offset, arq_grafo_leitura, t, "DIRECTED", 0);
					else if (sub_op == 10) relatorio_podio_decadas(arq_indices, offset, arq_grafo_leitura, t, "PRODUCED", 1);
					else if (sub_op == 11) relatorio_podio_decadas(arq_indices, offset, arq_grafo_leitura, t, "PRODUCED", 0);
					else printf("Sub-opcao invalida!\n");
					break;
					
				case 13: // (m)
					filmes_continuacoes(arq_indices, offset, t);
					break;
				case 14: // (n)
					atores_mesmo_ano(arq_indices, offset, t);
					break;
				case 15: // (o)
					atores_mais(arq_indices, offset, arq_grafo_leitura, t, "ACTED_IN", "DIRECTED");
					printf("\n");
					break;
				case 16: // (p)
					atores_mais(arq_indices, offset, arq_grafo_leitura, t, "ACTED_IN", "PRODUCED");
					printf("\n");
					break;
				case 17: // (q)
					getchar();
					
					char filme_alvo[150];
					printf("Digite o nome do filme para deletar a equipe: ");
					fgets(filme_alvo, 150, stdin);
					filme_alvo[strcspn(filme_alvo, "\n")] = '\0';
					
					offset = retira_equipe_filme(arq_indices, offset, arq_grafo_leitura, t, filme_alvo);
					break;
				case 18: // (r)
					filmes_mais(arq_indices, offset, arq_grafo_leitura, t, 1);
					printf("\n");
					break;
				case 19: // (s)
					filmes_mais(arq_indices, offset, arq_grafo_leitura, t, 0);
					printf("\n");
					break;
				case 20: // (t)
					atores_nascidos_ano_filme(arq_indices, offset, t);
					printf("\n");
					break;
					
				default:
					printf("Opcao invalida. Digite um numero de 1 a 20.\n");
					break;
			}
			fclose(arq_grafo_leitura);
		} else {
			printf("Opcao invalida\n");
		}
		printf("\n");
		printf("(1) - Inserir atores, diretores ou filmes\n");
		printf("(2) - Retirar atores, diretores ou filmes\n");
		printf("(3) - Buscar nome\n");
		printf("(4) - Imprimir árvore\n");
		printf("(5) - Questões\n");
		printf("(6) - Sair e Salvar\n");
		printf("Escolha: ");
		
		scanf("%d", &opcao);
	}
	
	fseek(arq_indices, sizeof(int), SEEK_SET);
	fwrite(&offset, sizeof(int), 1, arq_indices);
	
    fclose(arq_indices);

	return 0;
}
