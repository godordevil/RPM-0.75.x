/////////////////////////////////////////
//This fix's a buffer overflow in the
//QVM. DLL has its own fix (g_syscalls.c)
/////////////////////////////////////////
#include "g_local.h"

#ifdef Q3_VM
void trap_SendServerCommand2( int clientNum, const char *text );
void trap_SendServerCommand( int clientNum, const char *text ) {
        if( strlen( text ) >= 1022 ) {
			char a[1022] = "\0";
			Q_strncpyz (a, text, sizeof(a));
			trap_SendServerCommand2( -1, va("print \"^3Possible ^1CRASH Attempt ^3Logged: ^7%s\n\"", g_entities[clientNum].client->nvSess.cleanName));
			trap_SendServerCommand2(clientNum, a);
			a[800] = '\0';
			NV_crashLog (va("overflow: client:[%s//%s] - %s", g_entities[clientNum].client->nvSess.cleanName, g_entities[clientNum].client->nvSess.ip, a)) ;
            return;
        }
		trap_SendServerCommand2(clientNum, text);
} 

//  Same as the original trap_SendConsoleCommand() except removed the limit for the buffer overlow exploit
void	trap_SendDBQuery( int type, const char *query ) 
{
	trap_SendConsoleCommand(type, query);
}
#endif



