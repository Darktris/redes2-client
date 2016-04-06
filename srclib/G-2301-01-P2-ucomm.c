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
#define UCOMM_NEXTERN
#include <G-2301-01-P2-ucomm.h>

ucomm_t ucommands[UCOMM_LEN];
/**
  @brief Atiende el comando JOIN
  @param command: El comando recibido
  @param more: Puntero a estructura conn_data auxiliar
*/
void uJoin(char* command) {
    char *channel, *pass, *comm;
    
    IRCUserParse_Join(command, &channel, &pass);
    syslog(LOG_INFO, "ujoin: parsing");

    IRCMsg_Join (&comm, NULL, channel, pass, NULL);
    client_socketsnd(comm);
    if(comm) free(comm);

    if(pass) free(pass);
    if(channel) free(pass);
} 

/**
  @brief Atiende el comando LIST
  @param command: El comando recibido
  @param more: Puntero a estructura conn_data auxiliar
*/
void uList(char* command) {
    char *channel, *st, *comm;

    IRCUserParse_List (command, &channel, &st);

    IRCMsg_List(&comm, NULL, channel, NULL);
    client_socketsnd(comm);
    if(comm) free(comm);

    if(channel) free(channel);
    if(st) free(st);
}

/**
  @brief Atiende el comando NICK
  @param command: El comando recibido
  @param more: Puntero a estructura conn_data auxiliar
*/
void uNick(char* command) {
    char* nick, *comm;
    IRCUserParse_Nick (command, &nick);

    IRCMsg_Nick(&comm, NULL, nick, NULL);
    client_socketsnd(comm);
    if(comm) free(comm);

    if(nick) free(nick);
}
 
/**
  @brief Atiende el comando PART
  @param command: El comando recibido
  @param more: Puntero a estructura conn_data auxiliar
*/
void uPart(char* command) {
    char* msg;
    IRCUserParse_Part(command, &msg);

    //IRCMsg_Part(&comm, NULL, 
}
void udefault(char* command) {
    syslog(LOG_INFO, "udef: ERROR no command");    
}

void init_ucomm() {
    int i;
    //UNAMES, UHELP, ULIST, UJOIN, UPART, ULEAVE, UQUIT, UNICK, UAWAY, UWHOIS, UINVITE, UKICK, UTOPIC, UME, UMSG, UQUERY, UNOTICE, UNOTIFY, UIGNORE, UPING, UWHO, UWHOWAS, UISON, UCYCLE, UMOTD, URULES, ULUSERS, UVERSION, UADMIN, UUSERHOST, UKNOCK, UVHOST, UMODE, UTIME, UBOTMOTD, UIDENTIFY, UDNS, UUSERIP, USTATS, UCTCP, UDCC, UMAP, ULINKS, USETNAME, ULICENSE, UMODULE, UPARTALL, UCHAT
    for(i=0;i<UCOMM_LEN; i++) {
        ucommands[i]=udefault;
    }
    ucommands[UJOIN] = uJoin;
    ucommands[ULIST] = uList;
    ucommands[UNICK] = uNick;
}
