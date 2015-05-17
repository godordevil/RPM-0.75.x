// Copyright (C) 2001-2002 Raven Software
// Copyright (C) 2004-2005 Ryan Powell

//  This file is part of RPM.

//  RPM is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation;either version 2 of the License, or
//  (at your option) any later version.

//  RPM is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with RPM; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

// this file holds commands that can be executed by the server console, but not remote clients

#include "g_local.h"

/*
==============================================================================

PACKET FILTERING


You can add or remove addresses from the filter list with:

addip <ip>
removeip <ip>

The ip address is specified in dot format, and any unspecified digits will match any value, so you can specify an entire class C network with "addip 192.246.40".

Removeip will only remove an address specified exactly the same way.  You cannot addip a subnet, then removeip a single host.

listip
Prints the current list of filters.

g_filterban <0 or 1>

If 1 (the default), then ip addresses matching the current list will be prohibited from entering the game.  This is the default setting.

If 0, then only addresses matching the list will be allowed.  This lets you easily set up a private game, or a game that only allows players from your local network.


==============================================================================
*/

// extern	vmCvar_t	g_banIPs;
// extern	vmCvar_t	g_filterBan;


typedef struct ipFilter_s
{
	unsigned	mask;
	unsigned	compare;
} ipFilter_t;

#define	MAX_IPFILTERS	1024

static ipFilter_t	ipFilters[MAX_IPFILTERS];
static int			numIPFilters;

char	*ConcatArgs( int start );

/*
=================
StringToFilter
=================
*/
static qboolean StringToFilter (char *s, ipFilter_t *f)
{
	char	num[128];
	int		i, j;
	byte	b[4];
	byte	m[4];

	for (i=0 ; i<4 ; i++)
	{
		b[i] = 0;
		m[i] = 0;
	}

	for (i=0 ; i<4 ; i++)
	{
		if (*s < '0' || *s > '9')
		{
			Com_Printf( "Bad filter address: %s\n", s );
			return qfalse;
		}

		j = 0;
		while (*s >= '0' && *s <= '9')
		{
			num[j++] = *s++;
		}
		num[j] = 0;
		b[i] = atoi(num);
		if (b[i] != 0)
			m[i] = 255;

		if (!*s)
			break;
		s++;
	}

	f->mask = *(unsigned *)m;
	f->compare = *(unsigned *)b;

	return qtrue;
}

/*
=================
UpdateIPBans
=================
*/
static void UpdateIPBans (void)
{
	byte	b[4];
	int		i;
	char	iplist[MAX_INFO_STRING];

	*iplist = 0;
	for (i = 0 ; i < numIPFilters ; i++)
	{
		if (ipFilters[i].compare == 0xffffffff)
			continue;

		*(unsigned *)b = ipFilters[i].compare;
		Com_sprintf( iplist + strlen(iplist), sizeof(iplist) - strlen(iplist),
			"%i.%i.%i.%i ", b[0], b[1], b[2], b[3]);
	}

	trap_Cvar_Set( "g_banIPs", iplist );
}

/*
=================
G_FilterPacket
=================
*/
qboolean G_FilterPacket (char *from)
{
	int		i;
	unsigned	in;
	byte m[4];
	char *p;

	m[0] = m[1] = m[2] = m[3] = 0;

	i = 0;
	p = from;
	while (*p && i < 4) {
		while (*p >= '0' && *p <= '9') {
			m[i] = m[i]*10 + (*p - '0');
			p++;
		}
		if (!*p || *p == ':')
			break;
		i++, p++;
	}

	in = *(unsigned *)m;

	for (i=0 ; i<numIPFilters ; i++)
		if ( (in & ipFilters[i].mask) == ipFilters[i].compare)
			return g_filterBan.integer != 0;

	return g_filterBan.integer == 0;
}

/*
=================
AddIP
=================
*/
static void AddIP( char *str )
{
	int		i;

	for (i = 0 ; i < numIPFilters ; i++)
	{
		if (ipFilters[i].compare == 0xffffffff)
		{
			break;		// free spot
		}
	}

	if (i == numIPFilters)
	{
		if (numIPFilters == MAX_IPFILTERS)
		{
			Com_Printf ("IP filter list is full\n");
			return;
		}
		numIPFilters++;
	}

	if (!StringToFilter (str, &ipFilters[i]))
	{
		ipFilters[i].compare = 0xffffffffu;
	}

	UpdateIPBans();
}

