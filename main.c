#include <stdio.h> // Entrada e saída padrão
#include <stdlib.h> // Funções utilitárias
#include <string.h> // Manipulação de strings
#include <ctype.h> // Manipulação de caracteres
#include <time.h> // Manipulação de datas e horários
#include <pthread.h> // Manipulação de semáforos

#define MAX_PALAVRA 20    // tamanho máximo das palavras a serem utilizadas no jogo
#define MAX_TENTATIVAS 10 // número máximo de tentativas para o jogador adivinhar a palavra

typedef struct {
    char palavra[MAX_PALAVRA];            // representa a palavra secreta  
    char letras_adivinhadas[MAX_PALAVRA]; // mostra as letras adivinhadas pelo jogador até o momento
    int tentativas_restantes;             // quantas tentativas o jogador ainda tem
    pthread_mutex_t lock;                 // Semáforo (mutex) para controlar o acesso à palavra em ambientes concorrentes
} Jogo;

void inicializar_jogo(Jogo *jogo, const char *palavras[], int num_palavras);
void escolher_palavra_aleatoria(const char *palavras[], int num_palavras, char *palavra_escolhida);
int letra_adivinhada(char letra, const char *palavra, char *letras_adivinhadas);
void mostrar_progresso(const char *letras_adivinhadas, int tentativas_restantes);
int verificar_vitoria(const char *letras_adivinhadas);
void limpar_buffer();

int main() {
    const char *palavras[] = {"BALEIA", "SAPO", "GALINHA", "RATO", "PATO", "URSO", "MACACO", "GATO", "ELEFANTE"};
    int num_palavras = sizeof(palavras) / sizeof(palavras[0]); // Dividir o tamanho total da matriz pelo tamanho de um unico elemento

    srand(time(NULL)); // inicializa o gerador de numeros aleatorios

    Jogo jogo1, jogo2; // dois jogos para dois jogadores
    inicializar_jogo(&jogo1, palavras, num_palavras);
    inicializar_jogo(&jogo2, palavras, num_palavras);

    char letra;
    // Enquanto um jogador não ganhar ou ambos perderem, o jogo continua para cada um deles.
    while (jogo1.tentativas_restantes > 0 && !verificar_vitoria(jogo1.letras_adivinhadas) && jogo2.tentativas_restantes > 0 && !verificar_vitoria(jogo2.letras_adivinhadas)) {
        // Jogador 1
        pthread_mutex_lock(&jogo1.lock); // Bloquear acesso ao jogo1
        printf("\nJogador 1:\n");
        printf("Tentativas restantes: %d\n", jogo1.tentativas_restantes);
        mostrar_progresso(jogo1.letras_adivinhadas, jogo1.tentativas_restantes);
        printf("Digite uma letra: ");
        scanf(" %c", &letra);
        letra = toupper(letra); // Converter para maiuscula
        if (!isalpha(letra)) { // Validar que apenas letras são inseridas
            printf("Por favor, digite apenas letras.\n");
            limpar_buffer();
            pthread_mutex_unlock(&jogo1.lock); // Liberar acesso ao jogo1
            continue;
        }
        if (!letra_adivinhada(letra, jogo1.palavra, jogo1.letras_adivinhadas)) { // Se a letra inserida não estiver dentro da palavra
            printf("Letra incorreta!\n");
            jogo1.tentativas_restantes--; // Decresce a quantidade de tentativas restantes
        }
        pthread_mutex_unlock(&jogo1.lock); // Liberar acesso ao jogo1

        // Jogador 2
        pthread_mutex_lock(&jogo2.lock); // Bloquear acesso ao jogo2
        printf("\nJogador 2:\n");
        printf("Tentativas restantes: %d\n", jogo2.tentativas_restantes);
        mostrar_progresso(jogo2.letras_adivinhadas, jogo2.tentativas_restantes);
        printf("Digite uma letra: ");
        scanf(" %c", &letra);
        letra = toupper(letra); // Converter para maiuscula
        if (!isalpha(letra)) {
            printf("Por favor, digite apenas letras.\n");
            limpar_buffer();
            pthread_mutex_unlock(&jogo2.lock); // Liberar acesso ao jogo2
            continue;
        }
        if (!letra_adivinhada(letra, jogo2.palavra, jogo2.letras_adivinhadas)) {
            printf("Letra incorreta!\n");
            jogo2.tentativas_restantes--;
        }
        pthread_mutex_unlock(&jogo2.lock); // Liberar acesso ao jogo2
    }

    if (verificar_vitoria(jogo1.letras_adivinhadas)) {        // Se as letras adivinhadas pelo jogador 1 coincidirem com a palavra selecionada
        printf("\nJogador 1 venceu! A palavra era: %s\n", jogo1.palavra);
    } else if (verificar_vitoria(jogo2.letras_adivinhadas)) { // Se as letras adivinhadas pelo jogador 2 coincidirem com a palavra selecionada
        printf("\nJogador 2 venceu! A palavra era: %s\n", jogo2.palavra);
    } else {                                                  // Se nenhum dos dois tiver acertado
        printf("\nNenhum jogador venceu! A palavra era: %s\n", jogo1.palavra);
    }


    return 0;
}

