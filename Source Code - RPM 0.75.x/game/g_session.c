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
#include "g_local.h"

/*
=======================================================================
  SESSION DATA
Session data is the only data that stays persistant across level loads
and map restarts.
=======================================================================
*/

/*
================
G_WriteClientSessionData

Called on game shutdown
================
*/
void G_WriteClientSessionData( gclient_t *client ) 
{
	const char	*s;
	const char	*var;
	//Ryan june 7 2003
// GodOrDevil - #ClanList - Modded From RPM GOLD - 1.04.2005
// We want to save our special status so we dont have to check again when the map starts
	
#ifdef _CLANMEMBER
	s = va("%i %i %i %i", client->sess.team, client->nvSess.admin, client->nvSess.referee, client->sess.clanMember);
#else
	s = va("%i %i %i", client->sess.team, client->nvSess.admin, client->nvSess.referee );
#endif
	//s = va("%i", client->sess.team );

	//Ryan

	var = va( "session%i", client - level.clients );

	trap_Cvar_Set( var, s );
}

/*
================
G_ReadSessionData

Called on a reconnect
================
*/
void G_ReadSessionData( gclient_t *client ) 
{
	char		s[MAX_STRING_CHARS];
	const char	*var;
	int			sessionTeam;
	///Ryan june 7 2003
	int			adminSess;
	int			refSess;
	// GodOrDevil - #ClanList - 1.04.2005
#ifdef _CLANMEMBER
	int			clanSess;
#endif
	///End
	///Ryan

	var = va( "session%i", client - level.clients );
	trap_Cvar_VariableStringBuffer( var, s, sizeof(s) );

	///Ryan june 7 2003
#ifdef _CLANMEMBER 
	sscanf( s, "%i %i %i %i", &sessionTeam, &adminSess, &refSess, &clanSess); // GodOrDevil - 04.2005 #Clan
#else
	sscanf( s, "%i %i %i", &sessionTeam, &adminSess, &refSess );
#endif
	///Ryan

	/// bk001205 - format issues
	client->sess.team = (team_t)sessionTeam;
	///Ryan june 7 2003
	client->nvSess.admin = adminSess;
	client->nvSess.referee = refSess;
#ifdef _CLANMEMBER
	client->sess.clanMember = clanSess;		// GodOrDevil - 1.04.2005 - #ClanList
#endif
}


/*
================
G_InitSessionData

Called on a first-time connect
================
*/
void G_InitSessionData( gclient_t *client, char *userinfo ) 
{
	clientSession_t	*sess;
	const char		*value;
	

	sess = &client->sess;

	// initial team determination
	if ( level.gametypeData->teams ) 
	{
		if ( g_teamAutoJoin.integer ) 
		{
			sess->team = PickTeam( -1 );
			//Ryan We'll do this later 
			//BroadcastTeamChange( client, -1 );
			//Ryan
		} 
		else 
		{
			// always spawn as spectator in team games
			sess->team = TEAM_SPECTATOR;	
		}
	} 
	else 
	{
		value = Info_ValueForKey( userinfo, "team" );
		if ( value[0] == 's' ) 
		{
			// a willing spectator, not a waiting-in-line
			sess->team = TEAM_SPECTATOR;
		} 
		else 
		{
			if ( g_maxGameClients.integer > 0 && level.numNonSpectatorClients >= g_maxGameClients.integer ) 
			{
				sess->team = TEAM_SPECTATOR;
			} 
			else 
			{
				sess->team = TEAM_FREE;
			}
		}
	}

	sess->spectatorState = SPECTATOR_FREE;
	sess->spectatorTime = level.time;

	G_WriteClientSessionData( client );
}


/*
==================
G_InitWorldSession

==================
*/
void G_InitWorldSession( void ) 
{
	char	s[MAX_STRING_CHARS];
	int		gt;

	trap_Cvar_VariableStringBuffer( "session", s, sizeof(s) );
	
	gt = BG_FindGametype ( s );
	
	// if the gametype changed since the last session, don't use any
	// client sessions
	if ( level.gametype != gt ) 
	{
		level.newSession = qtrue;
		Com_Printf( "Gametype changed, clearing session data.\n" );
	}
	
	// GodOrDevil - Load files.  Do it here instead of G_InitGame(): because.. well, for whatever reason they arent being found.
	//... *MIGHT* be due to calculating the map list.
#ifdef _COMPMODE
	if(!g_compMode.integer) // skip all these in COMP MODE
#endif
	{
		NV_loadSettingFiles ();
	}
}

/*
==================
G_WriteSessionData
==================
*/
void G_WriteSessionData( void ) 
{
	int		i;

	trap_Cvar_Set( "session", level.gametypeData->name );

	for ( i = 0 ; i < level.maxclients ; i++ ) 
	{
		if ( level.clients[i].pers.connected == CON_CONNECTED ) 
		{
			G_WriteClientSessionData( &level.clients[i] );
		}
	}
}
