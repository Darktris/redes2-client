/* vim: set ts=4 sw=4 et: */
/**
  @file G-2301-01-P1-ucomm.c
  @brief Libreria de manejo de comandos de usuario
  @author Sergio Fuentes  <sergio.fuentesd@estudiante.uam.es>
  @author Daniel Perdices <daniel.perdices@estudiante.uam.es>
  @date 2016/02/10
  */
#include <redes2/irc.h>
#include <syslog.h>
#include <G-2301-01-P1-tools.h>
#include <G-2301-01-P2-client.h>
/**
  @brief Atiende el comando JOIN
  @param command: El comando recibido
  @param more: Puntero a estructura conn_data auxiliar
*/
void ujoin(char* command) {
    char *channel, *pass, *comm;

    IRCUserParse_Join(command, &channel, &pass);

    IRCMsg_Join (&comm, NULL, channel, pass, NULL);
    tcpsocket_snd(SOCKETD_CLIENT, comm, strlen(comm));
    if(comm) free(comm);

    if(pass) free(pass);
    if(channel) free(pass);
} 

/**
  @brief Atiende el comando LIST
  @param command: El comando recibido
  @param more: Puntero a estructura conn_data auxiliar
*/
void ulist(char* command) {
    char *channel, *st, *comm;

    IRCUserParse_List (command, &channel, &st);

    IRCMsg_List(&comm, NULL, channel, NULL);
    tcpsocket_snd(SOCKETD_CLIENT, comm, strlen(comm));
    if(comm) free(comm);

    if(channel) free(channel);
    if(st) free(st);
}

