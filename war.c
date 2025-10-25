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

// --- DEFINIÇÕES DE ESTRUTURAS ---

typedef struct territorio {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

typedef struct jogador {
    char cor[10];
    char *missao; // Ponteiro para a string da missão, alocada dinamicamente
} Jogador;

// --- DEFINIÇÕES DE CONSTANTES ---
#define MAX_JOGADORES 5
#define MAX_MISSOES 5

// Vetor de strings para as missões (constante, alocada estaticamente)
const char *MISSOES[] = {
    "Conquistar no mínimo 3 territorios com 5 ou mais tropas.",
    "Controlar todos os territorios da cor 'Verde'.",
    "Eliminar completamente o jogador da cor 'Vermelho'.", // Missão de eliminação
    "Ter pelo menos 10 tropas em um unico territorio.",
    "Conquistar 2 territorios consecutivos (na mesma rodada)." // Simplificado para "Manter a cor por 2 rodadas" na verificação
};

// --- VARIÁVEIS GLOBAIS (para simplificar a passagem de parâmetros) ---
int numTerritoriosGlobal = 0;
int numJogadoresGlobal = 0;

// --- PROTÓTIPOS DE FUNÇÕES ---
void liberarMemoria(Territorio *mapa, Jogador *jogadores);
void atacar(Territorio *atacante, Territorio *defensor);
void exibirTerritorios(Territorio *mapa, int numTerritorios);
int verificarVitoriaGlobal(Territorio *mapa, int numTerritorios);

// Funcionalidade de Missão
void atribuirMissao(Jogador *jogador);
int verificarMissao(char *missao, Jogador *jogador, Territorio *mapa, int tamanho);
int verificarVitoria(Jogador *jogadores, int numJogadores, Territorio *mapa, int numTerritorios);

// Funções Auxiliares
void lerString(char *buffer, int tamanho, const char *prompt);
char* obterCorDoDono(Territorio *mapa, int indice); // Obtém a cor do dono do território
int contarTerritoriosPorCor(Territorio *mapa, int tamanho, const char *cor);
int verificarEliminacao(Territorio *mapa, int tamanho, const char *corInimiga);

// --- FUNÇÃO PRINCIPAL ---
int main() {
    srand(time(NULL)); 
    
    printf("==============================\nVamos comecar a nossa guerra?\n==============================\n");
    
    // 1. Configuração de Jogadores
    printf("Digite o numero de JOGADORES (max. %d): ", MAX_JOGADORES);
    if (scanf("%d", &numJogadoresGlobal) != 1 || numJogadoresGlobal < 2 || numJogadoresGlobal > MAX_JOGADORES) {
        printf("Numero de jogadores invalido. Encerrando.\n");
        return 1;
    }
    int c; while ((c = getchar()) != '\n' && c != EOF) {}

    // Alocação dinâmica para os jogadores
    Jogador *jogadores = (Jogador *)calloc(numJogadoresGlobal, sizeof(Jogador));
    if (jogadores == NULL) {
        printf("Erro ao alocar memoria para jogadores.\n");
        return 1;
    }

    printf("\n--- CADASTRO E ATRIBUICAO DE MISSOES ---\n");
    for (int i = 0; i < numJogadoresGlobal; i++) {
        printf("\n== JOGADOR #%d ==\n", i + 1);
        lerString(jogadores[i].cor, sizeof(jogadores[i].cor), "Cor do Jogador: ");
        atribuirMissao(&jogadores[i]); // Passagem por referência para modificar a struct
        printf("Missao de %s (Cor %s): %s\n", jogadores[i].cor, jogadores[i].cor, jogadores[i].missao);
    }

    // 2. Configuração de Territórios
    printf("\nDigite o numero total de TERRITORIOS para o mapa (min. 2): ");
    if (scanf("%d", &numTerritoriosGlobal) != 1 || numTerritoriosGlobal <= 1) {
        printf("Número invalido. Encerrando.\n");
        liberarMemoria(NULL, jogadores);
        return 1;
    }
    while ((c = getchar()) != '\n' && c != EOF) {}

    // Alocação dinâmica para os territórios
    Territorio *mapa = (Territorio *)calloc(numTerritoriosGlobal, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro ao alocar memoria para mapa.\n");
        liberarMemoria(NULL, jogadores);
        return 1;
    }

    printf("\n--- CADASTRO INICIAL DOS TERRITORIOS ---\n");
    for (int i = 0; i < numTerritoriosGlobal; i++) {
        printf("\n== TERRITORIO #%d ==\n", i + 1);
        lerString(mapa[i].nome, sizeof(mapa[i].nome), "Nome: "); 
        lerString(mapa[i].cor, sizeof(mapa[i].cor), "Cor (Dono): ");
        printf("Quantidade de tropas (min 1): ");
        if (scanf("%d", &mapa[i].tropas) != 1 || mapa[i].tropas < 1) {
            mapa[i].tropas = 1; 
            printf("Invalida. Tropa definida para 1.\n");
        } 
        while ((c = getchar()) != '\n' && c != EOF) {}
    }

    exibirTerritorios(mapa, numTerritoriosGlobal);

    // 3. LOOP PRINCIPAL DO JOGO (Rodadas de Ataque e Verificação)
    int rodada = 1;
    while (verificarVitoria(jogadores, numJogadoresGlobal, mapa, numTerritoriosGlobal) == -1) {
        printf("\n\n=============== RODADA %d: FASE DE ATAQUE ===============\n", rodada);
        
        int idxAtacante, idxDefensor;

        // Lógica de ataque (simplificada para entrada direta do usuário)
        printf("Atacante (1 a %d, 0 para sair): ", numTerritoriosGlobal);
        if (scanf("%d", &idxAtacante) != 1 || idxAtacante == 0) { break; }
        // ... (validações omitidas para brevidade, mas são importantes) ...

        printf("Defensor (1 a %d): ", numTerritoriosGlobal);
        if (scanf("%d", &idxDefensor) != 1 || idxDefensor < 1 || idxDefensor > numTerritoriosGlobal) {
             printf("\nSelecao do defensor inválida. Rodada finalizada.\n");
             rodada++; while ((c = getchar()) != '\n' && c != EOF) {} continue;
        }

        int indiceA = idxAtacante - 1;
        int indiceD = idxDefensor - 1;

        while ((c = getchar()) != '\n' && c != EOF) {}

        // Validação básica
        if (indiceA < 0 || indiceA >= numTerritoriosGlobal || indiceD < 0 || indiceD >= numTerritoriosGlobal || indiceA == indiceD) {
            printf("\nErro na selecao de territorios.\n");
        } else if (strcmp(mapa[indiceA].cor, mapa[indiceD].cor) == 0) {
            printf("\nErro: Nao pode atacar territorio da propria cor ('%s').\n", mapa[indiceA].cor);
        } else if (mapa[indiceA].tropas < 2) {
            printf("\nErro: Atacante precisa de pelo menos 2 tropas.\n");
        } else {
            atacar(&mapa[indiceA], &mapa[indiceD]);
        }
        
        exibirTerritorios(mapa, numTerritoriosGlobal);
        
        // Verificação de vitória por MISSÃO
        int vencedorIdx = verificarVitoria(jogadores, numJogadoresGlobal, mapa, numTerritoriosGlobal);
        if (vencedorIdx != -1) {
            printf("\n\n!!! VITORIA POR MISSAO !!!\n");
            printf("O JOGADOR DA COR '%s' VENCEU AO CUMPRIR A MISSAO:\n-> %s\n", 
                   jogadores[vencedorIdx].cor, jogadores[vencedorIdx].missao);
            break;
        }
        
        // Verificação de vitória por Conquista Global (Regra Antiga)
        int vencedorGlobal = verificarVitoriaGlobal(mapa, numTerritoriosGlobal);
        if (vencedorGlobal == 1) {
            printf("\n\n!!! VITORIA POR CONQUISTA TOTAL !!!\n");
            printf("O JOGADOR DA COR '%s' CONQUISTOU TODOS OS TERRITORIOS!\n", mapa[0].cor);
            break;
        }

        rodada++;
    }

    // 4. Liberação de memória
    liberarMemoria(mapa, jogadores);
    return 0;
}

// --- IMPLEMENTAÇÃO DAS FUNÇÕES DE MISSÃO ---

/**
 * Sorteia e aloca a missão para o jogador.
 * @param jogador Ponteiro para o jogador cuja missão será definida (Referência).
 */
void atribuirMissao(Jogador *jogador) {
    int indiceSorteado = rand() % MAX_MISSOES;
    const char *missaoSorteada = MISSOES[indiceSorteado];
    size_t tamanhoMissao = strlen(missaoSorteada) + 1;

    // Alocação dinâmica de memória para a string da missão
    jogador->missao = (char *)malloc(tamanhoMissao);
    if (jogador->missao == NULL) {
        fprintf(stderr, "Erro de alocação de missão para %s.\n", jogador->cor);
        exit(EXIT_FAILURE);
    }

    // Cópia da missão sorteada
    strcpy(jogador->missao, missaoSorteada);
}

/**
 * Avalia se a missão de um jogador foi cumprida.
 * (Lógica simplificada baseada na descrição da missão)
 * @param missao A string da missão do jogador.
 * @param jogador A struct do jogador.
 * @param mapa O vetor de territórios.
 * @param tamanho O número total de territórios.
 * @return 1 se cumprida, 0 caso contrário.
 */
int verificarMissao(char *missao, Jogador *jogador, Territorio *mapa, int tamanho) {
    // 1. Conquistar 3 territórios com 5 ou mais tropas
    if (strstr(missao, "3 territórios com 5 ou mais tropas")) {
        int contagem = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, jogador->cor) == 0 && mapa[i].tropas >= 5) {
                contagem++;
            }
        }
        return (contagem >= 3);
    }

    // 2. Controlar todos os territórios da cor 'Verde'.
    if (strstr(missao, "Controlar todos os territórios da cor 'Verde'")) {
        if (strcmp(jogador->cor, "Verde") == 0) return 0; // Não pode ser a própria cor (simplificação)
        return (contarTerritoriosPorCor(mapa, tamanho, "Verde") == numTerritoriosGlobal); 
        // Lógica simplificada: Se todos os territórios forem de uma cor específica,
        // o jogador que tiver essa missão E NÃO for essa cor, vence.
    }
    
    // 3. Eliminar completamente o jogador da cor 'Vermelha'.
    if (strstr(missao, "Eliminar completamente o jogador da cor 'Vermelha")) {
         if (strcmp(jogador->cor, "Vermelha") == 0) return 0; // Não pode se eliminar
         return verificarEliminacao(mapa, tamanho, "Vermelha");
    }

    // 4. Ter pelo menos 10 tropas em um único território.
    if (strstr(missao, "Ter pelo menos 10 tropas em um único territorio")) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, jogador->cor) == 0 && mapa[i].tropas >= 10) {
                return 1;
            }
        }
        return 0;
    }

    // 5. Conquistar 2 territórios consecutivos (Simplificação: apenas verificar se tem pelo menos 2 territórios)
    if (strstr(missao, "2 territorios consecutivos")) {
        int count = contarTerritoriosPorCor(mapa, tamanho, jogador->cor);
        return (count >= 2);
    }

    return 0; // Missão não reconhecida ou não cumprida
}