void inicializar_jogo(Jogo *jogo, const char *palavras[], int num_palavras) {
    escolher_palavra_aleatoria(palavras, num_palavras, jogo->palavra); // escolhe uma palavra aleatória da lista de palavras disponíveis
    memset(jogo->letras_adivinhadas, '_', strlen(jogo->palavra)); // preenche a listra de letras adivinhadas com underlines
    jogo->letras_adivinhadas[strlen(jogo->palavra)] = '\0';  // garante que a string 'letras_adivinhadas' termine com o caractere nulo, mostrando onde ela termina
    jogo->tentativas_restantes = MAX_TENTATIVAS; // inicializa o numero de tentativas restantes
    pthread_mutex_init(&jogo->lock, NULL); // Inicializar o mutex para controle de acesso à palavra
}

// Escolhe aleatoriamente uma palavra da lista de palavras disponíveis
void escolher_palavra_aleatoria(const char *palavras[], int num_palavras, char *palavra_escolhida) {
    int indice = rand() % num_palavras;
    strcpy(palavra_escolhida, palavras[indice]);
}

// Verifica se a letra está presente na palavra
int letra_adivinhada(char letra, const char *palavra, char *letras_adivinhadas) {
    int acerto = 0; // Inicialmente, assume que a letra inserida não foi encontrada na palavra
    for (int i = 0; palavra[i] != '\0'; i++) { // Enquanto não encontrar o nulo, ou o final da string
        if (palavra[i] == letra) { // Verifica se a letra inserida está presente na palavra secreta
            letras_adivinhadas[i] = letra; // Se a letra estiver presente, atualiza a lista de letras adivinhadas para revelar a letra correta
            acerto = 1;
        }
    }
    return acerto;
}

// Exibir o progresso atual do jogador no jogo (palavra com letras adivinhadas, representação da forca e tentativas restantes)
void mostrar_progresso(const char *letras_adivinhadas, int tentativas_restantes) {
    printf("Palavra: %s\n", letras_adivinhadas); // Exibe a palavra atual com as letras adivinhadas reveladas
    printf("   +---+\n"); // Forca
    printf("   |   |\n"); // Corpo da forca
    printf("   |   %c\n", tentativas_restantes < MAX_TENTATIVAS ? 'O' : ' '); // Desenha a cabeça se o nr de tentativas restantes for menor que o máximo. Senão, desenha apenas um espaço.
    printf("   |  %c%c%c\n", tentativas_restantes < MAX_TENTATIVAS - 1 ? '/' : ' ', tentativas_restantes < MAX_TENTATIVAS - 2 ? '|' : ' ', tentativas_restantes < MAX_TENTATIVAS - 3 ? '\\' : ' '); // Desenha os braços do enforcado, um de cada vez, dependendo do numero de tentativas restantes
    printf("   |  %c %c\n", tentativas_restantes < MAX_TENTATIVAS - 4 ? '/' : ' ', tentativas_restantes < MAX_TENTATIVAS - 5 ? '\\' : ' '); // Desenha as pernas do enforcado, uma de cada vez, dependendo do numero de tentativas restantes
    printf("  ===\n");
}

// Verificar se o jogador venceu o jogo
int verificar_vitoria(const char *letras_adivinhadas) {
    for (int i = 0; letras_adivinhadas[i] != '\0'; i++) { // Percorre toda a palavra
        if (letras_adivinhadas[i] == '_') { 
            return 0; // A palavra ainda nao foi completamente adivinhada
        }
    }
    return 1; // Todas as letras foram adivinhadas
}

// Limpar o buffer do teclado para descartar qualquer entrada adicional
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

