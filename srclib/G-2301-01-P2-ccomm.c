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
*/
void cNick(char* command) {
    char *prefix, *nick, *msg, *old, *user, *host, *server, *realname, *password, *mnick;
    char text[512]={0};
    int ssl, port;
    IRCParse_Nick(command, &prefix, &nick, &msg);
    
    IRCParse_ComplexUser (prefix, &old, &user, &host, &server);
    if(user) free(user);
    if(host) free(host);
    if(server) free(server);
    
    IRCInterface_GetMyUserInfoThread(&mnick, &user, &realname,NULL, &server, &port, &ssl);
    if(user) free(user);
    if(realname) free(realname);
    if(server) free(server);
    
    if(!strcmp(mnick, old)) { 
        sprintf(text, "Usted es ahora conocido como %s", msg);
        IRCInterface_WriteSystemThread(nick, text);
    } else {
        sprintf(text, "%s es ahora conocido como %s", old, msg);
        //CHANGE NICK UI
        IRCInterface_WriteSystemThread(nick, text);
    }
    if(mnick) free(mnick);
    IRCInterface_ChangeNickThread(old, msg);
    if(old) free(old);
    if(nick) free(nick);
    if(msg) free(msg);
    if(prefix) free(prefix);
}

void cRplMotd(char* command) {
    char* prefix, *nick, *msg;
    IRCParse_RplMotd (command, &prefix, &nick, &msg);

    IRCInterface_WriteSystemThread(NULL, msg);
    if(nick) free(nick);
    if(msg) free(msg);
    if(prefix) free(prefix);
}

void cRplEndOfMotd(char* command) {
    char* prefix, *nick, *msg;
    IRCParse_RplEndOfMotd (command, &prefix, &nick, &msg);

    IRCInterface_WriteSystemThread(NULL, "End of MOTD command");
    if(nick) free(nick);
    if(msg) free(msg);
    if(prefix) free(prefix);
}
void cdefault(char* command) {
    char *chan = IRCInterface_ActiveChannelName();
    char text[500];
    sprintf(text, "Comando no implementado %s", command);
    if(strcmp(chan,"System")==0) 
        IRCInterface_WriteSystemThread(NULL, text);
    else IRCInterface_WriteChannelThread(chan, NULL, text);
}

void cRplList(char* command) {
    char* prefix, *nick, *channel, *visible, *topic;
    char text[500]={0};
    IRCParse_RplList (command, &prefix, &nick, &channel, &visible, &topic); 

    sprintf(text, "%s\t\t%s\t\t%s", channel, visible, !topic?"":topic);
    IRCInterface_WriteSystemThread(NULL, text);   
    if(prefix) free(prefix);
    if(nick) free(nick);
    if(channel) free(channel);
    if(visible) free(visible);
    if(topic) free(topic);
}

void cRplListEnd(char* command) {
    char* prefix, *nick, *msg;
    IRCParse_RplListEnd (command, &prefix, &nick, &msg);
    IRCInterface_WriteSystemThread(NULL, "End of LIST");
    if(prefix) free(prefix);
    if(nick) free(nick);
    if(msg) free(msg);
}

void cJoin(char* command) {
    char *prefix, *channel, *key, *msg, *mynick, *myuser, *myrealn, *pass, *myserver;
    char *nick, *user, *host, *server;
    char text[500];
    int port, ssl;
    IRCInterface_GetMyUserInfoThread(&mynick, &myuser, &myrealn, NULL, &myserver, &port, &ssl);
    IRCParse_Join (command, &prefix, &channel, &key, &msg); 
    IRCParse_ComplexUser (prefix, &nick, &user, &host, &server);

    if(strcmp(nick, mynick)==0) {
       if(!IRCInterface_QueryChannelExist(msg)) IRCInterface_AddNewChannelThread(msg, IRCInterface_ModeToIntMode("w"));
       IRCInterface_WriteChannelThread (msg, nick, "Has entrado al canal");
    } else {
        sprintf(text, "%s ha entrado al canal", nick);
        if(!IRCInterface_QueryChannelExist(msg)) IRCInterface_AddNewChannelThread(msg, IRCInterface_ModeToIntMode("w"));
        IRCInterface_WriteChannelThread (msg, nick,text);
        IRCInterface_AddNickChannelThread (msg, nick, nick, nick, nick, NONE); //TODO Ver estado del nick
    }

    if(user) free(user);
    if(host) free(host);
    if(server) free(server);

}

