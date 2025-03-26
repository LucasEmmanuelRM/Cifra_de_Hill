#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#define N 2 // Tamanho do bloco e da matriz de codificação
#define MOD 26 // Tamanho do alfabeto inglês

// Protótipos de funções
int determinante(int matriz[N][N], int n);
int inverso_modular(int num);
void matriz_cofatora(int matriz[N][N], int cofator[N][N], int n);
void inverter_matriz(int matriz[N][N], int inversa[N][N]);
void criar_matriz_chave(int chave[N][N]);
void multiplicar_matriz(const int matriz[N][N], int vetor[N]);
void ajustar_texto(char *text, char *texto_ajustado);
void cifra_hill(const char *entrada, const char *saida, const int chave[N][N]);


///////////////////////////////// Instruções ///////////////////////////////////
//                                                                            //
// Compilar: gcc CifraHill.c -o CifraHill                                     //
// Executar cifração: ./CifraHill -enc textoclaro.txt -out textocifrado.txt   //
// Executar decifração: ./CifraHill -dec textocifrado.txt -out textoclaro.txt //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


// Função principal que requisita os arquivos de entrada e saída, assim como a opção de cifrar ou decifrar o arquivo de entrada
// É necessário ter o arquivo de entrada com o texto a ser cifrado ou decifrado
// O arquivo de saída é criado automaticamente
int main(int argc, char *argv[]) {
    setlocale(LC_ALL, ""); // Suporte a caracteres especiais
    
    if (argc != 5) {
        printf("Uso: %s -enc|-dec <input.txt> -out <output.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *entrada = argv[2];
    const char *saida = argv[4];
    
    int matriz_chave[N][N], matriz_chave_inversa[N][N];
    criar_matriz_chave(matriz_chave);
    
    if (strcmp(argv[1], "-enc") == 0) {
        cifra_hill(entrada, saida, matriz_chave);
    } else if (strcmp(argv[1], "-dec") == 0) {
        inverter_matriz(matriz_chave, matriz_chave_inversa);
        cifra_hill(entrada, saida, matriz_chave_inversa);
    } else {
        printf("Opção inválida. Use -enc para cifrar ou -dec para decifrar.\n");
        return EXIT_FAILURE;
    }
    
    printf("Operação concluída com sucesso!\n");
    return EXIT_SUCCESS;
}


// Função que recebe um arquivo de entrada, o lê, e o cifra/decifra, escrevendo o resultado num arquivo de saída
// Ele recebe a matriz chave para cifrar ou a inversa dela para decifrar
void cifra_hill(const char *entrada, const char *saida, const int chave[N][N]) {
    FILE *f_in = fopen(entrada, "r");
    FILE *f_out = fopen(saida, "w");
    if (!f_in || !f_out) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }
    
    // O arquivo de entrada é lido para que o seu texto seja utilizado pelo programa
    char buffer[1024], texto_ajustado[1024];
    fgets(buffer, sizeof(buffer), f_in);
    fclose(f_in);
    
    // O texto lido é verificado para garantir que seus caracteres estão dentro do alfabeto
    ajustar_texto(buffer, texto_ajustado);
    
    // O texto é convertido blocos de números com tamanho N durante o processo para realizar as operações sobre matrizes
    int len = strlen(texto_ajustado);
    for (int i = 0; i < len; i += N) {
        int vetor[N];
        for (int j = 0; j < N; j++) {
            vetor[j] = texto_ajustado[i + j] - 'A';
        }

        // Multiplicação da chave (ou sua inversa) pelos blocos de caracteres
        multiplicar_matriz(chave, vetor);

        // O texto retorna a caracteres no final para ser colocado no arquivo de saída, bloco por bloo
        for (int j = 0; j < N; j++) {
            fputc(vetor[j] + 'A', f_out);
        }
    }
    fclose(f_out);
}


// Função para garantir que o texto cifrado ou claro esteja dentro do alfabeto
// Caso não seja perfeitamente dividido em N partes, as faltas são preenchidas com "X"
// Letras em minúsculo são transformadas em maiúsculo
void ajustar_texto(char *text, char *texto_ajustado) {
    int len = strlen(text);
    int i, j = 0;
    for (i = 0; i < len; i++) {
        if (isalpha(text[i])) {
            texto_ajustado[j++] = toupper((unsigned char)text[i]);
        }
    }
    while (j % N != 0) {
        texto_ajustado[j++] = 'X'; // Padding com 'X'
    }
    texto_ajustado[j] = '\0';
}


// Função para converter uma chave em texto para uma matriz numérica
// A chave passa por uma correção onde qualquer caractere fora do alfabeto é ignorado
// Se ela não possuir tamanho NxN, recebe preenchimento em X
void criar_matriz_chave(int chave[N][N]) {
    char key[100];
    printf("Digite a chave: ");
    fgets(key, sizeof(key), stdin);
    
    int len = strlen(key);
    int index = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            while (index < len && !isalpha(key[index])) {
                index++;
            }
            if (index < len) {
                chave[i][j] = toupper((unsigned char)key[index]) - 'A';
                index++;
            } else {
                chave[i][j] = 'X' - 'A'; // Preenchimento com 'X'
            }
        }
    }
}


