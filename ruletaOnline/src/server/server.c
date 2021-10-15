#include "../juego/juego.h"
#include "../queue/queue.h"
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
#define MAX_GAMES 15
static int sd;

/**
 * @depreceated 
 */
struct Partida
{
    // sd del jugador 1
    int player1;
    // sd del jugador 2
    int player2;
} typedef Partida;

struct Usuario
{
    int sd;
    char nickName[64];
    char password[64];

    // Socket del rival contra el que va a jugar
    int rival;

    // La variable turno se pone a 1 cuando le toque jugar, en cuyo caso podrá enviar una consonante, vocal, etc
    bool turno;

    // Variable que almacena la partida que está jugando, puntero para que asi los jugadores apunten a la misma partida
    struct Juego *partida;

    // Variable que almacena la puntuación del juego
    int puntuacion;

    // Variable que almacena si estoy en partida
    bool enPartida;

    // Variable que almacena si estoy encolado buscando
    bool encolado;

} typedef Usuario;

void manejador(int signum);

void salirCliente(int socket, fd_set *readfds, int *numClientes, Usuario arrayClientes[]);

bool comprobarUsername(char *usernameToCompare);

bool comprobarPassword(char *passwordToCompare, char *usernameToCompare);

void guardarUsuario(char *userName, char *password);

int getCliente(int socket, Usuario clientes[], const int numClientes);

int contarFrases();

