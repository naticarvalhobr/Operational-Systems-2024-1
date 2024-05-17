#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_PALAVRA 20
#define MAX_TENTATIVAS 10

typedef struct {
    char palavra[MAX_PALAVRA];
    char letras_adivinhadas[MAX_PALAVRA];
    int tentativas_restantes;
} Jogo;

void inicializar_jogo(Jogo *jogo, const char *palavras[], int num_palavras);
void escolher_palavra_aleatoria(const char *palavras[], int num_palavras, char *palavra_escolhida);
int letra_adivinhada(char letra, const char *palavra, char *letras_adivinhadas);
void mostrar_progresso(const char *letras_adivinhadas, int tentativas_restantes);
int verificar_vitoria(const char *letras_adivinhadas);
void limpar_buffer();

int main() {
    const char *palavras[] = {"BALEIA", "SAPO", "GALINHA", "RATO", "PATO", "URSO", "MACACO", "GATO", "ELEFANTE"};
    int num_palavras = sizeof(palavras) / sizeof(palavras[0]);

    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios

    Jogo jogo;
    inicializar_jogo(&jogo, palavras, num_palavras);

    char letra;
    while (jogo.tentativas_restantes > 0 && !verificar_vitoria(jogo.letras_adivinhadas)) {
        printf("\nTentativas restantes: %d\n", jogo.tentativas_restantes);
        mostrar_progresso(jogo.letras_adivinhadas, jogo.tentativas_restantes);
        printf("Digite uma letra: ");
        scanf(" %c", &letra);
        letra = toupper(letra); // Converter para maiúscula

        if (!isalpha(letra)) {
            printf("Por favor, digite apenas letras.\n");
            limpar_buffer();
            continue;
        }

        if (!letra_adivinhada(letra, jogo.palavra, jogo.letras_adivinhadas)) {
            printf("Letra incorreta!\n");
            jogo.tentativas_restantes--;
        }
    }

    if (verificar_vitoria(jogo.letras_adivinhadas)) {
        printf("\nParabéns! Você venceu. A palavra era: %s\n", jogo.palavra);
    } else {
        printf("\nVocê perdeu! A palavra era: %s\n", jogo.palavra);
    }

    return 0;
}

void inicializar_jogo(Jogo *jogo, const char *palavras[], int num_palavras) {
    escolher_palavra_aleatoria(palavras, num_palavras, jogo->palavra);
    memset(jogo->letras_adivinhadas, '_', strlen(jogo->palavra));
    jogo->letras_adivinhadas[strlen(jogo->palavra)] = '\0';
    jogo->tentativas_restantes = MAX_TENTATIVAS;
}

void escolher_palavra_aleatoria(const char *palavras[], int num_palavras, char *palavra_escolhida) {
    int indice = rand() % num_palavras;
    strcpy(palavra_escolhida, palavras[indice]);
}

int letra_adivinhada(char letra, const char *palavra, char *letras_adivinhadas) {
    int acerto = 0;
    for (int i = 0; palavra[i] != '\0'; i++) {
        if (palavra[i] == letra) {
            letras_adivinhadas[i] = letra;
            acerto = 1;
        }
    }
    return acerto;
}

void mostrar_progresso(const char *letras_adivinhadas, int tentativas_restantes) {
    printf("Palavra: %s\n", letras_adivinhadas);
    printf("   +---+\n");
    printf("   |   |\n");
    printf("   |   %c\n", tentativas_restantes < MAX_TENTATIVAS ? 'O' : ' ');
    printf("   |  %c%c%c\n", tentativas_restantes < MAX_TENTATIVAS - 1 ? '/' : ' ', 
           tentativas_restantes < MAX_TENTATIVAS - 2 ? '|' : ' ', 
           tentativas_restantes < MAX_TENTATIVAS - 3 ? '\\' : ' ');
    printf("   |  %c %c\n", tentativas_restantes < MAX_TENTATIVAS - 4 ? '/' : ' ', 
           tentativas_restantes < MAX_TENTATIVAS - 5 ? '\\' : ' ');
    printf("  ===\n");
}

int verificar_vitoria(const char *letras_adivinhadas) {
    for (int i = 0; letras_adivinhadas[i] != '\0'; i++) {
        if (letras_adivinhadas[i] == '_') {
            return 0; // A palavra ainda não foi completamente adivinhada
        }
    }
    return 1; // Todas as letras foram adivinhadas
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