void cPrivMsg(char* command) {
    char *prefix, *tar, *msg, *nick, *user, *host, *server;
    IRCParse_Privmsg(command, &prefix, &tar, &msg);

    if(IRCInterface_QueryChannelExist(tar)) {
        IRCParse_ComplexUser (prefix, &nick, &user, &host, &server);
        IRCInterface_WriteChannelThread(tar, nick, msg);
    } else {
        IRCParse_ComplexUser (prefix, &nick, &user, &host, &server);
        // TODO diferenciar propio mensaje de los de otro
        IRCInterface_AddNewChannelThread(nick, IRCInterface_ModeToIntMode("w"));
        IRCInterface_WriteChannelThread(nick, nick, msg);
    }
    //TODO liberar memoria
}


void cRplWhoisUser(char* command) {
    char *prefix, *nick, *dest, *name, *host, *realn;
    char text[500];

    IRCParse_RplWhoIsUser (command, &prefix, &nick, &dest, &name, &host, &realn);
    sprintf(text,"[%s] Usuario %s con nombre %s host %s y nombre real %s", dest, dest, name, host, realn); 

    IRCInterface_WriteSystemThread(nick, text);   

    if(prefix) free(prefix);
    if(nick) free(nick);
    if(dest) free(dest);
    if(name) free(name);
    if(host) free(host);
    if(realn) free(realn);
}
void cRplWhoisChannels(char* command) {
    char *prefix, *nick, *dest, *chann;
    char text[500];
    IRCParse_RplWhoIsChannels (command, &prefix, &nick, &dest, &chann);

    sprintf(text,"[%s] Canales del usuario %s", dest, chann);
    if(chann) IRCInterface_WriteSystemThread(nick, text);   

    if(prefix) free(prefix);
    if(nick) free(nick);
    if(dest) free(dest);
    if(chann) free(chann);
}
void cRplWhoisOperator(char* command) {
    char *p, *n, *dest, *msg;
    char text[500];
    IRCParse_RplWhoIsOperator (command, &p, &n, &dest, &msg);
    sprintf(text,"[%s] es un operador del servidor", dest);
    IRCInterface_WriteSystemThread(n, text);   
    if(p) free(p);
    if(n) free(n);
    if(dest) free(dest);
    if(msg) free(msg);
}
void cRplWhoisServer(char* command) {
    char *prefix, *nick, *dest, *server, *sinfo;
    char text[500];
    IRCParse_RplWhoIsServer(command, &prefix, &nick, &dest, &server, &sinfo);

    sprintf(text,"[%s] Conectado al servidor %s: %s", dest, server, sinfo);
    IRCInterface_WriteSystemThread(nick, text);   

    if(prefix) free(prefix);
    if(nick) free(nick);
    if(dest) free(dest);
    if(server) free(server);
    if(sinfo) free(sinfo);
}

void cRplWhoisIdle(char* command) {
    char *prefix, *nick, *dest, *server, *sinfo, *msg;
    char text[500];
    int signon, secs_idle;
  

    IRCParse_RplWhoIsIdle(command, &prefix, &nick, &dest, &secs_idle, &signon, &msg);
   
    //TODO SIGNON 
    sprintf(text,"[%s] Inactivo desde hace %d segundos", dest, secs_idle ); 
    IRCInterface_WriteSystemThread(nick, text);   
    if(prefix) free(prefix);
    if(nick) free(nick);
    if(dest) free(dest);
    if(msg) free(msg); 
}