void getFrase(char * frase);

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
    // Array con los clientes del sistema
    Usuario arrayClientes[MAX_CLIENTS];
    
    //frases
    // Array con las partidas en curso
    Partida partidas[MAX_GAMES];
    int numClientes = 0;
    //contadores
    int i, j, k;
    int recibidos;
    char identificador[MSG_SIZE];

    int on, ret;

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

                                // Inicializo el nuevo cliente para borrar sus posibles antiguos valores
                                arrayClientes[numClientes].nickName[0] = '\0';
                                arrayClientes[numClientes].password[0] = '\0';
                                arrayClientes[numClientes].rival = -1;
                                arrayClientes[numClientes].turno = 0;
                                arrayClientes[numClientes].partida = (struct Juego *)malloc(sizeof(struct Juego));
                                arrayClientes[numClientes].puntuacion = 0;
                                arrayClientes[numClientes].enPartida = false;
                                arrayClientes[numClientes].encolado = false;

                                numClientes++;
                                FD_SET(new_sd, &readfds);

                                strcpy(buffer, "+Ok. Usuario conectado\n");

                                send(new_sd, buffer, sizeof(buffer), 0);

                                /*for (j = 0; j < (numClientes - 1); j++)
                                {

                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "Nuevo jugador conectado: %d\n", new_sd);
                                    send(arrayClientes[j].sd, buffer, sizeof(buffer), 0);
                                }*/
                            }
                            else
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "-Err. Demasiados clientes conectados\n");
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
                                strcpy(buffer, ">>> Desconexión servidor\n");
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
                            if (strcmp(buffer, "\n") == 0)
                            {
                                // Descarto aquellas entradas con \n
                            }

                            else if (strcmp(buffer, "SALIR\n") == 0)
                            {
                                fprintf(stdout, "El cliente <%d> abandonó la sesión\n", arrayClientes[getCliente(i, arrayClientes, numClientes)].sd);

                                salirCliente(i, &readfds, &numClientes, arrayClientes);
                            }

                            // strncmp sscanf, comparo si str1 coincide en los 7 primeros digitos
                            else if (strncmp(buffer, "USUARIO ", 8) == 0)
                            {
                                // Almaceno el usuario en un char*
                                char user[MSG_SIZE];
                                sscanf(buffer, "USUARIO %s", user);

                                if (comprobarUsername(user))
                                {
                                    int index = getCliente(i, arrayClientes, numClientes);

                                    // Quiero copiar el nickName introducido del cliente con el socket sd en su variable nickName

                                    strcpy(arrayClientes[index].nickName, user);

                                    // Inicializo a \0 la password por si el usuario tenia un usuario previo validado, para borrar su anterior contraseña
                                    arrayClientes[index].password[0] = '\0';

                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Usuario correcto\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usuario incorrecto\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }
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

                                int index = getCliente(i, arrayClientes, numClientes);

                                if (comprobarPassword(password, arrayClientes[index].nickName))
                                {

                                    strcpy(arrayClientes[index].password, password);

                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Usuario validado\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }
                                else
                                {

                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Error en la validación\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }
                            }

                            else if (strncmp(buffer, "REGISTRO ", 9) == 0)
                            {
                                char password[MSG_SIZE], username[MSG_SIZE];
                                sscanf(buffer, "REGISTRO -u %s -p %s", username, password);

                                if (!comprobarUsername(username))
                                {
                                    guardarUsuario(username, password);
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Ok. Usuario registrado correctamente\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usuario ya registrado en el sistema\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }
                            }

                            else if (strcmp(buffer, "INICIAR-PARTIDA\n") == 0)
                            {

                                // Si el jugador ya estaba en partida previamente
                                if (arrayClientes[getCliente(i, arrayClientes, numClientes)].enPartida == true)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err.  Usted ya está en partida\n");

                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                // Si el jugador ya estaba encolado previamente
                                if (arrayClientes[getCliente(i, arrayClientes, numClientes)].encolado == true)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err.  Usted ya está en cola esperando partida\n");

                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                else
                                {
                                    int index = getCliente(i, arrayClientes, numClientes);

                                    // Compruebo si ambos char* no están a null dado que se han validado e inicializado previamente
                                    if (arrayClientes[index].nickName[0] != '\0' && arrayClientes[index].password[0] != '\0')
                                    {
                                        // Extraigo al jugador que estaba al principio de la cola
                                        int sdJugador = extraer();

                                        if (sdJugador == -1)
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "+Ok.  Petición  Recibida. Quedamos a la espera de más jugadores\n");

                                            // Inserto al jugador que estaba esperando a alguien
                                            insertar(i);

                                            // Indico que estoy en cola esperando
                                            arrayClientes[index].encolado = true;
                                            if (send(i, buffer, sizeof(buffer), 0) == -1)
                                            {
                                                perror("Error al invocar a send().");
                                            }
                                        }
                                        else
                                        {
                                            int indexPlayer1 = getCliente(sdJugador, arrayClientes, numClientes);
                                            int indexPlayer2 = getCliente(i, arrayClientes, numClientes);

                                            arrayClientes[indexPlayer1].rival = arrayClientes[indexPlayer2].sd;
                                            arrayClientes[indexPlayer2].rival = arrayClientes[indexPlayer1].sd;

                                            // Inicializao a 0 las puntuaciones
                                            arrayClientes[indexPlayer1].puntuacion = 0;
                                            arrayClientes[indexPlayer2].puntuacion = 0;

                                            // Indico que ya no estoy encolado
                                            arrayClientes[indexPlayer1].encolado = false;
                                            arrayClientes[indexPlayer2].encolado = false;

                                            // Indico que estoy en partida
                                            arrayClientes[indexPlayer1].enPartida = true;
                                            arrayClientes[indexPlayer2].enPartida = true;

                                            /*bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Empieza la partida. FRASE: \n");
                                        
                                        if (send(arrayClientes[indexPlayer1].sd, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        if (send(arrayClientes[indexPlayer2].sd, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }*/

                                            // Creo el refran y la estructura de juego en cada usuario
                                            char refran[64] = "por la boca muere el pez";

                                            struct Juego *juego = (struct Juego *)malloc(sizeof(struct Juego));
                                            char nuevaFrase[256];
                                            printf("%s",nuevaFrase);
                                            getFrase(nuevaFrase);
                                            *juego = crearCifrado(nuevaFrase);
                                            arrayClientes[indexPlayer1].partida = juego;
                                            arrayClientes[indexPlayer2].partida = juego;

                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. Empieza la partida. FRASE: %s.\n+Ok. Turno de partida\n", juego->fraseCifrada);

                                            // Empieza el jugador que estaba en la cola esperando
                                            if (send(arrayClientes[indexPlayer1].sd, buffer, sizeof(buffer), 0) == -1)
                                            {
                                                perror("Error al invocar a send().");
                                            }

                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. Empieza la partida. FRASE: %s.\n+Ok. Turno del otro jugador\n", juego->fraseCifrada);

                                            if (send(arrayClientes[indexPlayer2].sd, buffer, sizeof(buffer), 0) == -1)
                                            {
                                                perror("Error al invocar a send().");
                                            }

                                            arrayClientes[indexPlayer1].turno = 1;
                                            arrayClientes[indexPlayer2].turno = 0;
                                        }
                                    }

                                    else
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. Su usuario no ha sido validado\n");
                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }
                                    }
                                }
                            }

                            else if (strncmp(buffer, "CONSONANTE ", 11) == 0)
                            {
                                int indexPlayer = getCliente(i, arrayClientes, numClientes);

                                if (arrayClientes[indexPlayer].rival == -1)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usted no ha comenzado partida\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                else if (arrayClientes[indexPlayer].turno == 0)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. No es su turno todavía\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                // Compruebo si aparece la consonante
                                else
                                {
                                    char consonante;
                                    sscanf(buffer, "CONSONANTE %c", &consonante);

                                    // La consonante es una vocal
                                    if (consonante == 'a' || consonante == 'e' || consonante == 'i' || consonante == 'o' || consonante == 'u')
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. La letra introducida es una vocal\n");
                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }
                                    }

                                    // La letra ya fue escogida
                                    else if (letraEscogida(consonante, *arrayClientes[getCliente(i, arrayClientes, numClientes)].partida) == 0)
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. La letra ya se ha seleccionado previamente\n");
                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }
                                    }

                                    // Si ha acertado la letra
                                    else if (apareceLetra(consonante, *arrayClientes[getCliente(i, arrayClientes, numClientes)].partida) != -1)
                                    {
                                        descifrarLetra(consonante, arrayClientes[getCliente(i, arrayClientes, numClientes)].partida, i);

                                        // Si ha resuelto el último hueco
                                        if (fraseResuelta(*arrayClientes[getCliente(i, arrayClientes, numClientes)].partida))
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. Partida finalizada. FRASE: %s. Ha ganado el jugador %s con %d puntos\n Si quiere voler a iniciar una nueva partida indíquelo mediante el comando\n", arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->frase, arrayClientes[getCliente(i, arrayClientes, numClientes)].nickName, arrayClientes[getCliente(i, arrayClientes, numClientes)].puntuacion);

                                            if (send(i, buffer, sizeof(buffer), 0) == -1)
                                            {
                                                perror("Error al invocar a send().");
                                            }

                                            if (send(arrayClientes[getCliente(i, arrayClientes, numClientes)].rival, buffer, sizeof(buffer), 0) == -1)
                                            {
                                                perror("Error al invocar a send().");
                                            }

                                            // Pongo los datos de la partida a nulos
                                            arrayClientes[getCliente(i, arrayClientes, numClientes)].enPartida = false;
                                            arrayClientes[getCliente(i, arrayClientes, numClientes)].turno = 0;
                                            arrayClientes[getCliente(i, arrayClientes, numClientes)].encolado = false;

                                            int sd_rival = arrayClientes[getCliente(i, arrayClientes, numClientes)].rival;
                                            arrayClientes[getCliente(i, arrayClientes, numClientes)].rival = -1;

                                            arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].enPartida = false;
                                            arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].turno = 0;
                                            arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].encolado = false;
                                            arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].rival = -1;
                                        }

                                        else
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. %c aparece %d veces. FRASE: %s\n", consonante, numeroOcurrenciasLetra(consonante, *arrayClientes[indexPlayer].partida), arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->fraseCifrada);

                                            // Asigno la puntuación al jugador i
                                            arrayClientes[getCliente(i, arrayClientes, numClientes)].puntuacion += 50;

                                            // La letra es correcta, por tanto sigue jugando este jugador hasta que falle
                                            if (send(i, buffer, sizeof(buffer), 0) == -1)
                                            {
                                                perror("Error al invocar a send().");
                                            }

                                            if (send(arrayClientes[getCliente(i, arrayClientes, numClientes)].rival, buffer, sizeof(buffer), 0) == -1)
                                            {
                                                perror("Error al invocar a send().");
                                            }
                                        }
                                    }

                                    // El jugador ha fallado
                                    else
                                    {

                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. %c aparece 0 veces. FRASE: %s.\n+Ok. Turno del otro jugador\n", consonante, arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->fraseCifrada);
                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. %c aparece 0 veces. FRASE: %s.\n+Ok. Turno de partida\n", consonante, arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->fraseCifrada);
                                        if (send(arrayClientes[getCliente(i, arrayClientes, numClientes)].rival, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        // Cambio el turno del rival
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].turno = 0;
                                        int sd_rival = arrayClientes[getCliente(i, arrayClientes, numClientes)].rival;

                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].turno = 1;
                                    }
                                }
                            }

                            else if (strncmp(buffer, "VOCAL ", 6) == 0)
                            {
                                int indexPlayer = getCliente(i, arrayClientes, numClientes);

                                if (arrayClientes[indexPlayer].rival == -1)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usted no ha comenzado partida\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                else if (arrayClientes[indexPlayer].turno == 0)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. No es su turno todavía\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                // Compruebo si aparece la vocal
                                else
                                {
                                    char vocal;
                                    sscanf(buffer, "VOCAL %c", &vocal);

                                    // La letra introducida no es una vocal
                                    if (vocal != 'a' && vocal != 'e' && vocal != 'i' && vocal != 'o' && vocal != 'u')
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. La letra seleccionada no es una vocal\n");
                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }
                                    }

                                    // El usuario no tiene puntuación para comprar una vocal
                                    else if (arrayClientes[getCliente(i, arrayClientes, numClientes)].puntuacion < 50)
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. No tiene suficiente puntuación para comprar una vocal\n");
                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }
                                    }

                                    // La letra ya fue escogida
                                    else if (letraEscogida(vocal, *arrayClientes[getCliente(i, arrayClientes, numClientes)].partida) == 0)
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. La letra ya se ha seleccionado previamente\n");
                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }
                                    }

                                    // Si ha acertado la letra
                                    else if (apareceLetra(vocal, *arrayClientes[getCliente(i, arrayClientes, numClientes)].partida) != -1)
                                    {
                                        descifrarLetra(vocal, arrayClientes[getCliente(i, arrayClientes, numClientes)].partida, i);

                                        // Si ha resuelto el último hueco
                                        if (fraseResuelta(*arrayClientes[getCliente(i, arrayClientes, numClientes)].partida))
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. Partida finalizada. FRASE: %s. Ha ganado el jugador %s con %d puntos\n Si quiere voler a iniciar una nueva partida indíquelo mediante el comando\n", arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->frase, arrayClientes[getCliente(i, arrayClientes, numClientes)].nickName, arrayClientes[getCliente(i, arrayClientes, numClientes)].puntuacion);

                                            if (send(i, buffer, sizeof(buffer), 0) == -1)
                                            {
                                                perror("Error al invocar a send().");
                                            }

                                            if (send(arrayClientes[getCliente(i, arrayClientes, numClientes)].rival, buffer, sizeof(buffer), 0) == -1)
                                            {
                                                perror("Error al invocar a send().");
                                            }

                                            // Pongo los datos de la partida a nulos
                                            arrayClientes[getCliente(i, arrayClientes, numClientes)].enPartida = false;
                                            arrayClientes[getCliente(i, arrayClientes, numClientes)].turno = 0;
                                            arrayClientes[getCliente(i, arrayClientes, numClientes)].encolado = false;

                                            int sd_rival = arrayClientes[getCliente(i, arrayClientes, numClientes)].rival;
                                            arrayClientes[getCliente(i, arrayClientes, numClientes)].rival = -1;

                                            arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].enPartida = false;
                                            arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].turno = 0;
                                            arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].encolado = false;
                                            arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].rival = -1;
                                        }

                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. %c aparece %d veces. FRASE: %s\n", vocal, numeroOcurrenciasLetra(vocal, *arrayClientes[indexPlayer].partida), arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->fraseCifrada);

                                        // Le quito 50 puntos dado que ha consumido una vocal
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].puntuacion -= 50;

                                        // La letra es correcta, por tanto sigue jugando este jugador hasta que falle
                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        if (send(arrayClientes[getCliente(i, arrayClientes, numClientes)].rival, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }
                                    }

                                    // El jugador ha fallado
                                    else
                                    {

                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. %c aparece 0 veces. FRASE: %s.\n+Ok. Turno del otro jugador\n", vocal, arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->fraseCifrada);
                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. %c aparece 0 veces. FRASE: %s.\n+Ok. Turno de partida\n", vocal, arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->fraseCifrada);
                                        if (send(arrayClientes[getCliente(i, arrayClientes, numClientes)].rival, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        // Cambio el turno del rival
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].turno = 0;
                                        int sd_rival = arrayClientes[getCliente(i, arrayClientes, numClientes)].rival;

                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].turno = 1;
                                    }
                                }
                            }

                            else if (strcmp(buffer, "PUNTUACION\n") == 0)
                            {

                                int indexPlayer = getCliente(i, arrayClientes, numClientes);

                                if (arrayClientes[indexPlayer].rival == -1)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usted no ha comenzado partida\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                else if (arrayClientes[indexPlayer].turno == 0)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. No es su turno todavía\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "+Ok. Su puntuación es de: %d\n", arrayClientes[getCliente(i, arrayClientes, numClientes)].puntuacion);
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }
                            }

                            else if (strncmp(buffer, "RESOLVER ", 9) == 0)
                            {

                                // Si no está en partida
                                int indexPlayer = getCliente(i, arrayClientes, numClientes);

                                if (arrayClientes[indexPlayer].rival == -1)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usted no ha comenzado partida\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                else if (arrayClientes[indexPlayer].turno == 0)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. No es su turno todavía\n");
                                    if (send(i, buffer, sizeof(buffer), 0) == -1)
                                    {
                                        perror("Error al invocar a send().");
                                    }
                                }

                                // Está en partida y es su turno
                                else
                                {

                                    char fraseResuelta[350];
                                    strcpy(fraseResuelta, buffer+9);
                                    //memcpy(fraseResuelta,&buffer[9],strlen(buffer)-1);
                                    // Si ha acertado la frase
				for(int i=0;i<strlen(fraseResuelta);i++){
					if(fraseResuelta[i] == '_'){
					fraseResuelta[i]=' ';
					}
				}
                                    if (comparaFrase(*arrayClientes[getCliente(i, arrayClientes, numClientes)].partida, fraseResuelta))
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Partida finalizada. FRASE: %s. Ha ganado el jugador %s con %d puntos\n", arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->frase, arrayClientes[getCliente(i, arrayClientes, numClientes)].nickName, arrayClientes[getCliente(i, arrayClientes, numClientes)].puntuacion);

                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        if (send(arrayClientes[getCliente(i, arrayClientes, numClientes)].rival, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        // Pongo los datos de la partida a nulos
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].enPartida = false;
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].turno = 0;
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].encolado = false;

                                        int sd_rival = arrayClientes[getCliente(i, arrayClientes, numClientes)].rival;
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].rival = -1;

                                        
                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].enPartida = false;
                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].turno = 0;
                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].encolado = false;
                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].rival = -1;
                                    }

                                    // Si ha fallado la frase
                                    else
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Partida finalizada. FRASE: %s. No se ha acertado la frase\n Si quiere voler a iniciar una nueva partida indíquelo mediante el comando\n", arrayClientes[getCliente(i, arrayClientes, numClientes)].partida->frase);

                                        if (send(i, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        if (send(arrayClientes[getCliente(i, arrayClientes, numClientes)].rival, buffer, sizeof(buffer), 0) == -1)
                                        {
                                            perror("Error al invocar a send().");
                                        }

                                        // Pongo los datos de la partida a nulos
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].enPartida = false;
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].turno = 0;
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].encolado = false;

                                        int sd_rival = arrayClientes[getCliente(i, arrayClientes, numClientes)].rival;
                                        arrayClientes[getCliente(i, arrayClientes, numClientes)].rival = -1;

                                        
                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].enPartida = false;
                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].turno = 0;
                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].encolado = false;
                                        arrayClientes[getCliente(sd_rival, arrayClientes, numClientes)].rival = -1;
                                    }
                                }
                            }

                            else
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "-Err. Comando desconocido\n");
                                if (send(i, buffer, sizeof(buffer), 0) == -1)
                                {
                                    perror("Error al invocar a send().");
                                }
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
                            printf("El socket %d, ha introducido ctrl+c\n", arrayClientes[getCliente(i, arrayClientes, numClientes)].sd);
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

    // Extraigo al socket de la cola en caso de que estuviese esperando y haya cancelado la búsqueda
    extraer(socket);

    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j].sd == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j + 1]);

    (*numClientes)--;

    bzero(buffer, sizeof(buffer));
    sprintf(buffer, "+Ok. Desconexión del cliente: %d. Abortando partida. Si quieres empezar una nueva partida indícalo mediante el comando\n", socket);

    for (j = 0; j < (*numClientes); j++)
    {
        if (socket == arrayClientes[j].rival)
        {
            send(arrayClientes[j].sd, buffer, sizeof(buffer), 0);
            arrayClientes[j].rival = -1;
            arrayClientes[j].encolado = false;
            arrayClientes[j].enPartida = false;
        }
    }
}

