#include "../juego/juego.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdbool.h>

#define MSG_SIZE 350
#define MAX_CLIENTS 30
static int sd;

struct Usuario
{
    int sd;
    char nickName[MSG_SIZE];
    char password[MSG_SIZE];
} typedef Usuario;

void manejador(int signum);

void salirCliente(int socket, fd_set *readfds, int *numClientes, Usuario arrayClientes[]);

int main(int argc, char **argv)
{
    system("clear");

    // Información del buffer y del socket
    int new_sd;
    struct sockaddr_in sockname, from;
    char buffer[MSG_SIZE];
    socklen_t from_len;
    fd_set readfds, auxfds;
    int salida;
    Usuario arrayClientes[MAX_CLIENTS];
    int numClientes = 0;
    //contadores
    int i, j, k;
    int recibidos;
    char identificador[MSG_SIZE];

    int on, ret;

    // Vector de usuarios del sistema, inicializo a -1 para que se sepa que no se ha recibido
    Usuario usuarios[MAX_CLIENTS];
    for(size_t i = 0; i < MAX_CLIENTS; ++i) {
        usuarios[i].sd = -1;
    }

    // Se abre el socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("No se puede abrir el socket cliente\n");
        exit(1);
    }

    // Activaremos una propiedad del socket para permitir· que otros
    // sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    // Esto permite· en protocolos como el TCP, poder ejecutar un
    // mismo programa varias veces seguidas y enlazarlo siempre al
    // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    // quedase disponible (TIME_WAIT en el caso de TCP)
    on = 1;
    ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(2050);
    sockname.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1)
    {
        perror("Error en la operación bind");
        exit(1);
    }

    from_len = sizeof(from);

    if (listen(sd, 1) == -1)
    {
        perror("Error en la operación de listen");
        exit(1);
    }

    // Inicializar los conjuntos fd_set
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd, &readfds);
    FD_SET(0, &readfds);

    // Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT, manejador);

    // El servidor a partir de ahora debe recibir cualquier solicitud
    while (true)
    {

        //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)

        auxfds = readfds;

        salida = select(FD_SETSIZE, &auxfds, NULL, NULL, NULL);

        if (salida > 0)
        {

            for (i = 0; i < FD_SETSIZE; i++)
            {

                //Buscamos el socket por el que se ha establecido la comunicación
                if (FD_ISSET(i, &auxfds))
                {

                    // Me conecto al servidor, aqui debo comprobar y aceptar el socket del cliente
                    if (i == sd)
                    {

                        if ((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1)
                        {
                            perror("Error aceptando peticiones");
                        }
                        else
                        {
                            if (numClientes < MAX_CLIENTS)
                            {
                                arrayClientes[numClientes].sd = new_sd;
                                numClientes++;
                                FD_SET(new_sd, &readfds);

                                strcpy(buffer, "||| Bienvenido al juego de la ruleta |||\n");

                                send(new_sd, buffer, sizeof(buffer), 0);

                                for (j = 0; j < (numClientes - 1); j++)
                                {

                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "Nuevo jugador conectado: %d\n", new_sd);
                                    send(arrayClientes[j].sd, buffer, sizeof(buffer), 0);
                                }
                            }
                            else
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "Demasiados clientes conectados\n");
                                send(new_sd, buffer, sizeof(buffer), 0);
                                close(new_sd);
                            }
                        }
                    }

                    else if (i == 0)
                    {
                        //Se ha introducido información de teclado por parte del servidor
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);

                        //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                        if (strcmp(buffer, "SALIR\n") == 0)
                        {

                            for (j = 0; j < numClientes; j++)
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "Desconexión servidor\n");
                                send(arrayClientes[j].sd, buffer, sizeof(buffer), 0);
                                close(arrayClientes[j].sd);
                                FD_CLR(arrayClientes[j].sd, &readfds);
                            }
                            close(sd);
                            exit(-1);
                        }
                        //Mensajes que se quieran mandar a los clientes (implementar)
                    }

                    // Un cliente ha enviado información por teclado, debo almacenar su sd que es i
                    // debo crear una estructura que sea el usuario con su sd, nicnkname, password, etc
                    else
                    {
                        bzero(buffer, sizeof(buffer));

                        recibidos = recv(i, buffer, sizeof(buffer), 0);

                        if (recibidos > 0)
                        {

                            if (strcmp(buffer, "SALIR\n") == 0)
                            {
                                fprintf(stdout, "SALIR RECIBIDO\n");
                                salirCliente(i, &readfds, &numClientes, arrayClientes);
                            }

                            // strncmp sscanf, comparo si str1 coincide en los 7 primeros digitos
                            else if (strncmp(buffer, "USUARIO ", 8) == 0)
                            {
                                // Almaceno el usuario en un char*
                                char user[MSG_SIZE];
                                sscanf(buffer, "USUARIO %s", user);
                                // Comprueba si el usuario existe desde el fichero de username y password
                                // y en el vector de estructuras almaceno de forma temporal el username 
                                // para comprobar el password

                                // Por tanto debo leer de un fichero donde almacene todos los datos

                                // Si el nickname de ese fichero coincide, lo almaceno su indice del vector

                                // Y ya despues para el password, con ese username almacenado en el vector
                                // checkeo en el fichero si para ese usuario existe ese password
                            }

                            else if (strncmp(buffer, "PASSWORD ", 9) == 0)
                            {
                                char password[MSG_SIZE];
                                sscanf(buffer, "PASSWORD %s", password);
                                fprintf(stdout, "PASSWORD RECIBIDO -> %s\n", password);
                            }

                            else
                            {
                                fprintf(stderr, "Comando desconocido recibido\n");
                            }

                            /*else
                            {

                                sprintf(identificador, "<%d>: %s", i, buffer);
                                bzero(buffer, sizeof(buffer));

                                strcpy(buffer, identificador);

                                printf("%s\n", buffer);

                                for (j = 0; j < numClientes; j++)
                                    if (arrayClientes[j] != i)
                                        send(arrayClientes[j], buffer, sizeof(buffer), 0);
                            }*/
                        }
                        //Si el cliente introdujo ctrl+c
                        if (recibidos == 0)
                        {
                            printf("El socket %d, ha introducido ctrl+c\n", i);
                            //Eliminar ese socket
                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                        }
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

void manejador(int signum)
{
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT, manejador);

    close(sd);

    exit(EXIT_SUCCESS);

    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}

void salirCliente(int socket, fd_set *readfds, int *numClientes, Usuario arrayClientes[])
{

    char buffer[250];
    int j;

    close(socket);
    FD_CLR(socket, readfds);

    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j].sd == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j + 1]);

    (*numClientes)--;

    bzero(buffer, sizeof(buffer));
    sprintf(buffer, "Desconexión del cliente: %d\n", socket);

    for (j = 0; j < (*numClientes); j++)
        if (arrayClientes[j].sd != socket)
            send(arrayClientes[j].sd, buffer, sizeof(buffer), 0);
}