void cRplEndOfWhois(char* command) {
    char *prefix, *nick, *dest, *msg;
    char text[500];
    IRCParse_RplEndOfWhoIs (command, &prefix, &nick, &dest, &msg);
    sprintf(text,"[%s] Final de WHOIS", dest);
    IRCInterface_WriteSystemThread(nick, text);

    if(prefix) free(prefix);
    if(nick) free(nick);
    if(dest) free(dest);
    if(msg) free(dest);
}

void cTopic(char* command) {
    char *prefix, *nick, *topic, *channel;
    char text[500];
    IRCParse_Topic (command, &prefix, &channel, &topic);
    sprintf(text,"El topic del canal ahora es %s", topic);
    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), nick, text);
    if(prefix) free(prefix);
    if(channel) free(channel);
    if(topic) free(topic);
}
void cPing(char* command) {
    char* prefix, *server, *server2, *msg;
    char* comm;
    IRCParse_Ping (command, &prefix, &server, &server2, &msg);


    IRCMsg_Pong(&comm, NULL, server, NULL, server); 
    client_socketsnd_thread(comm);

    if(prefix) free(prefix);
    if(server) free(server);
    if(server2) free(server2);
    if(msg) free(msg);
    if(comm) free(comm);
}

void cRplTopic(char* command) {
    char *prefix, *nick, *topic, *channel;
    char text[500];
    IRCParse_RplTopic(command, &prefix, &nick, &channel, &topic);
    sprintf(text,"El topic del canal es %s", topic);
    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), NULL, text);
    if(prefix) free(prefix);
    if(nick) free(nick);
    if(channel) free(channel);
    if(topic) free(topic);
}

void cRplTopicWhoTime(char* command) {
   //TODO Eloy no parsea, yo no programo 
}
void cRplWelcome(char* command) {
    char *prefix, *nick, *msg;
    char text[500];
    IRCParse_RplWelcome (command, &prefix, &nick, &msg);
    sprintf(text,"Mensaje del servidor %s", msg);
    IRCInterface_WriteSystemThread(NULL, text);
    if(prefix) free(prefix);
    if(nick) free(nick); 
    if(msg) free(msg);
}

void cRplMotdStart(char* command) {
    IRCInterface_WriteSystemThread(NULL, "Inicio del MOTD");
}

void cRplNamReply(char* command) {
    char *prefix, *nick, *type, *channel, *msg; 
    char *u;
	char text[500];
    IRCParse_RplNamReply(command, &prefix, &nick, &type, &channel, &msg);
    sprintf(text,"Los usuarios del canal son: %s", msg);
    IRCInterface_WriteSystemThread(NULL, text);

    u = strtok(msg," ");
    while(u) { 
        IRCInterface_AddNickChannel (channel, u, u,u, u, u[0]=='@'?OPERATOR:(u[0]=='+'?VOICE:NONE));
        u = strtok(NULL, " ");
    } 

    //TODO liberar memoria
}

void cRplWhoReply(char* command) {
    char* prefix, *mnick, *channel, *user, *host, *server, *nick, *type, *msg, *realn;
    int hopcount;
    IRCParse_RplWhoReply (command, &prefix, &mnick, &channel, &user, &host, &server, &nick, &type, &msg, &hopcount, &realn);
    //TODO TERMINARLO
    IRCInterface_WriteSystemThread(NULL, nick);
    
}

void cPart(char* command) {
    char* prefix, *channel, *msg, *mynick, *myuser, *myrealn, *myserver, *nick, *user, *host, *server;
    char text[500];
    int port, ssl;
    IRCParse_Part(command, &prefix, &channel, &msg); 

    IRCInterface_GetMyUserInfoThread(&mynick, &myuser, &myrealn, NULL, &myserver, &port, &ssl);
    IRCParse_ComplexUser (prefix, &nick, &user, &host, &server);

    if(strcmp(mynick, nick)==0) {
       IRCInterface_RemoveChannelThread(channel); 
    } else {
        sprintf(text,"El usuario %s ha dejado el canal", nick);
        IRCInterface_WriteChannelThread(channel, NULL, text);
        IRCInterface_DeleteNickChannelThread (channel, nick);
    }

    //TODO liberar memoria
}

