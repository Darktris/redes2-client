/* vim: set ts=4 sw=4 et: */
/**
  @file G-2301-01-P1-irc.c
  @brief Libreria de manejo de mensajes IRC 
  @author Sergio Fuentes  <sergio.fuentesd@estudiante.uam.es>
  @author Daniel Perdices <daniel.perdices@estudiante.uam.es>
  @date 2016/02/10
  */
#include <G-2301-01-P1-irc.h>
#include <redes2/irc.h>
#include <redes2/ircxchat.h>
#include <syslog.h>
#include <G-2301-01-P2-client.h>
#include <G-2301-01-P1-tools.h>
#define CCOM_NEXTERN
#include <G-2301-01-P2-ccomm.h>

ccomm_t ccommands[CCOMM_LEN];
/**
  @brief Atiende el comando NICK
  @param command: El comando recibido
  @param more: Puntero a estructura conn_data auxiliar
  @return Ningún valor definido, la función controlan el error de manera interna
*/
void cNick(char* command) {
    char *prefix, *nick, *msg;
    char text[512]={0};
    IRCParse_Nick(command, &prefix, &nick, &msg);
    
    sprintf(text, "Usted es ahora conocido como %s", msg);
    IRCInterface_WriteSystemThread(nick, text);

    if(nick) free(nick);
    if(msg) free(msg);
    if(prefix) free(prefix);
}

void cRplMotd(char* command) {
    char* prefix, *nick, *msg;
    IRCParse_RplMotd (command, &prefix, &nick, &msg);

    IRCInterface_WriteSystemThread(nick, msg);
    if(nick) free(nick);
    if(msg) free(msg);
    if(prefix) free(prefix);
}

void cRplEndOfMotd(char* command) {
    char* prefix, *nick, *msg;
    IRCParse_RplEndOfMotd (command, &prefix, &nick, &msg);

    IRCInterface_WriteSystemThread(nick, "End of MOTD command");
    if(nick) free(nick);
    if(msg) free(msg);
    if(prefix) free(prefix);
}
void cdefault(char* command) {
    syslog(LOG_INFO, "Command %s is not being processed", command);
}

void init_ccomm() {
    int i;
    for(i=0;i<CCOMM_LEN;i++)
        ccommands[i] = cdefault;
    ccommands[RPL_MOTD]=cRplMotd;
    ccommands[RPL_ENDOFMOTD]=cRplEndOfMotd;
    ccommands[NICK]=cNick;
}


