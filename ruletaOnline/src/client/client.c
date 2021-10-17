#include "../juego/juego.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>

void imprimirAyuda(void);

int main(int argc, char **argv)
{
    system("clear");

    char ip[128];
    if (argc > 1)
        strcpy(ip, argv[1]);

    else
        strcpy(ip, "127.0.0.1");

    // Descriptor del socket y buffer de datos
    int sd;
    struct sockaddr_in sockname;
    char buffer[250];
    socklen_t len_sockname;
    fd_set readfds, auxfds;
    int salida;
    int fin = 0;

    // Abrimos el socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("No se puede abrir el socket cliente\n");
        exit(EXIT_FAILURE);
    }

    // Se rellenan los campos de la estructura con la IP del
    // servidor y el puerto del servicio que solicitamos
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(2050);
    sockname.sin_addr.s_addr = inet_addr(ip);

    // Solicitamos la conexión al servidor
    len_sockname = sizeof(sockname);

    if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
    {
        perror("Error de conexión");
        exit(EXIT_FAILURE);
    }

    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);

    FD_SET(0, &readfds);
    FD_SET(sd, &readfds);

    imprimirAyuda();

    // Transmitimos la información al servidor, solo me saldré con <SALIR>
    do
    {
        auxfds = readfds;
        salida = select(sd + 1, &auxfds, NULL, NULL, NULL);

        // Tengo mensaje desde el servidor
        if (FD_ISSET(sd, &auxfds))
        {

            bzero(buffer, sizeof(buffer));
            recv(sd, buffer, sizeof(buffer), 0);

            printf("\n%s\n", buffer);

            if (strcmp(buffer, "-Err. Demasiados clientes conectados\n") == 0)
                fin = 1;

            if (strcmp(buffer, "Desconexión servidor\n") == 0)
                fin = 1;

            if (strcmp(buffer, "+Ok. Desconexión del servidor\n") == 0)
                fin = 1;
        }
        else
        {

            //He introducido información por teclado
            if (FD_ISSET(0, &auxfds))
            {
                bzero(buffer, sizeof(buffer));

                fgets(buffer, sizeof(buffer), stdin);

                if (strcmp(buffer, "SALIR\n") == 0)
                {
                    fin = 1;
                }

                send(sd, buffer, sizeof(buffer), 0);
            }
        }

    } while (fin == 0);

    close(sd);

    return EXIT_SUCCESS;
}

void imprimirAyuda(void)
{
    fprintf(stdout, "Menú de opciones de la ruleta online: \n\n");
    fprintf(stdout, "Para introducir una ip -> ./client.exe <ip>. Por defecto se asigna LOCALHOST\n\n");
    fprintf(stdout, "USUARIO usuario: mensaje para introducir el usuario que desea. \n");
    fprintf(stdout, "PASSWORD contraseña: mensaje para introducir la contraseña asociada al usuario\n");
    fprintf(stdout, "REGISTRO  –u  usuario  –p  password:  mensaje  mediante  el  cual  el  usuario  solicita registrarse para acceder al juego de la ruleta que escucha en el puerto TCP  2050.\n");
    fprintf(stdout, "INICIAR-PARTIDA: mensaje para solicitar jugar una partida de la ruleta de la suerte en grupo. \n");
    fprintf(stdout, "CONSONANTE letra, donde letra indica una consonante que se piensa que puede estar en la frase.\n");
    fprintf(stdout, "VOCAL letra, para poder mandar este mensaje necesitas tener al menos 50 puntos, que se restarán por cada vocal que solicites, con independencia del número de veces que aparezca. \n");
    fprintf(stdout, "RESOLVER  frase,  donde  frase  representará  una  cadena  que  contiene  el  refrán  que  queremos resolver.\n");
    fprintf(stdout, "PUNTUACION, solicitar la puntuación en el juego.\n");
    fprintf(stdout, "SALIR: mensaje para solicitar salir del juego.\n");
}