void cRplAway(char* command) {
    char* prefix, *nick, *dest, *msg;
    char text[500];
    IRCParse_RplAway (command, &prefix, &nick, &dest, &msg);
    sprintf(text, "El usuario está AWAY. El mensaje que ha dejado es: %s", msg);
    IRCInterface_WriteChannelThread(dest,dest,text);
    if(prefix) free(prefix);
    if(nick) free(nick);
    if(dest) free(dest);
    if(msg) free(msg);
}

void cRplNowAway(char* command) {
    char* prefix, *nick, *msg, *chan;
    IRCParse_RplNowAway (command, &prefix, &nick, &msg); 
    chan = IRCInterface_ActiveChannelName();
    if(strcmp(chan,"System")==0) 
        IRCInterface_WriteSystemThread(NULL, "Has sido marcado como ausente");
    else IRCInterface_WriteChannelThread(chan, NULL, "Has sido marcado como ausente");
    if(prefix) free(prefix);
    if(nick) free(nick);
    if(msg) free(msg);
}

void cRplUnAway(char* command) {
    char *chan = IRCInterface_ActiveChannelName();
    if(strcmp(chan,"System")==0) 
        IRCInterface_WriteSystemThread(NULL, "Ya no estás marcado como ausente");
    else IRCInterface_WriteChannelThread(chan, NULL, "Ya no estás marcado como ausente");
}

void cRplNoTopic(char* command) {
    char* p, *n, *ch, *topic;
    IRCParse_RplNoTopic(command, &p, &n, &ch, &topic);

    if(IRCInterface_QueryChannelExist(ch))
        IRCInterface_WriteChannelThread(ch, NULL, "El canal no tiene TOPIC definido");
    else IRCInterface_WriteSystemThread(NULL, "El canal no tiene TOPIC definido");
    if(p) free(p);
    if(n) free(n);
    if(ch) free(ch);
    if(topic) free(topic);
}

void cRplEndOfNames() {
    IRCInterface_WriteSystemThread(NULL, "Fin de la lista de nombres");
}

void cKick(char* command) {
    char* prefix, *channel, *msg, *mynick, *myuser, *myrealn, *myserver, *nick, *user, *host, *server, *dest;
    char text[500];
    int port, ssl;

    IRCParse_Kick (command, &prefix, &channel, &dest, &msg);

    IRCInterface_GetMyUserInfoThread(&mynick, &myuser, &myrealn, NULL, &myserver, &port, &ssl);
    IRCParse_ComplexUser (prefix, &nick, &user, &host, &server);

    if(strcmp(mynick, dest)==0) {
       IRCInterface_RemoveChannelThread (channel); 
       sprintf(text, "Has sido expulsado del canal por: %s. Razon: %s",nick, msg);
       IRCInterface_WriteSystemThread(NULL, text);
    } else {
       sprintf(text, "El usuario %s ha sido expulsado del canal por %s. Razon: %s",dest, nick, msg);
       IRCInterface_DeleteNickChannelThread (channel, dest);
       IRCInterface_WriteChannelThread(channel, NULL, text);
    }

    //TODO Liberar memoria

}

void cErrChanOpPrivIsNeeded(char* command) {
    char* prefix, *nick, *channel, *msg;
    IRCParse_ErrChanOPrivsNeeded (command, &prefix, &nick, &channel, &msg);
    IRCInterface_WriteChannelThread(channel, NULL, "Necesitas ser OPERATOR para realizar esta accion");
    if(prefix) free(prefix);
    if(nick) free(nick);
    if(channel) free(channel);
    if(msg) free(msg);
}

void cRplEndOfWho(char* command) {
    IRCInterface_WriteSystemThread(NULL, "Fin del WHO"); 
}