/*
=================
G_ProcessIPBans
=================
*/
void G_ProcessIPBans(void)
{
	char *s, *t;
	char		str[MAX_TOKEN_CHARS];

	Q_strncpyz( str, g_banIPs.string, sizeof(str) );

	for (t = s = g_banIPs.string; *t; /* */ ) {
		s = strchr(s, ' ');
		if (!s)
			break;
		while (*s == ' ')
			*s++ = 0;
		if (*t)
			AddIP( t );
		t = s;
	}
}


/*
=================
Svcmd_AddIP_f
=================
*/
void Svcmd_AddIP_f (void)
{
	char		str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		Com_Printf("Usage:  addip <ip-mask>\n");
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	AddIP( str );

}

/*
=================
Svcmd_RemoveIP_f
=================
*/
void Svcmd_RemoveIP_f (void)
{
	ipFilter_t	f;
	int			i;
	char		str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		Com_Printf("Usage:  sv removeip <ip-mask>\n");
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	if (!StringToFilter (str, &f))
		return;

	for (i=0 ; i<numIPFilters ; i++) {
		if (ipFilters[i].mask == f.mask	&&
			ipFilters[i].compare == f.compare) {
			ipFilters[i].compare = 0xffffffffu;
			Com_Printf ("Removed.\n");

			UpdateIPBans();
			return;
		}
	}

	Com_Printf ( "Didn't find %s.\n", str );
}

/*
===================
Svcmd_EntityList_f
===================
*/
void	Svcmd_EntityList_f (void) {
	int			e;
	gentity_t		*check;

	check = g_entities+1;

#ifdef _DEBUG
		NV_Log (2, va("\n\nNum Entities: %i\n-----------------\n", level.num_entities)) ;
#endif

	for (e = 1; e < level.num_entities ; e++, check++) {
		if ( !check->inuse ) {
			continue;
		}
		Com_Printf("%3i:", e);
		switch ( check->s.eType ) {
		case ET_GENERAL:
			Com_Printf("ET_GENERAL          ");
			break;
		case ET_PLAYER:
			Com_Printf("ET_PLAYER           ");
			break;
		case ET_ITEM:
			Com_Printf("ET_ITEM             ");
			break;
		case ET_MISSILE:
			Com_Printf("ET_MISSILE          ");
			break;
		case ET_MOVER:
			Com_Printf("ET_MOVER            ");
			break;
		case ET_BEAM:
			Com_Printf("ET_BEAM             ");
			break;
		case ET_PORTAL:
			Com_Printf("ET_PORTAL           ");
			break;
		case ET_SPEAKER:
			Com_Printf("ET_SPEAKER          ");
			break;
		case ET_PUSH_TRIGGER:
			Com_Printf("ET_PUSH_TRIGGER     ");
			break;
		case ET_TELEPORT_TRIGGER:
			Com_Printf("ET_TELEPORT_TRIGGER ");
			break;
		case ET_INVISIBLE:
			Com_Printf("ET_INVISIBLE        ");
			break;
		case ET_GRAPPLE:
			Com_Printf("ET_GRAPPLE          ");
			break;
		default:
			Com_Printf("%3i                 ", check->s.eType);
			break;
		}

		if ( check->classname ) {
			Com_Printf("%s", check->classname);
#ifdef _DEBUG
			NV_Log (2, va("[%3i] %s",e, check->classname)) ;
#endif
		}
		Com_Printf("\n");
	}
}