bool comprobarUsername(char *usernameToCompare)
{
    FILE *file;
    char buffer[256];
    char username[64];
    char password[64];

    file = fopen("../data/usuarios.csv", "r");
    if (file == NULL)
    {
        perror("Error al llamar a fopen().");

        // Cierro el socket del servidor en caso de tener que detener la ejecución
        close(sd);
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, 256, file))
    {
        sscanf(buffer, "%s %s", username, password);

        if (strcmp(username, usernameToCompare) == 0)
            return true;
    }

    fclose(file);

    return false;
}

bool comprobarPassword(char *passwordToCompare, char *usernameToCompare)
{
    FILE *file;
    char buffer[256];
    char username[64];
    char password[64];

    file = fopen("../data/usuarios.csv", "r");
    if (file == NULL)
    {
        perror("Error al llamar a fopen().");

        // Cierro el socket del servidor en caso de tener que detener la ejecución
        close(sd);
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, 256, file))
    {
        sscanf(buffer, "%s %s", username, password);

        // Devuelvo verdadero en caso de que el username que tiene asignado a ese socket coincide con la entrada
        // del fichero y ademas con su correspondiente valor de contraseña
        if ((strcmp(username, usernameToCompare) == 0) && (strcmp(password, passwordToCompare) == 0))
            return true;
    }

    fclose(file);

    return false;
}