// Função para calcular o determinante de uma matriz NxN de forma recursiva
// Utiliza-se Expensão de Laplace, método comumente visto no ensino médio
int determinante(int matriz[N][N], int n) {
    if (n == 1) return matriz[0][0];
    if (n == 2) return (matriz[0][0] * matriz[1][1] - matriz[0][1] * matriz[1][0]) % MOD;
    
    int det = 0;
    int temp[N][N];
    int sinal = 1;
    
    for (int f = 0; f < n; f++) {
        int sub_i = 0;
        for (int i = 1; i < n; i++) {
            int sub_j = 0;
            for (int j = 0; j < n; j++) {
                if (j == f) continue;
                temp[sub_i][sub_j] = matriz[i][j];
                sub_j++;
            }
            sub_i++;
        }
        det = (det + sinal * matriz[0][f] * determinante(temp, n - 1)) % MOD;
        sinal = -sinal;
    }
    return (det + MOD) % MOD;
}


// Função para calcular a matriz de cofatores para o processo de inversão utilizando matriz adjunta
void matriz_cofatora(int matriz[N][N], int cofator[N][N], int n) {
    int temp[N][N];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sub_i = 0;
            for (int x = 0; x < n; x++) {
                if (x == i) continue;
                int sub_j = 0;
                for (int y = 0; y < n; y++) {
                    if (y == j) continue;
                    temp[sub_i][sub_j] = matriz[x][y];
                    sub_j++;
                }
                sub_i++;
            }
            cofator[i][j] = ((i + j) % 2 == 0 ? 1 : -1) * determinante(temp, n - 1);
        }
    }
}


// Função para calcular o inverso modular de um número módulo 26
// Seu papel é descobrir um x que, se multiplicado pelo determinante, obtém-se ≡ 1 mod 26
int inverso_modular(int num) {
    num = num % MOD;
    for (int x = 1; x < MOD; x++) {
        if ((num * x) % MOD == 1)
            return x;
    }
    return -1; // Se não existir inverso modular
}


// Função para calcular a inversa modular de uma matriz NxN
void inverter_matriz(int matriz[N][N], int inversa[N][N]) {
    int det = determinante(matriz, N);
    if (det < 0) det += MOD;
    
    int det_inv = inverso_modular(det);
    if (det_inv == -1) {
        printf("Erro: A matriz de chave não tem inversa módulo %d.\n", MOD);
        exit(EXIT_FAILURE);
    }
    
    int cofator[N][N];
    matriz_cofatora(matriz, cofator, N);
    
    // Transpõe a matriz de cofatores e multiplica pelo inverso do determinante módulo 26
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            inversa[j][i] = (cofator[i][j] * det_inv) % MOD;
            if (inversa[j][i] < 0) inversa[j][i] += MOD;
        }
    }
}


// Função que multiplica a chave, em forma de matriz, pelo texto, em forma de vetor
// O texto é dividido em blocos de tamanho N
void multiplicar_matriz(const int matriz[N][N], int vetor[N]) {
    int result[N] = {0};
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            result[i] += matriz[i][j] * vetor[j];
        }
        result[i] %= MOD;
    }
    for (int i = 0; i < N; i++) {
        vetor[i] = result[i];
    }
}