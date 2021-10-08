#ifndef JUEGO_H
#define JUEGO_H
#include <string.h>
#include <ctype.h>
#include <stdio.h>

struct Juego
{
    char frase[100];
    char fraseCifrada[100];
};

struct Juego crearCifrado(char frase[])
{
    char frasecifrada[100];
    strcpy(frasecifrada, frase);

    for (int i = 0; i < strlen(frasecifrada); i++)
    {
        if (isalpha(frasecifrada[i]))
        {
            frasecifrada[i] = '-';
        }
    }

    struct Juego juego;
    strcpy(juego.frase, frase);
    strcpy(juego.fraseCifrada, frasecifrada);
    return juego;
}

int apareceLetra(char letra, struct Juego juego)
{
    if (strchr(juego.frase, letra))
        return 0;
    return -1;
}

int letraEscogida(char letra, struct Juego juego)
{
    if (strchr(juego.fraseCifrada, letra))
        return 0;
    return -1;
}

int descifrarLetra(char letra, struct Juego *juego)
{
    int contador = 0;
    for (int i = 0; i < strlen((*juego).fraseCifrada); i++)
    {
        if ((*juego).frase[i] == letra)
        {
            (*juego).fraseCifrada[i] = letra;
            contador++;
        }
    }
    return contador;
}
#endif