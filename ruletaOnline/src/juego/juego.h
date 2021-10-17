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

int comparaFrase(struct Juego juego, char *fraseResuelta)
{
    if (fraseResuelta[strlen(fraseResuelta) - 1] == '\n')
        fraseResuelta[strlen(fraseResuelta) - 1] == '\0';

    if (strcmp(juego.frase, fraseResuelta) == 0)
        return 1;

    return 0;
}

int fraseResuelta(struct Juego juego)
{
    if (strcmp(juego.frase, juego.fraseCifrada) == 0)
        return 1;
    return 0;
}

int apareceLetra(char letra, struct Juego juego)
{
    if (strchr(juego.frase, letra))
        return 0;
    return -1;
}

int numeroOcurrenciasLetra(char letra, struct Juego juego)
{
    int count = 0;
    for (int i = 0; juego.frase[i] != '\0'; ++i)
    {
        if (juego.frase[i] == letra)
            ++count;
    }

    return count;
}

int letraEscogida(char letra, struct Juego juego)
{
    if (strchr(juego.fraseCifrada, letra))
        return 0;
    return -1;
}

int descifrarLetra(char letra, struct Juego *juego, int jugador)
{
    if (apareceLetra(letra, *juego) != 0)
    {

        // La letra no aparece
        return 0;
    }
    if (letraEscogida(letra, *juego) == 0)
    {
        // La letra ya fue escogida
        return 1;
    }

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
