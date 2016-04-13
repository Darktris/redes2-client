/* vim: set ts=4 sw=4 et: */
/**
  @file G-2301-01-P1-ucomm.c
  @brief Libreria de manejo de comandos de usuario
  @author Sergio Fuentes  <sergio.fuentesd@estudiante.uam.es>
  @author Daniel Perdices <daniel.perdices@estudiante.uam.es>
  @date 2016/02/10
  */
#include <redes2/irc.h>
#include <redes2/ircxchat.h>
#include <syslog.h>
#include <G-2301-01-P1-tools.h>
#include <G-2301-01-P2-client.h>
#define UCOMM_NEXTERN
#include <G-2301-01-P2-ucomm.h>

ucomm_t ucommands[UCOMM_LEN];

/**
  @brief Atiende el comando de usuario JOIN
  @param command: El comando recibido
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
  @brief Atiende el comando de usuario LIST
  @param command: El comando recibido
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
  @brief Atiende el comando de usuario NICK
  @param command: El comando recibido
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
  @brief Atiende el comando de usuario PART
  @param command: El comando recibido
*/
void uPart(char* command) {
    char* msg;
    IRCUserParse_Part(command, &msg);

    //IRCMsg_Part(&comm, NULL, channel, tar);
}
void udefault(char* command) {
    syslog(LOG_INFO, "udef: ERROR no command");    
}

/**
  @brief Atiende el comando de usuario NAMES
  @param command: El comando recibido
*/
void uNames(char* command) {
    char* channel, *tar, *comm;
    IRCUserParse_Names(command, &channel, &tar);
    
    IRCMsg_Names(&comm, NULL, channel?channel:IRCInterface_ActiveChannelName(), tar);
    client_socketsnd(comm);

    if(comm) free(comm);
    if(channel) free(channel);
    if(tar) free(tar);
}

/**
  @brief Atiende el comando de usuario WHOIS
  @param command: El comando recibido
*/
void uWhoIs(char* command) {
    char* nick, *comm;
    IRCUserParse_Whois(command, &nick);
    IRCMsg_Whois (&comm, NULL, NULL, nick);
    client_socketsnd(comm);

    if(comm) free(comm);
    if(nick) free(nick);
}

/**
  @brief Atiende el comando de usuario PING
  @param command: El comando recibido
*/
void uPing(char* command) {
    IRCInterface_WriteSystem ("AVISO", "El comando PING solo soporta el envío de un PING al servidor conectado para comprobar la conexión");
    client_socketsnd("PING 0001234567890\r\n"); 
}

/**
  @brief Atiende el comando de usuario TOPIC
  @param command: El comando recibido
*/
void uTopic(char* command) {
    char *comm, *topic;
    IRCUserParse_Topic (command, &topic);
    IRCMsg_Topic(&comm, NULL, IRCInterface_ActiveChannelName(), topic); 
    client_socketsnd(comm);
    if(topic) free(topic);
    if(comm) free(comm);
} 
void init_ucomm() {
    int i;
    //UNAMES, UHELP, ULIST, UJOIN, UPART, ULEAVE, UQUIT, UNICK, UAWAY, UWHOIS, UINVITE, UKICK, UTOPIC, UME, UMSG, UQUERY, UNOTICE, UNOTIFY, UIGNORE, UPING, UWHO, UWHOWAS, UISON, UCYCLE, UMOTD, URULES, ULUSERS, UVERSION, UADMIN, UUSERHOST, UKNOCK, UVHOST, UMODE, UTIME, UBOTMOTD, UIDENTIFY, UDNS, UUSERIP, USTATS, UCTCP, UDCC, UMAP, ULINKS, USETNAME, ULICENSE, UMODULE, UPARTALL, UCHAT
    for(i=0;i<UCOMM_LEN; i++) {
        ucommands[i]=udefault;
    }
    ucommands[UNAMES] = uNames;
    ucommands[UJOIN] = uJoin;
    ucommands[ULIST] = uList;
    ucommands[UNICK] = uNick;
    ucommands[UWHOIS] = uWhoIs;
    ucommands[UPING] = uPing;
    ucommands[UTOPIC] = uTopic;
}