void cNotice(char* command) {
    char *prefix, *tar, *msg, *nick, *user, *host, *server;
    char *filename=NULL, *hostname_destino=NULL;
    unsigned long length, port;
    int socketd;
    char* buf;
    IRCParse_Notice(command, &prefix, &tar, &msg);

    IRCInterface_WriteSystemThread(tar, msg); 

    if(msg[0]==1) {
        if(sscanf(msg, "\001FSEND %ms %ms %ms %li %li",&nick, &filename, &hostname_destino, &port, &length) > 0) {
            if(IRCInterface_RecibirDialogThread (nick, filename)) {
                if(fork()==0) {
                    buf = malloc(length);      
                    client_tcpsocket_open(port, &socketd, hostname_destino); 
                    tcpsocket_rcv(socketd, &buf, length, &port);
                    puts(buf);
                    exit(0);
                }
            }
        } else if(sscanf(msg, "\001AUDIOCHAT %ms %li", &hostname_destino, &port) > 0) {
            printf("host=%s, port=%li\n", hostname_destino, port);
        } else puts("Formato incorrecto"); 

    }
    if(prefix) free(prefix);
    if(tar) free(tar);
    if(msg) free(msg);
}

void cErrBadChannelKey(char* command) {
    char* chan=IRCInterface_ActiveChannelName();
    if(strcmp(chan,"System")==0) 
        IRCInterface_WriteSystemThread(NULL, "Clave del canal incorrecta");
    else IRCInterface_WriteChannelThread(chan, NULL, "Clave del canal incorrecta");
}

void cErrConnection(char* command) {
    sleep(1);
    //IRCInterface_ErrorDialog("Hubo algún error con la conexión. Por favor pruebe su conexión al servidor así como pruebe con otros nicks/user/realname si estos fallan");
}

void cErrNoSuchNick(char* command) {
    char *prefix, *nick, *dest, *msg;
    char text[500];
    char* chan=IRCInterface_ActiveChannelName();
    IRCParse_ErrNoSuchNick (command, &prefix, &nick,  &dest, &msg);
    sprintf(text, "No se encontró el nick: %s", dest);
    if(strcmp(chan,"System")==0) 
        IRCInterface_WriteSystemThread(NULL, text);
    else IRCInterface_WriteChannelThread(chan, NULL, text);

    if(prefix) free(prefix);
    if(nick) free(nick);
    if(dest) free(dest);
    if(msg) free(msg);
}

void cErrNoSuchChannel(char* command) {
    char *prefix, *nick, *dest, *msg;
    char text[500];
    char* chan=IRCInterface_ActiveChannelName();
    IRCParse_ErrNoSuchChannel(command, &prefix, &nick,  &dest, &msg);
    sprintf(text, "No se encontró el canal: %s", dest);
    if(strcmp(chan,"System")==0) 
        IRCInterface_WriteSystemThread(NULL, text);
    else IRCInterface_WriteChannelThread(chan, NULL, text);

    if(prefix) free(prefix);
    if(nick) free(nick);
    if(dest) free(dest);
    if(msg) free(msg);
}

void cErrBannedFromChannel(char* command) {
    char *p, *n, *c, *msg;
    char *chan;
    char text[500];
    IRCParse_ErrBannedFromChan (command, &p, &n, &c, &msg);
    sprintf(text,"Estás baneado del canal %s. Mensaje: %s", c, msg);
    chan = IRCInterface_ActiveChannelName();
    if(strcmp(chan,"System")==0) 
        IRCInterface_WriteSystemThread(NULL, text);
    else IRCInterface_WriteChannelThread(c, NULL, text);

    if(p) free(p);
    if(n) free(n);
    if(chan) free(chan);
    if(msg) free(msg);

}

