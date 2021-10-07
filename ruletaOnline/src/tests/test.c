#include "juego.h"
#include <string.h>
#include <stdlib.h>

int main()
{
    char frase[100] = "mas vale pajaro en mano que ciento volando";
    printf("%s\n", frase);
    struct Juego juego = crearCifrado(frase);
    printf("%s", juego.fraseCifrada);
    char a;
    while (strcmp(juego.frase, juego.fraseCifrada) != 0)
    {
        printf("\nIntroduce un caracter\n");
        scanf("%c", &a);
        if (apareceLetra(a, juego) == 0 && letraEscogida(a, juego) == -1)
        {
            int ocurrencias;
            descifrarLetra(a, &juego);
            printf("letra %c se encuentra en la frase %d veces\n", a, ocurrencias);
            printf("Nueva frase: %s\n", juego.fraseCifrada);
        }
        else
        {
            printf("el caracter no se encuentra en la frase o ya ha sido incluido\n");
        }
    }
    exit(1);
}