#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

struct nodo
{
    // Encolo los sockets mediante una estructura FIFO
    int sd;
    struct nodo *sig;
};

struct nodo *raiz = NULL;
struct nodo *fondo = NULL;

void insertar(int x)
{
    struct nodo *nuevo;
    nuevo = (struct nodo*)malloc(sizeof(struct nodo));
    nuevo->sd = x;
    nuevo->sig = NULL;
    if (raiz == NULL)
    {
        raiz = nuevo;
        fondo = nuevo;
    }
    else
    {
        fondo->sig = nuevo;
        fondo = nuevo;
    }
}

int extraer()
{
    if (raiz != NULL)
    {
        int informacion = raiz->sd;
        struct nodo *bor = raiz;
        if (raiz == fondo)
        {
            raiz = NULL;
            fondo = NULL;
        }
        else
        {
            raiz = raiz->sig;
        }
        free(bor);
        return informacion;
    }
    else
        return -1;
}

void imprimir()
{
    struct nodo *reco = raiz;
    printf("Listado de todos los elementos de la cola.\n");
    while (reco != NULL)
    {
        printf("%i - ", reco->sd);
        reco = reco->sig;
    }
    printf("\n");
}

void liberar()
{
    struct nodo *reco = raiz;
    struct nodo *bor;
    while (reco != NULL)
    {
        bor = reco;
        reco = reco->sig;
        free(bor);
    }
}

#endif