void Svcmd_ExtendTime_f (void)
{
	char str[MAX_TOKEN_CHARS];
	int	 time;

	if ( trap_Argc() < 2 )
	{
		Com_Printf("Usage:  extendtime <minutes>\n");
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	time = atoi(str);
	level.timeExtension += time;
#ifdef _DB_ENABLED
	mapStats.timeExtended = qtrue;
#endif

	G_LogPrintf ( "timelimit extended by %d minutes\n", time );

	trap_SendServerCommand( -1, va("print \"timelimit extended by %d minutes\n\"", time) );
}

gclient_t	*ClientForString( const char *s ) {
	gclient_t	*cl;
	int			i;
	int			idnum;

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' ) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			Com_Printf( "Bad client slot: %i\n", idnum );
			return NULL;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			Com_Printf( "Client %i is not connected\n", idnum );
			return NULL;
		}
		return cl;
	}

	// check for a name match
	for ( i=0 ; i < level.maxclients ; i++ ) {
		cl = &level.clients[i];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( !Q_stricmp( cl->pers.netname, s ) ) {
			return cl;
		}
	}

	Com_Printf( "User %s is not on the server\n", s );

	return NULL;
}

/*
===================
Svcmd_ForceTeam_f

forceteam <player> <team>
===================
*/
void Svcmd_ForceTeam_f( void )
{
	gclient_t	*cl;
	char		str[MAX_TOKEN_CHARS];

	// find the player
	trap_Argv( 1, str, sizeof( str ) );
	cl = ClientForString( str );
	if ( !cl )
	{
		return;
	}

	// set the team
	trap_Argv( 2, str, sizeof( str ) );
	SetTeam( &g_entities[cl - level.clients], str, NULL );
}

/*
===================
Svcmd_CancelVote_f

cancels the vote in progress
===================
*/
void Svcmd_CancelVote_f ( void )
{
	level.voteTime = 0;

	trap_SetConfigstring( CS_VOTE_TIME, "" );

	trap_SendServerCommand( -1, "print \"Vote cancelled by admin.\n\"" );
}

/*
=================
ConsoleCommand
=================
*/
qboolean ConsoleCommand( void )
{
	char cmd[MAX_TOKEN_CHARS];
	//Ryan
#ifdef _RPM_WEAPONMOD
	char arg1[64];
#endif
	//Ryan

	trap_Argv( 0, cmd, sizeof( cmd ) );

	if ( Q_stricmp (cmd, "entitylist") == 0 )
	{
		Svcmd_EntityList_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "forceteam") == 0 )
	{
		Svcmd_ForceTeam_f();
		return qtrue;
	}
	/*
	if ( Q_stricmp ( cmd, "cancelvote" ) == 0 )
	{
		Svcmd_CancelVote_f();
		return qtrue;
	}
	*/
#ifdef _SOF2_BOTS

	if (Q_stricmp (cmd, "addbot") == 0)
	{
		Svcmd_AddBot_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "botlist") == 0)
	{
		Svcmd_BotList_f();
		return qtrue;
	}

#endif

	if (Q_stricmp (cmd, "addip") == 0)
	{
		Svcmd_AddIP_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "removeip") == 0)
	{
		Svcmd_RemoveIP_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "listip") == 0)
	{
		trap_SendConsoleCommand( EXEC_NOW, "g_banIPs\n" );
		return qtrue;
	}

	if (Q_stricmp (cmd, "gametype_restart" ) == 0 )
	{
		G_ResetGametype ( );
		return qtrue;
	}

	if (Q_stricmp (cmd, "extendtime" ) == 0 )
	{
		Svcmd_ExtendTime_f();
		return qtrue;
	}	

	// GodOrDevil 
#ifdef _NEW_MAP_SETUP

	//  Min/Max players needed for map votes
	if (Q_stricmp (cmd, "maxplayers") == 0 || Q_stricmp (cmd, "minplayers") == 0)
	{
		// just so its not echoed in chat.
		return qtrue;
	}
	if (Q_stricmp (cmd, "setRandom" ) == 0 )
	{
		if(level.randMapNum > 0)
			level.doRandom = qtrue;
		return qtrue;
	}
#endif
	if (Q_stricmp(cmd, "admcmds") == 0)
	{
		char cmd2[MAX_TOKEN_CHARS];
		trap_Argv(1, cmd2, sizeof(cmd2));
		NV_setRestriction(cmd2);
		return qtrue;
	}
	if (cmd[2])
	{
		if (!NV_rconCmds(cmd))
			return qtrue;
	}

