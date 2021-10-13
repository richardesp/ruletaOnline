#ifndef JUEGO_H
#define JUEGO_H
#include <string.h>
#include <ctype.h>
#include <stdio.h>

struct Juego
{
    char frase[100];
    char fraseCifrada[100];
    int puntosPlayer1;
    int puntosPlayer2;
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
    juego.puntosPlayer1 = 0;
    juego.puntosPlayer2 = 0;
    return juego;
}

int comparaFrase(struct Juego juego, char *fraseResuelta) {
    if(fraseResuelta[strlen(fraseResuelta) - 1] == '\n')
        fraseResuelta[strlen(fraseResuelta) - 1] == '\0';
    
    if(strcmp(juego.frase, fraseResuelta) == 0)
        return 1;
    
    return 0;
}

int fraseResuelta(struct Juego juego) {
    if(strcmp(juego.frase, juego.fraseCifrada) == 0)
        return 1;
    return 0;
}

int apareceLetra(char letra, struct Juego juego)
{
    if (strchr(juego.frase, letra))
        return 0;
    return -1;
}

int numeroOcurrenciasLetra(char letra, struct Juego juego) {
    int count = 0;
    for(int i = 0; juego.frase[i] != '\0'; ++i) {
        if(juego.frase[i] == letra)
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
    int puntuacion = 0;
    int contador = 0;
    if (letra == 'a' | letra == 'e' || letra == 'i' || letra == 'o' || letra == 'u')
    {
        puntuacion = -50;
    }
    if (jugador == 1)
    {
        if (juego->puntosPlayer1 <= 0)
        {
            // No hay puntos para averigurar la vocal
            return 2;
        }
    }
    if (jugador == 2)
    {
        if (juego->puntosPlayer2 <= 0)
        {
            // No hay puntos para averiguar la vocal
            return 2;
        }
    }
    else
    {
        puntuacion = 50;
    }
    for (int i = 0; i < strlen((*juego).fraseCifrada); i++)
    {
        if ((*juego).frase[i] == letra)
        {
            (*juego).fraseCifrada[i] = letra;
            contador++;
        }
    }
    if (puntuacion > 0)
    {

        if (jugador == 1)
        {
            juego->puntosPlayer1 += puntuacion * contador;
        }
        if (jugador == 2)
        {
            juego->puntosPlayer2 += puntuacion * contador;
        }
        
    }
    return contador;
}
#endif


