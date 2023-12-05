#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef unsigned char byte;

/** Definição da árvore */
typedef struct noArvore{
    int frequencia;
    byte c;
    struct noArvore   *esquerda;
    struct noArvore   *direita;
} noArvore;

typedef struct noLista{
    noArvore          *n;
    struct noLista    *proximo;
} noLista;

typedef struct lista{
    noLista   *head;
    int         elementos;
} lista;
/*----------------------------------------------*/
/*----------------------------------------------*/
void CompressFile(const char *arquivoEntrada, const char *arquivoSaida);
void DecompressFile(const char *arquivoEntrada, const char *arquivoSaida);
noArvore *BuildHuffmanTree(unsigned *listaBytes);
noArvore *novonoArvore(byte c, int frequencia, noArvore *esquerda, noArvore *direita);
void FreeHuffmanTree(noArvore *n);
noLista *novonoLista(noArvore *nArv);
noArvore *popMinLista(lista *l);
void pegaByteFrequencia(FILE *entrada, unsigned int *listaBytes);
bool pegaCodigo(noArvore *n, byte c, char *buffer, int tamanho);
void insereLista(noLista *n, lista *l);
int geraBit(FILE *entrada, int posicao, byte *aux );
void erroArquivo();
/*----------------------------------------------*/
/*----------------------------------------------*/
int main(int argc, char *argv[]){
    if (argc < 4){
        printf("Uso: huffman [OPERAÇAO] [ARQUIVO] [ARQUIVO]\n\n");
        printf("Operações:\n");
        printf("\t-c\tCompacta\n");
        printf("\t-d\tDescompacta\n");
        printf("\nExemplo: ./huffman -c comprima.isso nisso.anao\n");
        printf("\nO arquivo comprimido deve ter a extenção .anao\n");
        return 0;
    }
    if (strcmp("-c", argv[1]) == 0){
        if (strstr(argv[3], ".anao")){
            CompressFile(argv[2], argv[3]);
        }else{
            printf("O arquivo comprimido deve ter a extensao '.anao'.\n");
            printf("Exemplo: \n\t./huffman -c comprima.isso nisso.anao\n");
            return 0;
        }
    }else if (strcmp("-d", argv[1]) == 0){
        if (strstr(argv[2], ".anao"))
           DecompressFile(argv[2], argv[3]);
        else{
            printf("O arquivo a ser descomprimido deve ter a extensao '.anao'.\n");
            printf("Exemplo: \n\t./huffman -d descomprime.anao nisso.extensao\n");
            return 0;
        }
    }else{
        printf("Uso: huffman [OPERAÇAO] [ARQUIVO] [ARQUIVO]\n\n");
        printf("Operações:\n");
        printf("\t-c\tCompacta\n");
        printf("\t-d\tDescompacta\n");
        printf("\nExemplo: ./huffman -c comprima.isso nisso.anao\n");
        return 0;
    }
    return 0;
}
/*----------------------------------------------*/
/*----------------------------------------------*/
void CompressFile(const char *arquivoEntrada, const char *arquivoSaida){
    unsigned listaBytes[256] = {0};
    FILE *entrada = fopen(arquivoEntrada, "rb");
    (!entrada) ? erroArquivo() : NULL == NULL ;
    FILE *saida = fopen(arquivoSaida, "wb");
    (!saida) ? erroArquivo() : NULL == NULL ;
    pegaByteFrequencia(entrada, listaBytes);
    noArvore *raiz = BuildHuffmanTree(listaBytes);
    fwrite(listaBytes, 256, sizeof(listaBytes[0]), saida);
    fseek(saida, sizeof(unsigned int), SEEK_CUR);
    byte c;
    unsigned tamanho = 0;
    byte aux = 0;
    while (fread(&c, 1, 1, entrada) >= 1){
        char buffer[1024] = {0};
        pegaCodigo(raiz, c, buffer, 0);
        for (char *i = buffer; *i; i++){
            if (*i == '1')
                aux = aux | (1 << (tamanho % 8));
            tamanho++;
            if (tamanho % 8 == 0){
                fwrite(&aux, 1, 1, saida);
                aux = 0;
            }
        }
    }
    //
    printf("\n");
    system("echo \"\\e[1;32mArquivo COMPACTADO!!!\\e[0m\"");
    printf("\n");
    system("echo \"\\e[1;33m\"");
    printf("Matheus Amorim\nHuffman (Atividade Avaliativa) 7.0.1\nFinalmente funcionou!\n\n");
    system("echo \"\\e[0m\"");
    //
    fclose(entrada);
    fclose(saida);
}
/*----------------------------------------------*/
/*----------------------------------------------*/
void erroArquivo(){
    printf("Arquivo nao encontrado\n");
    exit(0);
}
/*----------------------------------------------*/
/*----------------------------------------------*/
void pegaByteFrequencia(FILE *entrada, unsigned int *listaBytes){
    byte c;
    while (fread(&c, 1, 1, entrada) >= 1){
        listaBytes[(byte)c]++;
    }
    rewind(entrada);
}
/*----------------------------------------------*/
/*----------------------------------------------*/
noArvore *BuildHuffmanTree(unsigned *listaBytes){
    lista l = {NULL, 0};
    for (int i = 0; i < 256; i++){
        if (listaBytes[i])
            insereLista(novonoLista(novonoArvore(i, listaBytes[i], NULL, NULL)), &l);
    }
    while (l.elementos > 1){
        noArvore *nodeEsquerdo = popMinLista(&l);
        noArvore *nodeDireito = popMinLista(&l);
        noArvore *soma = novonoArvore('#', nodeEsquerdo->frequencia + nodeDireito->frequencia, nodeEsquerdo, nodeDireito);
        insereLista(novonoLista(soma), &l);
    }
    return popMinLista(&l);
}
/*----------------------------------------------*/
/*----------------------------------------------*/
bool pegaCodigo(noArvore *n, byte c, char *buffer, int tamanho){
    if (!(n->esquerda || n->direita) && n->c == c){
        buffer[tamanho] = '\0';
        return true;
    }else{
        bool encontrado = false;
        if (n->esquerda){
            buffer[tamanho] = '0';
            encontrado = pegaCodigo(n->esquerda, c, buffer, tamanho + 1);
        }
        if (!encontrado && n->direita){
            buffer[tamanho] = '1';
            encontrado = pegaCodigo(n->direita, c, buffer, tamanho + 1);
        }
        if (!encontrado)
            buffer[tamanho] = '\0';
        return encontrado;
    }
}
/*----------------------------------------------*/
/*----------------------------------------------*/
void FreeHuffmanTree(noArvore *n){
    if (!n) 
        return;
    else{
        noArvore *esquerda = n->esquerda;
        noArvore *direita = n->direita;
        free(n);
        FreeHuffmanTree(esquerda);
        FreeHuffmanTree(direita);
    }
}
/*----------------------------------------------*/
/*----------------------------------------------*/
void insereLista(noLista *n, lista *l){
    if (!l->head)
        l->head = n;
    else if (n->n->frequencia < l->head->n->frequencia){
        n->proximo = l->head;
        l->head = n;
    }else{
        noLista *aux = l->head->proximo;
        noLista *aux2 = l->head;
        while (aux && aux->n->frequencia <= n->n->frequencia){
            aux2 = aux;
            aux = aux2->proximo;
        }
        aux2->proximo = n;
        n->proximo = aux;
    }
    l->elementos++;
}
/*----------------------------------------------*/
/*----------------------------------------------*/
noLista *novonoLista(noArvore *nArv){
    noLista *novo;
    if ( (novo = malloc(sizeof(*novo))) == NULL ) return NULL;
    novo->n = nArv;
    novo->proximo = NULL;
    return novo;
}
/*----------------------------------------------*/
/*----------------------------------------------*/
noArvore *novonoArvore(byte c, int frequencia, noArvore *esquerda, noArvore *direita){
    noArvore *novo;
    if ( ( novo = malloc(sizeof(*novo)) ) == NULL ) return NULL;
    novo->c = c;
    novo->frequencia = frequencia;
    novo->esquerda = esquerda;
    novo->direita = direita;
    return novo;
}
/*----------------------------------------------*/
/*----------------------------------------------*/
noArvore *popMinLista(lista *l){
    noLista *aux = l->head;
    noArvore *aux2 = aux->n;
    l->head = aux->proximo;
    free(aux);
    aux = NULL;
    l->elementos--;
    return aux2;
}
/*----------------------------------------------*/
/*----------------------------------------------*/
void DecompressFile(const char *arquivoEntrada, const char *arquivoSaida){
    unsigned listaBytes[256] = {0};
    FILE *entrada = fopen(arquivoEntrada, "rb");
    (!entrada) ? erroArquivo() : NULL == NULL ;
    FILE *saida = fopen(arquivoSaida, "wb");
    (!saida) ? erroArquivo() : NULL == NULL ;
    fread(listaBytes, 256, sizeof(listaBytes[0]), entrada);
    noArvore *raiz = BuildHuffmanTree(listaBytes);
    unsigned tamanho;
    fread(&tamanho, 1, sizeof(tamanho), entrada);
    unsigned posicao = 0;
    byte aux = 0;
    while (posicao < tamanho){
        noArvore *nodeAtual = raiz;
        while ( nodeAtual->esquerda || nodeAtual->direita ){
            nodeAtual = geraBit(entrada, posicao++, &aux) ? nodeAtual->direita : nodeAtual->esquerda;
        }
        fwrite(&(nodeAtual->c), 1, 1, saida);
    }
    FreeHuffmanTree(raiz);
    //
    printf("\n");
    system("echo \"\\e[1;32mArquivo DESCOMPACTADO!!!\\e[0m\"");
    printf("\n");
    system("echo \"\\e[1;35m\"");
    printf("Matheus Amorim\nHuffman (Atividade Avaliativa) 7.0.1\nFinalmente funcionou!\n\n");
    system("echo \"\\e[0m\"");
    //
    fclose(saida);
    fclose(entrada);
}
/*----------------------------------------------*/
/*----------------------------------------------*/
int geraBit(FILE *entrada, int posicao, byte *aux ){
    (posicao % 8 == 0) ? fread(aux, 1, 1, entrada) : NULL == NULL ;
    return !!((*aux) & (1 << (posicao % 8)));
}