#ifdef _RPM_WEAPONMOD
	if (Q_stricmp (cmd, "mod" ) == 0 )
	{
		int		mod;
		int		flags = g_weaponModFlags.integer;

		if(!*g_weaponModFile.string)
		{
			Com_Printf("^3No mod file specified in g_weaponModFile cvar\n");
			return qtrue;
		}

		trap_Argv( 1, arg1, sizeof( arg1 ) );
		switch (arg1[0])
		{
			case 'W':
			case 'w':
				flags |= (AMMO_MOD|DAMAGE_MOD);
				mod = 3; // 3 == AMMO_MOD & DAMAGE_MOD
				break;

			case 'A':
			case 'a':
				flags |= AMMO_MOD;
				mod = AMMO_MOD;
				break;

			case 'D':
			case 'd':
				flags |= DAMAGE_MOD;
				mod = DAMAGE_MOD;
				break;
			default:
				Com_Printf("Bad argument %s\n", arg1);
				Com_Printf("Valid arguments are: ^3w^7eapon, ^3a^7mmo, ^3d^7amage\n");
				return qtrue;
		}
		//we set the cvar 2 ways so because it takes to long for the
		//trap_cvar_set to change the value which is used in the
		//RPM_WeaponMod function -- kind of a hack I dont like it but...
		g_weaponModFlags.integer = flags;
		trap_Cvar_Set( "g_weaponModFlags", va("%d", flags));

		RPM_WeaponMod(qtrue, mod);
		RPM_UpdateLoadScreenMessage();
		return qtrue;
	}

	if (Q_stricmp (cmd, "unmod" ) == 0 )
	{
		int		flags = g_weaponModFlags.integer;

		trap_Argv( 1, arg1, sizeof( arg1 ) );
		switch (arg1[0])
		{
			case 'W':
			case 'w':
				if(flags == 0)
				{
					Com_Printf("^3Weapon stats are already at default values\n");
					return qtrue;
				}
				flags = 0;
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, Servers ^3Default Damage & Ammos ^7were Restored", level.time + 5000));
				G_LogPrintf("Default ammos & Damage loaded\n");
				break;

			case 'A':
			case 'a':
				if(!(flags & AMMO_MOD))
				{
					Com_Printf("^3Weapon Ammos are already at default values\n");
					return qtrue;
				}
				flags &= ~AMMO_MOD;
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, Servers ^3Default Ammo ^7amounts were Restored", level.time + 5000));
				G_LogPrintf("Default ammos loaded\n");
				break;

			case 'D':
			case 'd':
				if(!(flags & DAMAGE_MOD))
				{
					Com_Printf("^3Weapon Damages are already at default values\n");
					return qtrue;
				}
				flags &= ~DAMAGE_MOD;
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^7Servers ^3Default Damage ^7amounts were Restored", level.time + 5000));
				G_LogPrintf("Default Damage loaded\n");
				break;
			default:
				Com_Printf("Bad argument %s\n", arg1);
				Com_Printf("Valid arguments are: ^3w^7eapon, ^3a^7mmo, ^3d^7amage\n");
				return qtrue;
		}

		//we set the cvar 2 ways so because it takes to long for the
		//trap_cvar_set to change the value which is used in the
		//RPM_Undo_WeaponMods function - kind of a hack I dont like it but...
		g_weaponModFlags.integer = flags;
		trap_Cvar_Set( "g_weaponModFlags", va("%d", flags));

		RPM_Undo_WeaponMods();
		RPM_UpdateLoadScreenMessage();
		return qtrue;
	}
#endif
	if ((Q_stricmp (cmd, "addshader" ) == 0) ) 
	{	
		char		cmd2[MAX_QPATH] = "textures/skies/inf_shop";
		char		cmd3[MAX_QPATH] = "textures/skies/jor1";
		extern		int remapCount;
		float f = level.time * 0.001;
		
		trap_Argv( 1, cmd2, sizeof(cmd2));
		trap_Argv( 2, cmd3, sizeof( cmd3 ) );
		AddRemap(cmd2, cmd3, f);
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
		Com_Printf("Shader Remapped: ^5%i ^7of ^5%i\n", remapCount, 128);
		return qtrue;
	}
	
	// END REMOVE
#ifdef _DB_ENABLED