int getCliente(int socket, Usuario clientes[], const int numClientes)
{
    for (int i = 0; i < numClientes; ++i)
    {

        if (clientes[i].sd == socket)
        {
            return i;
        }
    }
}

void guardarUsuario(char *userName, char *password)
{
    FILE *file;

    file = fopen("../data/usuarios.csv", "a");
    if (file == NULL)
    {
        perror("Error al llamar a fopen().");

        // Cierro el socket del servidor en caso de tener que detener la ejecución
        close(sd);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s %s\n", userName, password);

    fclose(file);

    return;
}
int contarFrases(){
    FILE *file;
    char buffer[256];
    srand(time(NULL));
    int contador=0;
    char frase[256];
    file = fopen("../data/frases.txt", "r");
    if (file == NULL)
    {
        perror("Error al llamar a fopen().");

    }
    while (fgets(buffer, 256, file)){
        contador++;
    }
    fclose(file);
    return contador-1;
}
void getFrase(char * frase)
{
    FILE *file;
    char buffer[256];
    srand(time(NULL));
    int contador=contarFrases();
    int random=rand()%contador;
    int n=0;
    file = fopen("../data/frases.txt", "r");
    if (file == NULL)
    {
        perror("Error al llamar a fopen().");

    }
    while (fgets(buffer, 256, file))
    {   
        if(n==random){
                strcpy(frase,buffer);
        }
                n++;
    }
    fclose(file);
}