/**
 * Itera sobre todos os jogadores e verifica se algum cumpriu sua missão.
 * @return O índice do jogador vencedor (0 a N-1) ou -1 se ninguém venceu.
 */
int verificarVitoria(Jogador *jogadores, int numJogadores, Territorio *mapa, int numTerritorios) {
    for (int i = 0; i < numJogadores; i++) {
        // Passagem da missão (por valor/cópia da string) e do jogador/mapa (por referência)
        if (verificarMissao(jogadores[i].missao, &jogadores[i], mapa, numTerritorios)) {
            return i; // Retorna o índice do jogador vencedor
        }
    }
    return -1; // Nenhum vencedor por missão
}


// --- IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES ---

/**
 * Libera a memória alocada dinamicamente para o mapa e as missões dos jogadores.
 */
void liberarMemoria(Territorio *mapa, Jogador *jogadores) {
    if (mapa != NULL) {
        free(mapa);
    }
    if (jogadores != NULL) {
        for (int i = 0; i < numJogadoresGlobal; i++) {
            if (jogadores[i].missao != NULL) {
                free(jogadores[i].missao); // Libera a string da missão
            }
        }
        free(jogadores); // Libera o vetor de jogadores
    }
    printf("\nMemoria alocada (Mapa e Missoes) liberada com sucesso.\n");
}

