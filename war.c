// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- DEFINIÇÃO DA ESTRUTURA ---
typedef struct territorio {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// --- PROTÓTIPOS DE FUNÇÕES ---
void liberarMemoria(Territorio *mapa);
void atacar(Territorio *atacante, Territorio *defensor);
void exibirTerritorios(Territorio *mapa, int numTerritorios);
int verificarVitoria(Territorio *mapa, int numTerritorios);
void lerString(char *buffer, int tamanho, const char *prompt);

// --- VARIÁVEL GLOBAL ---
int numTerritoriosGlobal = 0;

// --- FUNÇÃO PRINCIPAL ---
int main() {
    srand(time(NULL)); 
    
    printf("==============================\nVamos começar nossa guerra?\n==============================\n");
    printf("Premeiro digite o numero total de territorios para o mapa: ");
    
    // 1. Alocação e validação inicial
    if (scanf("%d", &numTerritoriosGlobal) != 1 || numTerritoriosGlobal <= 1) {
        printf("Numero invalido. Encerrando.\n");
        return 1;
    }
    int c; while ((c = getchar()) != '\n' && c != EOF) {}

    // *** ALOCAÇÃO DINÂMICA (calloc) ***
    Territorio *mapa = (Territorio *)calloc(numTerritoriosGlobal, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    printf("\n--- CADASTRO INICIAL ---\n");
    
    // 2. Loop para cadastro dos territórios
    for (int i = 0; i < numTerritoriosGlobal; i++) {
        printf("\n== TERRITORIO #%d ==\n", i + 1);
        lerString(mapa[i].nome, sizeof(mapa[i].nome), "Nome: "); 
        lerString(mapa[i].cor, sizeof(mapa[i].cor), "Cor ( EX: Azul, Verde): ");

        printf("Quantidade de tropas (min 1): ");
        if (scanf("%d", &mapa[i].tropas) != 1 || mapa[i].tropas < 1) {
            mapa[i].tropas = 1; 
            printf("Invalida. Tropa definida para 1.\n");
        } 
        while ((c = getchar()) != '\n' && c != EOF) {}
    }

    exibirTerritorios(mapa, numTerritoriosGlobal);

    // 3. LOOP PRINCIPAL DO JOGO (Rodadas de Ataque)
    int rodada = 1;
    while (verificarVitoria(mapa, numTerritoriosGlobal) == 0) {
        printf("\n\n=============== RODADA %d: FASE DE ATAQUE ===============\n", rodada);
        
        int idxAtacante, idxDefensor;

        // Solicita e valida o atacante
        printf("Atacante (1 a %d, 0 para sair): ", numTerritoriosGlobal);
        if (scanf("%d", &idxAtacante) != 1 || idxAtacante == 0) { break; }
        if (idxAtacante < 1 || idxAtacante > numTerritoriosGlobal) {
            printf("\nSelecao do defensor invalida. Tente novamente.\n");
            rodada++; while ((c = getchar()) != '\n' && c != EOF) {} continue;
        }
        int indiceA = idxAtacante - 1;

        // Solicita e valida o defensor
        printf("Defensor (1 a %d): ", numTerritoriosGlobal);
        if (scanf("%d", &idxDefensor) != 1 || idxDefensor < 1 || idxDefensor > numTerritoriosGlobal) {
             printf("\nSelecao do defensor invalida. Tente novamente.\n");
             rodada++; while ((c = getchar()) != '\n' && c != EOF) {} continue;
        }
        int indiceD = idxDefensor - 1;

        while ((c = getchar()) != '\n' && c != EOF) {}

        // Validação de regras do jogo e execução do ataque
        if (indiceA == indiceD) {
            printf("\nErro: Um territorio nao pode atacar a si mesmo.\n");
        } else if (strcmp(mapa[indiceA].cor, mapa[indiceD].cor) == 0) {
            printf("\nErro: Você nao pode atacar um territorio da propria cor ('%s').\n", mapa[indiceA].cor);
        } else if (mapa[indiceA].tropas < 2) {
            printf("\nErro: O atacante precisa de pelo menos 2 tropas.\n");
        } else {
            atacar(&mapa[indiceA], &mapa[indiceD]);
        }
        
        exibirTerritorios(mapa, numTerritoriosGlobal);
        rodada++;
    }

    // 4. Fim de jogo e liberação de memória
    printf("\n\n================================================\n!!! O JOGO TERMINOU !!!\n");
    printf("A cor '%s' CONQUISTOU TODOS OS TERRITORIOS!\n================================================\n", mapa[0].cor);
    
    liberarMemoria(mapa);
    return 0;
}

// --- IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES ---

/**
 * Le uma string do console e limpa o caractere de newline.
 */
void lerString(char *buffer, int tamanho, const char *prompt) {
    printf("%s", prompt);
    fgets(buffer, tamanho, stdin);
    buffer[strcspn(buffer, "\n")] = 0; 
}

/**
 * Verifica se apenas uma cor (dono) possui todos os territórios.
 */
int verificarVitoria(Territorio *mapa, int numTerritorios) {
    if (numTerritorios <= 0) return 1; 

    for (int i = 1; i < numTerritorios; i++) {
        if (strcmp(mapa[i].cor, mapa[0].cor) != 0) {
            return 0; // Jogo continua
        }
    }
    return 1; // Vencedor!
}

/**
 * Libera o bloco de memória.
 */
void liberarMemoria(Territorio *mapa) {
    if (mapa != NULL) {
        free(mapa);
        printf("\nMemoria alocada liberada com sucesso.\n");
    }
}

/**
 * Simula o ataque.
 */
void atacar(Territorio *atacante, Territorio *defensor) {
    printf("\n--- SIMULAÇÃO DE ATAQUE ---\n");
    printf("ATACANTE: %s (%s, %d tropas) vs DEFENSOR: %s (%s, %d tropas)\n", 
           atacante->nome, atacante->cor, atacante->tropas, defensor->nome, defensor->cor, defensor->tropas);

    int dadoAtacante = (rand() % 6) + 1;
    int dadoDefensor = (rand() % 6) + 1;

    printf("Dado Atacante: %d | Dado Defensor: %d\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf("\n*** VITORIA DO ATACANTE! %s CONQUISTOU %s! ***\n", atacante->nome, defensor->nome);
        
        strcpy(defensor->cor, atacante->cor);
        
        int tropasTransferidas = defensor->tropas / 2;
        defensor->tropas -= tropasTransferidas;
        atacante->tropas += tropasTransferidas;
        
        printf("%d tropas transferidas. Nova cor: %s.\n", tropasTransferidas, defensor->cor);

    } else {
        printf("\n*** VITORIA DO DEFENSOR! O ataque de %s falhou. ***\n", atacante->nome);
        
        if (atacante->tropas > 1) {
            atacante->tropas--;
            printf("%s perde 1 tropa de ataque.\n", atacante->nome);
        } else {
            printf("%s não perdeu tropas (minimo mantido).\n", atacante->nome);
        }
    }
}

/**
 * Exibe a lista de todos os territórios e seu estado atual.
 */
void exibirTerritorios(Territorio *mapa, int numTerritorios) {
    printf("\n\n==============================\nESTADO ATUAL DOS TERRITORIOS\n==============================\n");
    for (int i = 0; i < numTerritorios; i++) {
        printf("\n--- TERRITORIO #%d ---\nNome: %s\nCor (Dono): %s\nTropas: %d\n", 
               i + 1, (mapa + i)->nome, (mapa + i)->cor, (mapa + i)->tropas);
    }