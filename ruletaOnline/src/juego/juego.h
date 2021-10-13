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
    juego.puntosPlayer1=0;
    juego.puntosPlayer2=0;
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

int descifrarLetra(char letra, struct Juego *juego,int jugador)
{
    if(apareceLetra(letra,*juego)!= 0){
        printf("La letra no aparece\n");
    }
    if(letraEscogida(letra,*juego) == 0){
        printf("La letra ya fue escogida\n");
    }
    int puntuacion=0;
    int contador = 0;
    if(letra == 'a'| letra == 'e' || letra == 'i' || letra == 'o' || letra == 'u'){
        puntuacion=-50;}
    if(jugador == 1){
        if(juego->puntosPlayer1 <= 0){
            printf("No tienes puntos para averiguar vocal\n");
        return 0;
        }
    }
    if(jugador == 2){
        if(juego->puntosPlayer2 <= 0){
            printf("No tienes puntos para averiguar vocal\n");
        return 0;
        }
    }
    else{
        puntuacion=50;
    }
    for (int i = 0; i < strlen((*juego).fraseCifrada); i++)
    {
        if ((*juego).frase[i] == letra)
        {
            (*juego).fraseCifrada[i] = letra;
            contador++;
        }
    }
    if(puntuacion>0){

    
    if(jugador == 1){
        juego->puntosPlayer1+=puntuacion*contador;
    }
    if(jugador == 2){
        juego->puntosPlayer2+=puntuacion*contador;
    }
    else{
        printf("Error, solo hay dos jugadores\n");
    }
    }
    return contador;
}
#endif