/**
 * Verifica a condição de vitória por conquista global (regra antiga).
 */
int verificarVitoriaGlobal(Territorio *mapa, int numTerritorios) {
    if (numTerritorios <= 0) return 1; 

    for (int i = 1; i < numTerritorios; i++) {
        if (strcmp(mapa[i].cor, mapa[0].cor) != 0) {
            return 0; // Jogo continua
        }
    }
    return 1; // Vencedor!
}

/**
 * Conta quantos territórios pertencem a uma cor específica.
 */
int contarTerritoriosPorCor(Territorio *mapa, int tamanho, const char *cor) {
    int cont = 0;
    for (int i = 0; i < tamanho; i++) {
        if (strcmp(mapa[i].cor, cor) == 0) {
            cont++;
        }
    }
    return cont;
}

/**
 * Verifica se o jogador da cor inimiga foi eliminado (não controla mais nenhum território).
 */
int verificarEliminacao(Territorio *mapa, int tamanho, const char *corInimiga) {
    for (int i = 0; i < tamanho; i++) {
        if (strcmp(mapa[i].cor, corInimiga) == 0) {
            return 0; // O jogador inimigo ainda tem territórios
        }
    }
    return 1; // O jogador inimigo foi eliminado
}

// (Funções atacar, exibirTerritorios e lerString não foram alteradas e são mantidas da versão anterior.)

/**
 * Função que simula o ataque.
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
            printf("%s nao perdeu tropas (minimo mantido).\n", atacante->nome);
        }
    }
}

/**
 * Exibe a lista de todos os territórios e seu estado atual (printf condensado).
 */
void exibirTerritorios(Territorio *mapa, int numTerritorios) {
    printf("\n\n==============================\nESTADO ATUAL DOS TERRITORIOS\n==============================\n");
    for (int i = 0; i < numTerritorios; i++) {
        printf("\n--- TERRITORIO #%d ---\nNome: %s\nCor (Dono): %s\nTropas: %d\n", 
               i + 1, (mapa + i)->nome, (mapa + i)->cor, (mapa + i)->tropas);
    }
}

/**
 * Le uma string do console e limpa o caractere de newline.
 */
void lerString(char *buffer, int tamanho, const char *prompt) {
    printf("%s", prompt);
    fgets(buffer, tamanho, stdin);
    buffer[strcspn(buffer, "\n")] = 0; 
}
