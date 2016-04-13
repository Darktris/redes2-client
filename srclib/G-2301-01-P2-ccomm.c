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
    
    //IRCInterface_GetMyUserInfoThread(&mnick, &user, &realname, &password, &server, &port, &ssl);
    //if(user) free(user);
    //if(realname) free(realname);
    //if(password) free(password);
    //if(server) free(server);
    
    //if(!strcmp(mnick, old)) { 
        sprintf(text, "Usted es ahora conocido como %s", msg);
        IRCInterface_WriteSystemThread(nick, text);
    //} else {
        //sprintf(text, "%s es ahora conocido como %s", old, msg);
        //IRCInterface_WriteSystemThread(nick, text);
    //}
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
    //syslog(LOG_INFO, "Command %s is not being processed", command);
}

void cRplList(char* command) {
    char* prefix, *nick, *channel, *visible, *topic;
    char text[500]={0};
    IRCParse_RplList (command, &prefix, &nick, &channel, &visible, &topic); 

    sprintf(text, "%s\t\t%s\t\t%s", channel, visible, !topic?"":topic);
    IRCInterface_WriteSystemThread(nick, text);   
    if(prefix) free(prefix);
    if(nick) free(nick);
    if(channel) free(channel);
    if(visible) free(visible);
    if(topic) free(topic);
}

void cRplListEnd(char* command) {
    char* prefix, *nick, *msg;
    IRCParse_RplListEnd (command, &prefix, &nick, &msg);
    IRCInterface_WriteSystemThread(nick, "End of LIST");
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
       if(!IRCInterface_QueryChannelExist(channel)) IRCInterface_AddNewChannelThread(msg, IRCInterface_ModeToIntMode("w"));
       IRCInterface_WriteChannelThread (msg, nick, "Has entrado al canal");
    } else {
        sprintf(text, "%s ha entrado al canal", nick);
        if(!IRCInterface_QueryChannelExist(channel)) IRCInterface_AddNewChannelThread(msg, IRCInterface_ModeToIntMode("w"));
        IRCInterface_WriteChannelThread (msg, nick,text);
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
    //TODO Sgdo
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

 void cRplNoTopic(char* command) {
    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), NULL, "El canal no tiene topic definido");
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
    ccommands[RPL_WHOISUSER]=cRplWhoisUser;
    ccommands[RPL_WHOISCHANNELS]=cRplWhoisChannels;
    ccommands[RPL_WHOISOPERATOR]=cRplWhoisOperator;
    ccommands[RPL_WHOISSERVER]=cRplWhoisServer;
    ccommands[RPL_WHOISIDLE]=cRplWhoisIdle;
    ccommands[RPL_ENDOFWHOIS]=cRplEndOfWhois;
    ccommands[PING]=cPing;
    ccommands[TOPIC]=cTopic;
    ccommands[RPL_TOPIC]=cRplTopic;
    ccommands[RPL_NOTOPIC]=cRplNoTopic;
}