#ifdef _DEBUG
	if ( Q_stricmp ( cmd, "maptest" ) == 0 )
	{
		NV_getMapList(qfalse);
		return qtrue;
	}
	if ( Q_stricmp ( cmd, "endgame" ) == 0 )
	{
		level.teamScores[TEAM_RED] = g_scorelimit.integer + 1;
		//NV_SaveMapStats();
		return qtrue;
	}	
	if ( Q_stricmp ( cmd, "savestats" ) == 0 )
	{
		RPM_Awards();
		NV_SaveMapStats();
		return qtrue;
	}

	if ( Q_stricmp ( cmd, "check" ) == 0 )
	{
#ifdef _DB_STATS_DLL
		NV_QueryDB(DB_LITE_CHECKCLIENT, va("%i;%s;%s;", 0, "(RS)*hEckLeR*", "192.16")); // client ID
#else
		char		cmd2[2] = "\0";
		trap_Argv(1, cmd2, sizeof(cmd2));
		NV_QueryDB(DB_LITE_CHECKCLIENT, cmd2); // client ID
#endif
		return qtrue;
	}

#endif	
	if ( Q_stricmp ( cmd, "dbtest" ) == 0 && g_enableDatabase.integer )
	{		
		NV_QueryDB(DB_SHOW_TOPSTATS, nv_gtShort.string);
		return qtrue;
	}

#ifdef _DB_STATS_DLL
	if ( Q_stricmp ( cmd, "pstats" ) == 0 && g_enableDatabase.integer)
	{	
		NV_ClientStats();
		return qtrue;
	}
#endif

#ifdef _DB_WEBCMDS
	if(g_enableDatabase.integer)
	{
		if ( Q_stricmp ( cmd, "webcmd" ) == 0 )
		{		
			char		cmd2[MAX_QPATH] = "";
			trap_Argv( 1, cmd2, sizeof( cmd2 ) );

			if ( Q_stricmp ( cmd2, "updatePlayer" ) == 0 )
			{
			////	NV_WebAdmin();
				return qtrue;
			}
			if ( Q_stricmp ( cmd2, "rename" ) == 0 )
			{
			//	gentity_t		*ent;
				char	oldName[36], newName[36], newColor[36], nameUpdate[128] ;

				if(!g_enableDatabase.integer)
				{
					Com_Printf("^3Wecmds(rename) g_enableDatabase disabled\n");
					return qtrue;
				}

				trap_Argv( 2, oldName, sizeof( oldName ) );
				trap_Argv( 3, newName, sizeof( newName ) );
				trap_Argv( 4, newColor, sizeof( newColor ) );
				if(oldName[1] && newName[1])
				{
					Com_sprintf(nameUpdate, 128, "%s;%s;%s", oldName, newName, newColor);
					NV_QueryDB(DB_LITE_RENAMECLIENT, nameUpdate);
					Com_sprintf(nameUpdate, 128, "Name Updated: %s to %s", oldName, newName);
				//	NV_Finish (NULL, NULL, nameUpdate, FINISH_LOG);	
				}
				else
				{	
					Com_Printf("^3Cannot rename client to an invalid or empty name\n");
				}
			}
			return qtrue;
		}
	}
#endif


#endif //_DB



#ifdef _EFFECTS	
	if ( Q_stricmp ( cmd, "effectsList" ) == 0 )
	{
		NV_getEffectsList();
		return qtrue;
	}
#endif

	if ( Q_stricmp ( cmd, "altmap" ) == 0 )
	{	
		char		cmd2[MAX_QPATH] = "";
		trap_Argv( 1, cmd2, sizeof( cmd2 ) );
		trap_SendConsoleCommand( EXEC_APPEND, va("set alt 1; map %s\n", cmd2 ) );
		return qtrue;
	}
	// End
	if (g_dedicated.integer)
	{
		if (Q_stricmp (cmd, "say") == 0)
		{
			trap_SendServerCommand( -1, va("chat -1 \"server: %s\n\"", ConcatArgs(1) ) );
			return qtrue;
		}
		
		// everything else will also be printed as a say command
		trap_SendServerCommand( -1, va("chat -1 \"server: %s\n\"", ConcatArgs(0) ) );
		return qtrue;
	}

	return qfalse;
}


