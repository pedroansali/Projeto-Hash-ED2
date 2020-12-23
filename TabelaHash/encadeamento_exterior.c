#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "encadeamento_exterior.h"
#include "compartimento_hash.h"
#include "cliente.h"


int hash (int cod){
    return cod % 7;
}

int total_registros_clientes(char *nome_arquivo_dados){
    FILE *arq;
    arq = fopen(nome_arquivo_dados, "rb");
    fseek(arq, 0, SEEK_END);
    int tam = trunc(ftell(arq)/tamanho_cliente());
    fclose(arq);
    return tam;
}


void cria_hash(char *nome_arquivo_hash, int tam)
{
	FILE *arq;
	CompartimentoHash *Compartimento = (CompartimentoHash*) malloc(sizeof(CompartimentoHash));
	if((arq=fopen(nome_arquivo_hash,"wb"))==NULL){
        printf("Erro ao abrir arquivo!!");
    }
    else
    {
        for(int i=0; i<tam; i++)
        {
            Compartimento = compartimento_hash(-1);
            salva_compartimento(Compartimento, arq);
        }
    }
    fclose(arq);
}

int busca(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
	FILE *arq, *arq2;
    int h = hash(cod_cli);
    int aux;
    arq = fopen(nome_arquivo_hash,"rb");
    arq2= fopen(nome_arquivo_dados,"rb");

    if(arq == NULL || arq2==NULL)
    {
        printf("Erro ao abrir arquivos!!");
        exit(1);
    }

    int pos=-1;
    fseek(arq, h * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, arq);


    if(pos == -1)
    {
        return pos;
    }

    Cliente *cliente;
    int clienteOld = 0;
    do
    {
        aux = pos;
        fseek(arq2, pos * tamanho_cliente(), SEEK_SET);
        cliente = le_cliente(arq2);
        pos = cliente->prox;
        if(cliente->cod_cliente == cod_cli)
        {
            if(cliente->status == LIBERADO)
            {
                clienteOld = 1;
            }
            else
            {
                return aux;
            }
        }
    } while(cliente->prox != -1);
    if(clienteOld)
    {
        return -1;
    }

    fclose(arq);
    fclose(arq2);
    return -1;
}


int insere(int cod_cli, char *nome_cli, char *nome_arquivo_hash, char *nome_arquivo_dados, int num_registros)
{
    FILE *arq, *arq2;
    int h = hash(cod_cli);
    int aux;
    arq = fopen(nome_arquivo_hash,"rb+");
    arq2= fopen(nome_arquivo_dados,"rb+");

    if(arq == NULL || arq2==NULL)
    {
        printf("Erro ao abrir arquivos!!");
        exit(1);
    }

    Cliente *c = cliente(cod_cli, nome_cli, -1, OCUPADO);
    fseek(arq2, sizeof(Cliente), SEEK_CUR);
    salva_cliente(c, arq2);

    fclose(arq2);

    int tam = total_registros_clientes(nome_arquivo_dados);
    int pos=-1;
    pos = tam-1;
    fclose(arq);

    arq = fopen(nome_arquivo_hash,"rb+");
    fseek(arq, h*sizeof(int), SEEK_SET);
    fwrite(&pos, sizeof(int), 1, arq);
    fclose(arq);


    fclose(arq);
    fclose(arq2);
    aux = busca(cod_cli, nome_arquivo_hash, nome_arquivo_dados);


    return aux;

}

int exclui(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
    FILE *arq, *arq2;
    arq = fopen(nome_arquivo_hash,"rb+");
    arq2= fopen(nome_arquivo_dados,"rb+");

    if(arq == NULL || arq2==NULL)
    {
        printf("Erro ao abrir arquivos!!");
        exit(1);
    }
    int achou = busca(cod_cli,nome_arquivo_hash,nome_arquivo_dados);
    if(achou==-1)
    {
        return -1;
    }
    else
    {
        fseek(arq2, achou*sizeof(Cliente), SEEK_SET);
        Cliente *c = le_cliente(arq2);
        c->status = LIBERADO;
        return achou;
    }

}