void init_ccomm() {
    int i;
    /*  NICK, OPER, MODE, SERVICE, QUIT, SQUIT, JOIN, PART, TOPIC, NAMES, LIST, INVITE, KICK, PRIVMSG, NOTICE, MOTD, LUSERS, VERSION, STATS, LINKS, TIME, CONNECT, TRACE, ADMIN, INFO, SERVLIST, SQUERY, WHO, WHOIS, WHOWAS, KILL, PING, PONG, ERROR, AWAY, REHASH, DIE, RESTART, SUMMON, USERS, WALLOPS, USERHOST, ISON, HELP, RULES, SERVER, ENCAP, CNOTICE, CPRIVMSG, NAMESX, SILENCE, UHNAMES, WATCH, KNOCK, USERIP,SETNAME, ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTRED, ERR_NONICKNAMEGIVEN, ERR_ERRONEUSNICKNAME, ERR_NICKNAMEINUSE, ERR_NICKCOLLISION, ERR_UNAVAILRESOURCE, ERR_RESTRICTED, RPL_YOUREOPER, ERR_NOOPERHOST, ERR_PASSWDMISMATCH, RPL_UMODEIS, ERR_UMODEUNKNOWNFLAG, ERR_USERSDONTMATCH, RPL_YOURESERVICE, RPL_YOURHOST, RPL_MYINFO, ERR_NOPRIVILEGES, ERR_NOSUCHSERVER, RPL_ENDOFWHO, RPL_ENDOFWHOIS, RPL_ENDOFWHOWAS, ERR_WASNOSUCHNICK, RPL_WHOWASUSER, RPL_WHOISUSER, RPL_WHOISCHANNELS, RPL_WHOISOPERATOR, RPL_WHOISSERVER, RPL_WHOISIDLE, RPL_WHOREPLY, ERR_BADMASK, ERR_CANNOTSENDTOCHAN, ERR_NOTEXTTOSEND, ERR_NOTOPLEVEL, ERR_WILDTOPLEVEL, ERR_BADCHANMASK, ERR_BADCHANNELKEY, RPL_BANLIST, ERR_BANNEDFROMCHAN, ERR_CHANNELISFULL, RPL_CHANNELMODEIS, ERR_CHANOPRIVSNEEDED, RPL_ENDOFBANLIST, RPL_ENDOFEXCEPTLIST, RPL_ENDOFINVITELIST, RPL_ENDOFNAMES, RPL_EXCEPTLIST, RPL_INVITELIST, ERR_INVITEONLYCHAN, RPL_INVITING, ERR_KEYSET, RPL_LISTSTART, RPL_LIST, RPL_LISTEND, RPL_NAMREPLY, ERR_NOCHANMODES, ERR_NOSUCHCHANNEL,ERR_NOTONCHANNEL, RPL_NOTOPIC, ERR_TOOMANYCHANNELS, ERR_TOOMANYTARGETS, ERR_UNKNOWNMODE, ERR_USERNOTINCHANNEL, ERR_USERONCHANNEL, RPL_UNIQOPIS, RPL_TOPIC, RPL_ADMINME, RPL_ADMINLOC1, RPL_ADMINLOC2, RPL_ADMINEMAIL, RPL_INFO, RPL_ENDOFLINKS, RPL_ENDOFINFO, RPL_ENDOFMOTD, RPL_ENDOFSTATS, RPL_LINKS, RPL_LUSERCHANNELS, RPL_LUSERCLIENT, RPL_LUSERME, RPL_LUSEROP, RPL_LUSERUNKNOWN, RPL_MOTD, RPL_MOTDSTART, ERR_NOMOTD, RPL_STATSCOMMANDS, RPL_STATSLINKINFO, RPL_STATSOLINE, RPL_STATSUPTIME, RPL_TIME, RPL_TRACECLASS, RPL_TRACECONNECT, RPL_TRACECONNECTING, RPL_TRACEHANDSHAKE, RPL_TRACELINK, RPL_TRACENEWTYPE, RPL_TRACEOPERATOR, RPL_TRACESERVER, RPL_TRACESERVICE, RPL_TRACEUSER, RPL_TRACEUNKNOWN, RPL_TRACELOG, RPL_TRACEEND, RPL_VERSION, ERR_NOSUCHSERVICE, RPL_SERVLIST, RPL_SERVLISTEND, ERR_CANTKILLSERVER, ERR_NOORIGIN, RPL_ENDOFUSERS, ERR_FILEERROR, RPL_ISON, ERR_NOLOGIN, RPL_NOUSERS, RPL_NOWAWAY, RPL_REHASHING, ERR_SUMMONDISABLED, RPL_SUMMONING, RPL_UNAWAY, RPL_USERHOST, RPL_USERS, ERR_USERSDISABLED, RPL_USERSSTART, RPL_AWAY, ERR_NOSUCHNICK, RPL_WELCOME, RPL_CREATED, RPL_BOUNCE, RPL_TRYAGAIN, ERR_UNKNOWNCOMMAND, ERR_NOADMININFO, ERR_NOTREGISTERED, ERR_NOPERMFORHOST, ERR_YOUREBANNEDCREEP, ERR_YOUWILLBEBANNED, ERR_BANLISTFULL, ERR_UNIQOPPRIVSNEEDED, ERR_NORECIPIENT, ERR_TOOMANYMATCHES, RPL_YOURID, RPL_CREATIONTIME, RPL_LOCALUSERS, y RPL_GLOBALUSERS,RPL_TOPICWHOTIME, RPL_CHANNELURL. */
    for(i=0;i<CCOMM_LEN;i++)
        ccommands[i] = cdefault;
    ccommands[RPL_MOTD]=cRplMotd;
    ccommands[RPL_ENDOFMOTD]=cRplEndOfMotd;
    ccommands[NICK]=cNick;
    ccommands[JOIN]=cJoin;
    ccommands[RPL_LIST]=cRplList;
    ccommands[RPL_LISTEND]=cRplListEnd;
    ccommands[PRIVMSG]=cPrivMsg;
    ccommands[NOTICE]=cNotice;
    ccommands[RPL_WHOISUSER]=cRplWhoisUser;
    ccommands[RPL_WHOISCHANNELS]=cRplWhoisChannels;
    ccommands[RPL_WHOISOPERATOR]=cRplWhoisOperator;
    ccommands[RPL_WHOISSERVER]=cRplWhoisServer;
    ccommands[RPL_WHOISIDLE]=cRplWhoisIdle;
    ccommands[RPL_ENDOFWHOIS]=cRplEndOfWhois;
    ccommands[PING]=cPing;
    ccommands[TOPIC]=cTopic;
    ccommands[PART]=cPart;
    ccommands[KICK]=cKick;
    ccommands[RPL_TOPIC]=cRplTopic;
    ccommands[RPL_NOTOPIC]=cRplNoTopic;
    ccommands[RPL_WELCOME]=cRplWelcome;
    ccommands[RPL_MOTDSTART]=cRplMotdStart;
    ccommands[RPL_NAMREPLY]=cRplNamReply;
    ccommands[RPL_NOWAWAY]=cRplNowAway;
    ccommands[RPL_WHOREPLY]=cRplWhoReply;
    ccommands[RPL_AWAY]=cRplAway;
    ccommands[RPL_ENDOFNAMES]=cRplEndOfNames;
    ccommands[RPL_ENDOFWHO]=cRplEndOfWho;
    ccommands[ERR_CHANOPRIVSNEEDED]=cErrChanOpPrivIsNeeded;
    ccommands[ERR_NOSUCHNICK]=cErrNoSuchNick;
    ccommands[ERR_NOSUCHCHANNEL]=cErrNoSuchChannel;
    ccommands[ERR_BANNEDFROMCHAN]=cErrBannedFromChannel;
    
    /* Errores de conexion */ 
    //ccommands[ERR_NICKNAMEINUSE]=cErrConnection;
    //ccommands[ERR_NICKCOLLISION]=cErrConnection;
    //ccommands[ERR_PASSWDMISMATCH]=cErrConnection;

}
