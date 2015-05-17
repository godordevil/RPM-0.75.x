
// Copyright (C) 2001-2002 Raven Software.
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

#include "../../ui/menudef.h"

// KRIS 15/04/2003 11:12PM
#ifdef _CRS_BOTS
qboolean zClientCommand(int clientNum);
#elif defined _SOF2_BOTS
int AcceptBotCommand(char *cmd, gentity_t *pl);
#endif
// KRIS

/*
==================
DeathmatchScoreboardMessage
==================
*/
void DeathmatchScoreboardMessage( gentity_t *ent )
{
	char		entry[1024];
	//Ryan
	char		string[1400];
	//char		string[2048];
	//Ryan
	int			stringlength;
	int			i, j;
	gclient_t	*cl;
	int			numSorted;

	//Ryan may 15 2004
	//dont let the scores update duing the awards
	if(level.awardTime)
	{
		return;
	}
	//Ryan

	// send the latest information on all clients
	string[0]    = 0;
	stringlength = 0;

	numSorted = level.numConnectedClients;

	for (i=0 ; i < numSorted ; i++)
	{
		int	ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->pers.connected == CON_CONNECTING )
		{
			ping = -1;
		}
		else
		{
			ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
		}
		
		//Ryan may 12 2004
		if(ent->client->sess.rpmClient > 0.5)
		{
			Com_sprintf (entry, sizeof(entry),
				" %i %i %i %i %i %i %i %i %i %.2f %i",
				level.sortedClients[i],
				cl->sess.score,
				cl->sess.kills,
				cl->sess.deaths,
				ping,			
				(level.time - cl->pers.enterTime)/60000,
				(cl->sess.ghost || cl->ps.pm_type == PM_DEAD) ? qtrue : qfalse,
				g_entities[level.sortedClients[i]].s.gametypeitems,	
				g_teamkillDamageMax.integer ? 100 * cl->sess.teamkillDamage / g_teamkillDamageMax.integer : 0,
				cl->pers.statinfo.accuracy,
				cl->pers.statinfo.headShotKills
				);
		}
		else
		{
			Com_sprintf (entry, sizeof(entry),
				" %i %i %i %i %i %i %i %i %i",
				level.sortedClients[i],
				cl->sess.score,
				cl->sess.kills,
				cl->sess.deaths,
				ping,
				(level.time - cl->pers.enterTime)/60000,
				(cl->sess.ghost || cl->ps.pm_type == PM_DEAD) ? qtrue : qfalse,
				g_entities[level.sortedClients[i]].s.gametypeitems,
				g_teamkillDamageMax.integer ? 100 * cl->sess.teamkillDamage / g_teamkillDamageMax.integer : 0
				);
		}
		//Ryan

		j = strlen(entry);
		if (stringlength + j >= 1000)
		{
			break;
		}

		strcpy (string + stringlength, entry);
		stringlength += j;
	}
	trap_SendServerCommand( ent-g_entities, va("scores %i %i %i%s", i,
							level.teamScores[TEAM_RED],
							level.teamScores[TEAM_BLUE],
											string ) );
}


/*
==================
Cmd_Score_f

Request current scoreboard information
==================
*/
void Cmd_Score_f( gentity_t *ent )
{
	DeathmatchScoreboardMessage( ent );
}

/*
==================
CheatsOk
==================
*/
qboolean	CheatsOk( gentity_t *ent ) {
#ifndef _DEBUG
	if ( !g_cheats.integer ) {
		trap_SendServerCommand( ent-g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return qfalse;
	}
#endif
	if ( ent->health <= 0 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"You must be alive to use this command.\n\""));
		return qfalse;
	}
	return qtrue;
}


/*
==================
ConcatArgs
==================
*/
char	*ConcatArgs( int start ) {
	int		i, c, tlen;
	static char	line[MAX_STRING_CHARS];
	int		len;
	char	arg[MAX_STRING_CHARS];

	len = 0;
	c = trap_Argc();
	for ( i = start ; i < c ; i++ ) {
		trap_Argv( i, arg, sizeof( arg ) );
		tlen = strlen( arg );
		// GodOrDevil -  Crash Fix
		//if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
		if ( len + tlen >= 896 ) {
		///end
			break;
		}
		memcpy( line + len, arg, tlen );
		len += tlen;
		if ( i != c - 1 ) {
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}

/*
==================
SanitizeString

Remove case and control characters
==================
*/
void SanitizeString( char *in, char *out ) {
	while ( *in ) {
		if ( *in == 27 ) {
			in += 2;		// skip color code
			continue;
		}
		if ( *in < 32 ) {
			in++;
			continue;
		}
		*out++ = tolower( *in++ );
	}

	*out = 0;
}

/*
==================
G_ClientNumberFromName

Finds the client number of the client with the given name
==================
*/
int G_ClientNumberFromName ( const char* name )
{
	char		s2[MAX_STRING_CHARS];
	char		n2[MAX_STRING_CHARS];
	int			i;
	gclient_t*	cl;

	// check for a name match
	SanitizeString( (char*)name, s2 );
	for ( i=0, cl=level.clients ; i < level.numConnectedClients ; i++, cl++ )
	{
		SanitizeString( cl->pers.netname, n2 );
		if ( !strcmp( n2, s2 ) )
		{
			return i;
		}
	}

	return -1;
}

/*
==================
ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int ClientNumberFromString( gentity_t *to, char *s ) {
	gclient_t	*cl;
	int			idnum;
	char		s2[MAX_STRING_CHARS];
	char		n2[MAX_STRING_CHARS];

	// numeric values are just slot numbers
	if (s[0] >= '0' && s[0] <= '9') {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			trap_SendServerCommand( to-g_entities, va("print \"Bad client slot: %i\n\"", idnum));
			return -1;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected != CON_CONNECTED ) {
			trap_SendServerCommand( to-g_entities, va("print \"Client %i is not active\n\"", idnum));
			return -1;
		}
		return idnum;
	}

	// check for a name match
	SanitizeString( s, s2 );
	for ( idnum=0,cl=level.clients ; idnum < level.maxclients ; idnum++,cl++ ) {
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		SanitizeString( cl->pers.netname, n2 );
		if ( !strcmp( n2, s2 ) ) {
			return idnum;
		}
	}

	trap_SendServerCommand( to-g_entities, va("print \"User %s is not on the server\n\"", s));
	return -1;
}

/*
==================
Cmd_Drop_f

Drops the currenty selected weapon
==================
*/
void Cmd_Drop_f ( gentity_t* ent )
{
	gentity_t* dropped;

	// spectators cant drop anything since they dont have anything
	if ( ent->client->sess.team == TEAM_SPECTATOR )
	{
		return;
	}

	// Ghosts and followers cant drop stuff
	if ( ent->client->ps.pm_flags & (PMF_GHOST|PMF_FOLLOW) )
	{
		return;
	}

	// Drop the weapon the client wanted to drop
	dropped = G_DropWeapon ( ent, atoi(ConcatArgs( 1 )), 3000 );
	if ( !dropped )
	{
		return;
	}
}

// GodOrDevil - 05.07.05 - 07:37pm - #DropGtItem
/*
==================
Cmd_DropItem_f

Drops the gametype items the player is carrying
==================
*/
void Cmd_DropItem_f ( gentity_t* ent )
{
	// spectators cant drop anything since they dont have anything
	if ( ent->client->sess.team == TEAM_SPECTATOR )
		return;

	// Ghosts and followers cant drop stuff
	if ( ent->client->ps.pm_flags & (PMF_GHOST|PMF_FOLLOW) )
		return;

	// Nothing to drop
	if ( !ent->client->ps.stats[STAT_GAMETYPE_ITEMS] )
		return;
	if(NV_isVIP())
		return;
	G_DropGametypeItems ( ent, 3000 );
}
//End  - 05.07.05 - 07:37pm

/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f (gentity_t *ent)
{
	char		*name;
	gitem_t		*it;
	int			i;
	qboolean	give_all;
	gentity_t		*it_ent;
	trace_t		trace;
	char		arg[MAX_QPATH];

	int start;
	int end;
	int l;

	trap_Argv( 1, arg, sizeof( arg ) );

	if ( !Q_stricmp ( arg, "me" ) )
	{
		start = ent->s.number;
		end = start + 1;
	}
	else if ( !Q_stricmp ( arg, "all" ) )
	{
		start = 0;
		end   = MAX_CLIENTS;
	}
	else
	{
		start = atoi ( arg );
		end = start + 1;
	}

	for ( l = start; l < end; l ++ )
	{
		ent = &g_entities[l];

		if ( !ent->inuse )
		{
			continue;
		}

		if ( G_IsClientDead ( ent->client ) )
		{
			continue;
		}

		if ( !CheatsOk( ent ) ) {
			return;
		}

	name = ConcatArgs( 2 );

	if (Q_stricmp(name, "all") == 0)
		give_all = qtrue;
	else
		give_all = qfalse;

	if (give_all || Q_stricmp( name, "health") == 0)
	{
		ent->health = MAX_HEALTH;
		if (!give_all)
			continue;
	}

	if (give_all || Q_stricmp(name, "weapons") == 0)
	{
		ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_NONE );
		if (!give_all)
			continue;
	}

	if (give_all || Q_stricmp(name, "ammo") == 0)
	{
		for ( i = WP_NONE + 1 ; i < WP_NUM_WEAPONS ; i++ )
		{
			attackType_t a;

			for ( a = ATTACK_NORMAL; a < ATTACK_MAX; a ++ )
			{
				ent->client->ps.clip[a][i] = weaponData[i].attack[a].clipSize;
				ent->client->ps.ammo[weaponData[i].attack[a].ammoIndex] = ammoData[weaponData[i].attack[a].ammoIndex].max;
			}
		}

		if (!give_all)
			continue;
	}

	if (give_all || Q_stricmp(name, "armor") == 0)
	{
		ent->client->ps.stats[STAT_REALARMOR] = MAX_ARMOR;

		if (!give_all)
			continue;
	}

	// spawn a specific item right on the player
	if ( !give_all )
	{
		it = BG_FindItem (name);
		if (!it)
		{
			continue;
		}

		if ( it->giType == IT_GAMETYPE )
		{
			continue;
		}

		it_ent = G_Spawn();
		VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
		it_ent->classname = it->classname;
		G_SpawnItem (it_ent, it);
		FinishSpawningItem(it_ent );
		memset( &trace, 0, sizeof( trace ) );
		Touch_Item (it_ent, ent, &trace);
		if (it_ent->inuse)
		{
			G_FreeEntity( it_ent );
		}
	}

	}
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f (gentity_t *ent)
{
	char	*msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE) )
		msg = "godmode OFF\n";
	else
		msg = "godmode ON\n";

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f( gentity_t *ent ) {
	char	*msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET) )
		msg = "notarget OFF\n";
	else
		msg = "notarget ON\n";

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f( gentity_t *ent ) {
	char	*msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	if ( ent->client->noclip ) {
		msg = "noclip OFF\n";
	} else {
		msg = "noclip ON\n";
	}
	ent->client->noclip = !ent->client->noclip;

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void Cmd_LevelShot_f( gentity_t *ent )
{
	if ( !CheatsOk( ent ) )
	{
		return;
	}

	BeginIntermission();

	trap_SendServerCommand( ent-g_entities, "clientLevelShot" );
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent )
{
	// No killing yourself if your a spectator
	if ( G_IsClientSpectating ( ent->client ) )
	{
		return;
	}

	// No killing yourself if your dead
	if ( G_IsClientDead ( ent->client ) )
	{
		return;
	}

	ent->flags &= ~FL_GODMODE;
	ent->client->ps.stats[STAT_HEALTH] = ent->health = -999;
	player_die (ent, ent, ent, 100000, MOD_SUICIDE, HL_NONE, vec3_origin );
}

/*
=================
BroadCastTeamChange

Let everyone know about a team change
=================
*/
void BroadcastTeamChange( gclient_t *client, int oldTeam )
{
	//Ryan june7 2003
	char	*adm = "";
	char	*clan = "";	// GodOrDevil - 1.05.2005 - #Clan Message at server join
	
	if(client->sess.motdStartTime)
	{
		return;
	}
// GodOrDevil - 1.12.2005 - Modified for #AdminLevels & added #hideAdmins
	if(client->sess.firstTime) {
		//if(!g_hideAdmins.integer){
		//	if(g_noRanks.integer && client->nvSess.admin > 1)   adm = "^6ADMIN:";
			/*else*/ if(client->nvSess.admin == 4)	adm = "^SS-ADMIN:";
			else if(client->nvSess.admin == 3)	adm = "^6ADMIN:";
			else if(client->nvSess.admin == 2)	adm = "^4B-ADMIN:";
#ifdef _RECRUITER
			else if(client->nvSess.admin == 5)	adm = "^3RECRUITER:";
#endif
		//}
		
#ifdef _CLANMEMBER
		if(client->sess.clanMember)		clan = "^_*^7Clan Member^_*\n";
#endif
	}

//End
// GodOrDevil - Changing the "Joined" Message to show #ClanMember Message
	switch ( client->sess.team ){	//use this so we can have longer names without cutting off

		case TEAM_RED:
			trap_SendServerCommand( -1, va("cp \"%s%s ^3%s\n^7Has joined the ^1RED ^7team.\n\"", clan, adm, client->pers.netname));
			break;
		case TEAM_BLUE:
			trap_SendServerCommand( -1, va("cp \"%s%s ^3%s\n^7Has joined the ^4BLUE ^7team.\n\"", clan, adm, client->pers.netname));
			break;
		case TEAM_SPECTATOR:
			if ( oldTeam != TEAM_SPECTATOR || adm[0] != '\0')
				trap_SendServerCommand( -1, va("cp \"%s%s ^3%s\n^7Has joined the ^7Spectators.\n\"", clan, adm, client->pers.netname));
			break;
		case TEAM_FREE:
			trap_SendServerCommand( -1, va("cp \"%s%s ^3%s\n^7Has joined the ^3Battle.\n\"", clan, adm, client->pers.netname));
			break;
	}
//OLD WAY

	//send the team change info to the gametypes instead so longer names that have colors
	//will still fit the whole message on the screen.
/*	else
	{
		int clientid = client - level.clients;

		if(oldTeam != client->sess.team)
		{
			trap_GT_SendEvent ( GTEV_TEAM_CHANGE, level.time, clientid, client->sess.team, -1, -1, 0 );
		}
	}

//ORIGINAL WAY

/*	switch ( client->sess.team )
	{
		case TEAM_RED:
			trap_SendServerCommand( -1, va("cp \"%s joined the red team.\n\"", client->pers.netname) );
			break;

		case TEAM_BLUE:
			trap_SendServerCommand( -1, va("cp \"%s joined the blue team.\n\"", client->pers.netname));
			break;

		case TEAM_SPECTATOR:
			if ( oldTeam != TEAM_SPECTATOR )
			{
				trap_SendServerCommand( -1, va("cp \"%s joined the spectators.\n\"", client->pers.netname));
			}
			break;

		case TEAM_FREE:
			trap_SendServerCommand( -1, va("cp \"%s joined the battle.\n\"", client->pers.netname));
			break;
	}
*/
	//Ryan
}

/*
=================
SetTeam
=================
*/
void SetTeam( gentity_t *ent, char *s, const char* identity )
{
	int					team;
	int					oldTeam;
	gclient_t			*client;
	int					clientNum;
	spectatorState_t	specState;
	int					specClient;
	qboolean			ghost;
   	qboolean			noOutfittingChange = qfalse;

//#ifdef _ZOMBIES
//	if(NV_isZom())
//	{
//		ZOM_SetTeam( ent, s, identity );
//		return;
//	}
//#endif
	// GodOrDevil - 02.04.05 - 09:18am #Pbox
	// Ryan Nov 20 2004
	// Don't let them change teams if they are in the penalty box
	// or if they are in the process of being warned.
	if(ent->client->sess.penaltySeconds 
#ifdef _TORTURE
		|| ent->client->sess.warnSeconds
#endif
		)
	{
		return;
	}

	//End - 02.04.05 - 09:19am

	// see what change is requested
	//
	client = ent->client;

	clientNum = client - level.clients;
	specClient = 0;
	specState = SPECTATOR_NOT;
		
	// GodOrDevil
	if(ent->client->nvSess.teamlock == 2)
	{	
		CPx(clientNum, "print \"You cannot change teams.  Admin Decision\n\"" );
		return;
	}
	// END

	// If an identity was specified then inject it into
	// the clients userinfo
	if ( identity )
	{
		char userinfo[MAX_INFO_STRING];
		// GodOrDevil - 08.30.06 - 09:02pm #Identity Limiter
		ent->client->sess.lastIdentityChange = level.time;
		///End  - 08.30.06 - 09:03pm
		trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

		if ( Q_stricmp ( identity, Info_ValueForKey ( userinfo, "identity" ) ) )
		{
			Info_SetValueForKey ( userinfo, "identity", identity );
			Info_SetValueForKey ( userinfo, "team_identity", identity );
			trap_SetUserinfo ( clientNum, userinfo );
		}
		else
		{
			identity = NULL;
		}
	}

	if ( !Q_stricmp( s, "follow1" ) )
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FOLLOW;
		specClient = -1;
	}
	else if ( !Q_stricmp( s, "follow2" ) )
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FOLLOW;
		specClient = -2;
	}
	else if ( !Q_stricmp( s, "spectator" ) || !Q_stricmp( s, "s" ) )
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FREE;
	}
#ifdef _ZOMBIES
	else if(NV_isZom())
	{
		if ( ent->client->nvSess.zombie )
		{
			team = TEAM_RED;
		}
		else
		{
			team = TEAM_BLUE;
		}
	}
#endif
	else if ( level.gametypeData->teams )
	{
		// if running a team game, assign player to one of the teams
		specState = SPECTATOR_NOT;
		if ( !Q_stricmp( s, "red" ) || !Q_stricmp( s, "r" ) )
		{
			team = TEAM_RED;
		}
		else if ( !Q_stricmp( s, "blue" ) || !Q_stricmp( s, "b" ) )
		{
			team = TEAM_BLUE;
		}
		else
		{
			// pick the team with the least number of players
			team = PickTeam( clientNum );
		}
		// GodOrDevil
		if(!ent->client->nvSess.teamlock)
		{
			if ( (g_teamForceBalance.integer == 2 || (g_teamForceBalance.integer == 3 &&  client->nvSess.admin < 4)) && ((level.time - level.startTime) >= 30000) )
			{
				team = NV_PickTeam(clientNum, (team_t)team, client->sess.team );
			}
			else
		//  
			if ( g_teamForceBalance.integer == 1 )
			{
				int		counts[TEAM_NUM_TEAMS];

				// Something is fucked here ...  Spectators show the error message to other clients?  But entering a clientNum in teamcount returns 0 for specs.. wtf!!
				if(client->sess.team != TEAM_SPECTATOR)
				{
					counts[TEAM_BLUE] = TeamCount( ent->client->ps.clientNum, TEAM_BLUE, NULL );
					counts[TEAM_RED] = TeamCount( ent->client->ps.clientNum, TEAM_RED, NULL );
				}
				else
				{
					counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE, NULL );
					counts[TEAM_RED] = TeamCount( -1, TEAM_RED, NULL );
				}
				if ( team == TEAM_RED && counts[TEAM_BLUE] < counts[TEAM_RED] && level.extras.blueLocked == 0)	
				{
					trap_SendServerCommand( ent->client->ps.clientNum, "cp \"Red team has too many players.\n\"" );
					team = TEAM_BLUE;
					return;
				}
				if (team == TEAM_BLUE && counts[TEAM_RED] < counts[TEAM_BLUE] && level.extras.redLocked == 0)	
				{
					trap_SendServerCommand( ent->client->ps.clientNum, "cp \"Blue team has too many players.\n\"" );
					team = TEAM_RED;			
					return;
				}		
				// It's ok, the team we are switching to has less or same number of players
			}
	
		} // End


	}
	else
	{
		// force them to spectators if there aren't any spots free
		team = TEAM_FREE;
	}

	// override decision if limiting the players
	if ( g_maxGameClients.integer > 0 && level.numNonSpectatorClients >= g_maxGameClients.integer )
	{
		team = TEAM_SPECTATOR;
	}

	// decide if we will allow the change
	oldTeam = client->sess.team;
	ghost   = client->sess.ghost;

	if ( team == oldTeam && team != TEAM_SPECTATOR )
	{
		if ( identity )
		{
			// get and distribute relevent paramters
			client->pers.identity = NULL;
			ClientUserinfoChanged( clientNum );
		}

		return;
	}
	
	// GodOrDevil - 03.05.05 - 07:19am	
	///Tortured cant switch
#ifdef _TORTURE
	if(client->nvSess.torture)
	{
		return ;
	}
#endif
	///End  - 03.05.05 - 07:19am

	//Ryan
	// GodOrDevil - 1.14.2005 - Dont need #Compmode to lock teams #NOTE - admins can join locked teams
//	if (g_compMode.integer && level.gametypeData->teams)
	if (level.gametypeData->teams && ent->client->nvSess.teamlock != 1)
	{
		// GodOrDevil - 03.05.05 - 07:19am
		///if ((team == TEAM_RED) && level.extras.redLocked)
		if ((team == TEAM_RED) && level.extras.redLocked && client->nvSess.admin < 2) {
			trap_SendServerCommand(clientNum, "cp \"^1Red ^7team has been ^3locked.\n\"" );
			return;
		}
		///else if ((team == TEAM_BLUE) && level.extras.blueLocked)
		else if ((team == TEAM_BLUE) && level.extras.blueLocked && client->nvSess.admin < 2) {
			trap_SendServerCommand(clientNum, "cp \"^4Blue ^7team has been ^3locked.\n\"" );
			return;
		}
	}

	if(oldTeam == TEAM_SPECTATOR)
	{
		//Ryan march 28 2004 3:24pm
		//keep track of how much time the player spent spectating
		//we'll subtract this later from the players time for awards
		client->sess.totalSpectatorTime += level.time - client->sess.spectatorTime;
		//Ryan

		//clear the invites if we are joining game play
		if((team == TEAM_RED || team == TEAM_BLUE) && ent->client->nvSess.teamlock != 1)
		{
			client->sess.invitedByRed = qfalse;
			client->sess.invitedByBlue = qfalse;
		}
	}
	//Ryan

   	noOutfittingChange = ent->client->noOutfittingChange;
	
	client->pers.ready = 0;

	// he starts at 'base'
	client->pers.teamState.state = TEAM_BEGIN;
	
	if ( oldTeam != TEAM_SPECTATOR )
	{
		if ( ghost )
		{
			G_StopGhosting ( ent );
		}
		else if ( !G_IsClientDead ( client ) )
		{
			// Kill him (makes sure he loses flags, etc)
			ent->flags &= ~FL_GODMODE;
			ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
			// GodOrDevil - 02.04.05 - 06:32am - #teamChangeDeath
			if ( g_teamChangeDeath.integer == 1 ){
				player_die (ent, ent, ent, 100000, MOD_TEAMCHANGE, HL_NONE, vec3_origin );}
			else {
				G_DropGametypeItems ( ent, 0);
				}
			//End - 02.04.05 - 06:35am


			ent->client->sess.ghost = qfalse;
		}
	}

	// If respawn interval start as a ghost
	if ( level.gametypeRespawnTime[ team ] )
	{
		ghost = qtrue;
	}

	// they go to the end of the line
	if ( team == TEAM_SPECTATOR )
	{
		client->sess.spectatorTime = level.time;
	}

	client->sess.team = (team_t)team;
	client->sess.spectatorState = specState;
	client->sess.spectatorClient = specClient;

	// GodOrDevil - GAMETYPE - remove frozen timers
	NV_freeFrozenModel(ent); // clear model
	ent->client->nvSess.frozen = 0;
	ent->client->nvSess.frozenTime = level.time;
	// END

	// Kill any child entities of this client to protect against grenade team changers
	G_FreeEnitityChildren ( ent );

	// Always spawn into a ctf game using a respawn timer.
	//Ryan june 15 2003		this will avoid problems during pause if ppl change teams
	if(!level.pause)
	{
		// GodOrDevil - GAMETYPES
		//if ( team != TEAM_SPECTATOR && level.gametypeData->respawnType == RT_INTERVAL )
		//{
		//	G_SetRespawnTimer ( ent );
		//	ghost = qtrue;
		//}
		if ( team != TEAM_SPECTATOR && (level.gametypeData->respawnType == RT_INTERVAL || NV_isSab() /*|| NV_isTFT()*/ || NV_isRaze())) //TFT #test
		{
			G_SetRespawnTimer ( ent );
			ghost = qtrue;		
		}
		else if (client->nvSess.frozenTime >= level.time && (NV_isFT() || NV_isTFT()))
		{
			//ent->client->ps.respawnTimer = level.time + (client->nvSess.frozenTime - level.time);	
			//ent->client->ps.respawnTimer = level.time + (g_freezeTime.value * 1000);	
			ent->client->ps.respawnTimer = level.time + (g_respawnInterval.value * 1000);	
			ghost = qtrue;
		}
		// END
	}
	//Ryan
	//Ryan june 15 2003
	if(!level.pause && !client->sess.firstTime)
	{
		BroadcastTeamChange( client, oldTeam );
	}
	//Ryan

	// See if we should spawn as a ghost
	// GodOrDevil - GAMETYPES
	//if ( team != TEAM_SPECTATOR && level.gametypeData->respawnType == RT_NONE )
#ifdef _ZOMBIES
	if ( team != TEAM_SPECTATOR && level.gametypeData->respawnType == RT_NONE && !NV_isSab() && !NV_isTFT() && !NV_isRaze() && !NV_isZom()) //TFT #TEST
#else
	if ( team != TEAM_SPECTATOR && level.gametypeData->respawnType == RT_NONE && !NV_isSab() && !NV_isTFT() && !NV_isRaze()) //TFT #TEST
#endif
		// END	
	{
		// If there are ghosts already then spawn as a ghost because
		// the game is already in progress.

		// GodOrDevil - 09.09.06 - 11:51pm #roundjointime
		///if ( (level.gametypeJoinTime && (level.time - level.gametypeJoinTime) > 20000) || noOutfittingChange || client->sess.noTeamChange )
		if ( !level.warmupTime && (level.gametypeJoinTime && (level.time - level.gametypeJoinTime) > (g_roundjointime.integer * 1000)) || noOutfittingChange || client->sess.noTeamChange )
		///End  - 09.09.06 - 11:52pm
		{
			ghost = qtrue;
		}

		// Spectator to a team doesnt count
		if ( oldTeam != TEAM_SPECTATOR )
		{
			client->sess.noTeamChange = qtrue;
		}
	}

	// If a ghost, enforce it
	if ( ghost )
	{
		// Make them a ghost again
		if ( team != TEAM_SPECTATOR )
		{
			G_StartGhosting ( ent );

			// get and distribute relevent paramters
			client->pers.identity = NULL;
			ClientUserinfoChanged( clientNum );

			CalculateRanks();

			return;
		}
	}

	// get and distribute relevent paramters
	client->pers.identity = NULL;
	ClientUserinfoChanged( clientNum );

	CalculateRanks();

	// Begin the clients new life on the their new team
	//Ryan april 10 2004 10:10am
	//ClientBegin( clientNum );
	ClientBegin( clientNum, qfalse );
	//Ryan
}

/*
=================
G_StartGhosting

Starts a client ghosting.  This essentially will kill a player which is alive
=================
*/
void G_StartGhosting ( gentity_t* ent )
{
	int i;

	// Dont start ghosting if already ghosting
	if ( ent->client->sess.ghost )
	{
		return;
	}
	ent->client->sess.ghost = qtrue;
	ent->client->sess.spectatorState = SPECTATOR_FREE;
	ent->client->sess.spectatorClient = -1;
	ent->client->ps.pm_flags |= PMF_GHOST;
	ent->client->ps.stats[STAT_HEALTH] = 100;
	ent->client->ps.pm_type = PM_SPECTATOR;
	ent->client->ps.pm_flags &= ~PMF_FOLLOW;
	trap_UnlinkEntity (ent);

	// stop any following clients
	for ( i = 0 ; i < level.maxclients ; i++ )
	{
		if ( G_IsClientSpectating ( &level.clients[i] )
			&& level.clients[i].sess.spectatorState == SPECTATOR_FOLLOW
			&& level.clients[i].sess.spectatorClient == ent->s.number )
		{
			G_StopFollowing( &g_entities[i] );
		}
	}
}

/*
=================
G_StopGhosting

Stops a client from ghosting.  The client will be dead after this
call
=================
*/
void G_StopGhosting ( gentity_t* ent )
{
	// Dont stop someone who isnt ghosting in the first place
	if ( !ent->client->sess.ghost )
	{
		return;
	}

	ent->client->sess.ghost = qfalse;
	ent->client->ps.pm_flags &= ~PMF_GHOST;
	ent->client->ps.pm_flags &= ~PMF_FOLLOW;

	if ( ent->client->sess.team == TEAM_SPECTATOR )
	{
		ent->client->ps.pm_type = PM_SPECTATOR;
	}
	else
	{
		ent->client->ps.pm_type = PM_DEAD;
		ent->health = ent->client->ps.stats[STAT_HEALTH] = 0;
	}
}

/*
=================
G_StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void G_StopFollowing( gentity_t *ent )
{
	// Cant stop following if not following in the first place
	if ( !(ent->client->ps.pm_flags&PMF_FOLLOW) )
	{
		return;
	}

	// Clear the following variables
	ent->client->ps.pm_flags &= ~PMF_FOLLOW;
	ent->client->sess.spectatorState = SPECTATOR_FREE;
	ent->client->ps.clientNum = ent - g_entities;
	ent->client->ps.zoomFov = 0;
	ent->client->ps.loopSound = 0;
	ent->client->ps.pm_flags &= ~(PMF_GOGGLES_ON|PMF_ZOOM_FLAGS);
	ent->client->ps.persistant[PERS_TEAM] = ent->client->sess.team;

	// If we were in fact following someone, then make the angles and origin nice for
	// when we stop
   	if ( ent->client->sess.spectatorClient != -1 )
   	{
   		gclient_t* cl = &level.clients[ent->client->sess.spectatorClient];

   		int i;
   		for ( i = 0; i < 3; i ++ )
		{
   			ent->client->ps.delta_angles[i] = ANGLE2SHORT(cl->ps.viewangles[i] - SHORT2ANGLE(ent->client->pers.cmd.angles[i]));
		}

   		VectorCopy ( cl->ps.viewangles, ent->client->ps.viewangles );
   		VectorCopy ( cl->ps.origin, ent->client->ps.origin );
   		VectorClear ( ent->client->ps.velocity );
   		ent->client->ps.movementDir = 0;

   		BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, qtrue );
   	}

	// Ghots dont really become spectators, just psuedo spectators
	if ( ent->client->sess.ghost )
	{
		// Do a start and stop to ensure the variables are all set properly
		G_StopGhosting ( ent );
		G_StartGhosting ( ent );
	}
	else
	{
		ent->client->sess.team = TEAM_SPECTATOR;	
		ent->client->ps.persistant[ PERS_TEAM ] = TEAM_SPECTATOR;	
	}
}

/*
=================
Cmd_Team_f
=================
*/
void Cmd_Team_f( gentity_t *ent )
{
	char team[MAX_TOKEN_CHARS];
	char identity[MAX_TOKEN_CHARS];

	// Need at least the team specified in the arguments
	if ( trap_Argc() < 2 )
	{
		int oldTeam = ent->client->sess.team;
		switch ( oldTeam )
		{
			case TEAM_BLUE:
				trap_SendServerCommand( ent-g_entities, "print \"Blue team\n\"" );
				break;

			case TEAM_RED:
				trap_SendServerCommand( ent-g_entities, "print \"Red team\n\"" );
				break;

			case TEAM_FREE:
				trap_SendServerCommand( ent-g_entities, "print \"Free team\n\"" );
				break;

			case TEAM_SPECTATOR:
				trap_SendServerCommand( ent-g_entities, "print \"Spectator team\n\"" );
				break;
		}

		return;
	}

	// Limit how often one can switch team  // GodOrDevil - Admins can switch more often!
#ifndef _DEBUG
	if ( ent->client->switchTeamTime > level.time && !ent->client->nvSess.admin )
	//if ( ent->client->switchTeamTime > level.time )
	{
		trap_SendServerCommand( ent-g_entities, "print \"May not switch teams more than once per 5 seconds.\n\"" );
		return;
	}
#endif

	trap_Argv( 1, team, sizeof( team ) );
	trap_Argv( 2, identity, sizeof( identity ) );

	SetTeam( ent, team, identity[0]?identity:NULL );

	// Remember the team switch time so they cant do it again really quick
	ent->client->switchTeamTime = level.time + 5000;
}


/*
=================
Cmd_Follow_f
=================
*/
void Cmd_Follow_f( gentity_t *ent )
{
	int		i;
	char	arg[MAX_TOKEN_CHARS];

	// GodOrDevil - 02.04.05 - 09:19am #Pbox
	// Ryan Nov 20 2004
	// Don't allow them to follow if they are in the penalty box
	if(ent->client->sess.penaltySeconds){
		return;
	}
	//End - 02.04.05 - 09:19am

	if ( trap_Argc() != 2 )
	{
		if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
		{
			G_StopFollowing( ent );
		}
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	i = ClientNumberFromString( ent, arg );
	if ( i == -1 )
	{
		return;
	}

	// can't follow self
	if ( &level.clients[ i ] == ent->client )
	{
		return;
	}

	// cant cycle to dead people
	if ( level.clients[i].ps.pm_type == PM_DEAD )
	{
		return;
	}

	// can't follow another spectator
	if ( G_IsClientSpectating ( &level.clients[ i ] ) )
	{
		return;
	}

	// Ryan Oct 24 2004
	if(ent->client->sess.team == TEAM_SPECTATOR && level.extras.specsLocked)
	{
		if(level.clients[ i ].sess.team == TEAM_RED && !ent->client->sess.invitedByRed)
		{
			return;
		}
		if(level.clients[ i ].sess.team == TEAM_BLUE && !ent->client->sess.invitedByBlue)
		{
			return;
		}
	}

	// Dissallow following of the enemy if the cvar is set
	//if ( level.gametypeData->teams && !g_followEnemy.integer && ent->client->sess.team != TEAM_SPECTATOR )
#ifdef _COMPMODE
	if ( level.gametypeData->teams && (!g_followEnemy.integer || g_compMode.integer) &&
		ent->client->sess.team != TEAM_SPECTATOR && !ent->client->adminspec)
#else
	if ( level.gametypeData->teams && !g_followEnemy.integer &&
		ent->client->sess.team != TEAM_SPECTATOR && !ent->client->adminspec)
#endif
	{
		// Are they on the same team?
		if ( level.clients[ i ].sess.team != ent->client->sess.team )
		{
			return;
		}
	}
	// Ryan

	// first set them to spectator as long as they arent a ghost
	if ( !ent->client->sess.ghost && ent->client->sess.team != TEAM_SPECTATOR )
	{
		SetTeam( ent, "spectator", NULL );
	}


	ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
	ent->client->sess.spectatorClient = i;
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent, int dir )
{
	int		clientnum;
	int		deadclient;
	int		original;

	// GodOrDevil - 02.04.05 - 09:20am #Pbox 
	// Ryan Nov 20 2004
	// If in the penalty box dont allow them to follow
	if(ent->client->sess.penaltySeconds){
		return;
	}
	//End - 02.04.05 - 09:20am

	// first set them to spectator
	if ( !ent->client->sess.ghost && ent->client->sess.team != TEAM_SPECTATOR )
	{
		SetTeam( ent, "spectator", NULL );
	}

	if ( dir != 1 && dir != -1 )
	{
		Com_Error( ERR_FATAL, "Cmd_FollowCycle_f: bad dir %i", dir );
	}

	if ( ent->client->sess.spectatorClient == -1 )
	{
		clientnum = original = ent->s.number;
	}
	else
	{
		clientnum = original = ent->client->sess.spectatorClient;
	}

	deadclient = -1;
	do
	{
		clientnum += dir;
		if ( clientnum >= level.maxclients )
		{
			clientnum = 0;
		}
		if ( clientnum < 0 )
		{
			clientnum = level.maxclients - 1;
		}

		// can only follow connected clients
		if ( level.clients[ clientnum ].pers.connected != CON_CONNECTED )
		{
			continue;
		}

		// can't follow another spectator
		if ( G_IsClientSpectating ( &level.clients[ clientnum ] ) )
		{
			continue;
		}

		// Cant switch to dead people unless there is nobody else to switch to
		if ( G_IsClientDead ( &level.clients[clientnum] ) )
		{
			deadclient = clientnum;
			continue;
		}
		//Ryan
#ifdef _COMPMODE
		if(g_compMode.integer && !ent->client->adminspec && !ent->client->nvSess.referee)
		{
			//No enemy following in compmode
			if(ent->client->sess.team != TEAM_SPECTATOR && level.clients[ clientnum ].sess.team != ent->client->sess.team)
			{
				continue;
			}

			if(ent->client->sess.team == TEAM_SPECTATOR)
			{
				if(level.clients[ clientnum ].sess.team == TEAM_RED)
				{
					if(level.extras.specsLocked && !ent->client->sess.invitedByRed)
					{
						continue;
					}
				}
				else if(level.clients[ clientnum ].sess.team == TEAM_BLUE)
				{
					if(level.extras.specsLocked && !ent->client->sess.invitedByBlue)
					{
						continue;
					}
				}
			}
		}
#endif
		//Ryan

		// Dissallow following of the enemy if the cvar is set
		//Ryan june 7 2003
			//if ( level.gametypeData->teams && !g_followEnemy.integer && ent->client->sess.team != TEAM_SPECTATOR )
		if ( level.gametypeData->teams && !g_followEnemy.integer && ent->client->sess.team != TEAM_SPECTATOR && !ent->client->adminspec)
		//Ryan
		{
			// Are they on the same team?
			if ( level.clients[ clientnum ].sess.team != ent->client->sess.team )
			{
				continue;
			}
		}

		// this is good, we can use it
		ent->client->sess.spectatorClient = clientnum;
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		return;

	} while ( clientnum != original );

	// If being forced to follow and there is a dead client to jump to, then jump to them now
	//Ryan
#ifdef _COMPMODE
	if ( deadclient != -1 && (g_forceFollow.integer || g_compMode.integer))
#else	
	if ( deadclient != -1 && g_forceFollow.integer )
#endif
	//Ryan
	{
		// this is good, we can use it
		ent->client->sess.spectatorClient = deadclient;
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		return;
	}

   	G_StopFollowing( ent );

	// leave it where it was
}

/*
==================
G_SayTo
==================
*/
static void G_SayTo( gentity_t *ent, gentity_t *other, int mode, const char *name, const char *message )
{
	qboolean	 ghost = qfalse;
	qboolean	 spec  = qfalse;
	const char*  type;

	if (!other)
	{
		return;
	}

	if (!other->inuse)
	{
		return;
	}

	if (!other->client)
	{
		return;
	}

	if ( other->client->pers.connected != CON_CONNECTED )
	{
		return;
	}

	if ( mode == SAY_TEAM  && !OnSameTeam(ent, other) )
	{
		return;
	}
	//Ryan if ref or admin only and they are not ref or admin dont send

	if ( mode == ADM_CHAT && !other->client->nvSess.admin )
		return;
	///if ( mode == (ADM_CHAT | AW_CHAT | AI_CHAT | AR_CHAT | AN_CHAT | AS_CHAT) && !other->client->nvSess.admin )
	///	return;
	if (mode == REF_CHAT && !other->client->nvSess.referee && !other->client->nvSess.admin)
		return;
	//Ryan
	// GodOrDevil - 05.28.06 - 08:35pm #adminChat
	if ( mode == SADM_CHAT  && other->client->nvSess.admin != 4 )
		return;
#ifdef _CLANMEMBER
	if (mode == CLAN_CHAT && !other->client->sess.clanMember )
		return;
#endif
	if (mode == CADM_CHAT && other->client->nvSess.admin < 2 )
		return;
	///End  - 05.28.06 - 08:37pm

	if ( !level.intermissiontime && !level.intermissionQueued )
	{
		// Spectators cant talk to alive people
		if ( ent->client->sess.team == TEAM_SPECTATOR )
		{
			spec = qtrue;
		}
				
		// GodOrDevil - GAMETYPES
		//if ( level.gametypeData->respawnType == RT_NONE )
		if ( level.gametypeData->respawnType == RT_NONE && !NV_isSab() && !NV_isFT() && !NV_isTFT() && !NV_isRaze())
		// END
		{
			// Dead people cant talk to alive people
			if ( !spec && G_IsClientDead ( ent->client ) )
			{
				ghost = qtrue;
			}

			//Ryan ignore rules if admin or ref is sending a message
			//if(mode == ADM_CHAT || mode == ADM_TALK || mode == REF_CHAT || mode == REF_TALK)
			if(mode >= ADM_TALK && mode <= AS_CHAT)
			{
				ghost = qfalse;
				spec = qfalse;
			}
			//Ryan
			
			// GodOrDevil - 11.04.05 - 01:57am #talkingGhosts
			if(!g_talkingGhosts.integer){
			///End  - 11.04.05 - 02:22am

				// If the client we are talking to is alive then a check
				// must be made to see if this talker is alowed to speak to this person
				if ( ent->s.number != other->s.number && !G_IsClientDead ( other->client ) && !G_IsClientSpectating( other->client) && (ghost || spec))
				{
					return;
				}
			}// GodOrDevil - 11.04.05 - 01:57am
		}
	}

	type = "";
	if ( ghost )
	{
		type = "*ghost* ";
	}
	else if ( spec )
	{
		type = "*spec* ";
	}
	//Ryan
	//if (mode == ADM_CHAT)
	//{
	//	type = "**Admins only** ";
	//}
	//else if (mode == ADM_TALK)
	//{
	//	type = "**Admin** ";
	//}
	//else if (mode == REF_CHAT)
	//{
	//	type = "**Refs only** ";
	//}
	//else if (mode == REF_TALK)
	//{
	//	type = "**Referee** ";
	//}
	//Ryan
	// GodOrDevil - 05.28.06 - 08:33pm #adminChat
	switch(mode)
	{
	case ADM_CHAT:
		type = g_adminChat.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/menus/invalid.wav"));
		break;
	case ADM_TALK:
		type = g_adminTalk.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/confused/sec_camera2.mp3"));
		break;
	case REF_CHAT:
		type = g_refChat.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/menus/invalid.wav"));
		break;
	case REF_TALK:
		type = g_refTalk.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/confused/sec_camera2.mp3"));
		break;
#ifdef _CLANMEMBER
	case CLAN_CHAT:
		type = g_clanChat.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/menus/invalid.wav"));
		break;
	case CLAN_TALK:
		type = g_clanTalk.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/confused/sec_camera2.mp3"));
		break;
#endif
	case SADM_CHAT:
		type = g_sadminChat.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/menus/invalid.wav"));
		break;
	case CADM_CHAT:
		type = g_caChat.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/menus/invalid.wav"));
		break;
// GodOrDevil
	case AW_CHAT:
		type = g_adminWarning.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/confused/sec_camera2.mp3"));
		trap_SendServerCommand( other-g_entities, va("chat %d \"%s^7 %s\"", ent->s.number, type, message));
		return;
	case AI_CHAT:
		type = g_adminInfo.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/confused/sec_camera2.mp3"));
		trap_SendServerCommand( other-g_entities, va("chat %d \"%s^7 %s\"", ent->s.number, type, message));
		return;
	case AR_CHAT:	
		type = g_adminRule.string;
		RPM_ClientSound(other, G_SoundIndex("sound/misc/confused/sec_camera2.mp3"));
		trap_SendServerCommand( other-g_entities, va("chat %d \"%s^7 %s\"", ent->s.number, type, message));
		return;
	case AN_CHAT:
		type = g_adminNotify.string;
		///RPM_ClientSound(other, G_SoundIndex("sound/misc/menus/invalid.wav"));
		trap_SendServerCommand( other-g_entities, va("chat %d \"%s^7%s\"", ent->s.number, type, message));
		return;
		///
	default:
		break;
	}
	///End  - 05.28.06 - 08:33pm

	trap_SendServerCommand( other-g_entities, va("%s %d \"%s%s%s\"",
							mode == SAY_TEAM ? "tchat" : "chat",
							ent->s.number,
							type, name, message));
}

/*
==================
G_GetChatPrefix
==================
*/
void G_GetChatPrefix ( gentity_t* ent, gentity_t* target, int mode, char* name, int nameSize )
{
	const char* namecolor;
	char		location[64];
	qboolean	locationOk = qtrue;

	// Spectators and ghosts dont show locations
	if ( ent->client->ps.pm_type == PM_DEAD || G_IsClientSpectating ( ent->client ) )
	{
		locationOk = qfalse;
	}

	if ( !level.gametypeData->teams && mode == SAY_TEAM )
	{
		mode = SAY_ALL;
	}

	if ( level.gametypeData->teams )
	{
		switch ( ent->client->sess.team )
		{
			case TEAM_BLUE:
				namecolor = S_COLOR_BLUE;
				break;

			case TEAM_RED:
				namecolor = S_COLOR_RED;
				break;

			default:
				namecolor = S_COLOR_WHITE;
				break;
		}
	}
	else
	{
		namecolor = S_COLOR_WHITE;
	}

	switch ( mode )
	{
		default:
		case SAY_ALL:
			//Ryan april 23 2003

			//change all of the name pointers to the new name variable "talkname"
			//so that when they talk their name is printed in the color of their team.

			//Com_sprintf (name, nameSize, "%s%s%s: ", namecolor, ent->client->pers.netname, S_COLOR_WHITE );
			Com_sprintf (name, nameSize, "%s%s%s: ", namecolor, ent->client->pers.talkname, S_COLOR_WHITE );
			//Ryan
			break;

		case SAY_TEAM:
			if ( locationOk && Team_GetLocationMsg(ent, location, sizeof(location)))
			{
				Com_sprintf ( name, nameSize, "%s(%s%s) %s(%s): ",
							  namecolor,
							  //Ryan april 23 2003
							  ent->client->pers.talkname,
							  //ent->client->pers.netname,
							  //Ryan
							  namecolor,
							  S_COLOR_WHITE, location );
			}
			else
			{
				Com_sprintf ( name, nameSize, "%s(%s%s)%s: ",
							  namecolor,
							  //Ryan april 23 2003
							  ent->client->pers.talkname,
							  //ent->client->pers.netname,
							  //Ryan
							  namecolor,
							  S_COLOR_WHITE );
			}
			break;		
		
		/// GodOrDevil - 03.27.05 - 01:40pm #PlayChat
		//case PLAY_CHAT: 
		//	{
		//		Com_sprintf ( name, nameSize, "%s(%s%s)%s: ",
		//						namecolor,
		//						ent->client->pers.talkname,
		//						namecolor,
		//						S_COLOR_WHITE );
		//	}
		//	break;
		////End  - 03.27.05 - 01:53pm

		case SAY_TELL:

			if ( locationOk && target && level.gametypeData->teams   &&
				 target->client->sess.team == ent->client->sess.team  &&
				 Team_GetLocationMsg(ent, location, sizeof(location))    )
			{
				Com_sprintf ( name, nameSize, "%s[%s%s] %s(%s): ",
							  namecolor,
							  //Ryan april 23 2003
							  ent->client->pers.talkname,
							  //ent->client->pers.netname,
							  //Ryan
							  namecolor,
							  S_COLOR_WHITE, location );
			}
			else
			{
				Com_sprintf ( name, nameSize, "%s[%s%s]%s: ",
							  namecolor,
							  //Ryan april 23 2003
							  ent->client->pers.talkname,
							  //ent->client->pers.netname,
							  //Ryan
							  namecolor,
							  S_COLOR_WHITE );
			}
			break;
		case ADM_TALK:
		case ADM_CHAT:
		case REF_TALK:
		case REF_CHAT:
		case SADM_CHAT:
#ifdef _CLANMEMBER
		case CLAN_CHAT:
		case CLAN_TALK:
#endif
		case CADM_CHAT:
			Com_sprintf ( name, nameSize, "%s%s%s: ",
							 namecolor,
							 ent->client->pers.talkname,
							 S_COLOR_WHITE );
			break;
	}

	strcat ( name, S_COLOR_GREEN );
}

/*
==================
G_Say
==================
*/
void G_Say ( gentity_t *ent, gentity_t *target, int mode, const char *chatText )
{
	int			j;
	gentity_t	*other;
	char		text[MAX_SAY_TEXT];
	char		name[128];
	char		string[128];
	
	//if(level.awardTime && level.time >= level.awardTime + 15000)
	//	return;

	// Logging stuff
	switch ( mode )
	{
		case SAY_ALL:
			G_LogPrintf( "say: %s: %s\n", ent->client->pers.netname, chatText );
			break;

		case SAY_TEAM:
			G_LogPrintf( "sayteam: %s: %s\n", ent->client->pers.netname, chatText );
			break;
			//Ryan
		case ADM_TALK:
			G_LogPrintf( "adminsay: %s: %s\n", ent->client->pers.netname, chatText );
			break;
		case ADM_CHAT:
			G_LogPrintf( "adminchat: %s: %s\n", ent->client->pers.netname, chatText );
			break;
		case REF_TALK:
			G_LogPrintf( "refsay: %s: %s\n", ent->client->pers.netname, chatText );
			break;
		case REF_CHAT:
			G_LogPrintf( "refchat: %s: %s\n", ent->client->pers.netname, chatText );
			break;
#ifdef _CLANMEMBER
		case CLAN_CHAT:
			G_LogPrintf( "clanchat: %s: %s\n", ent->client->pers.netname, chatText );
			break;
		case CLAN_TALK:
			G_LogPrintf( "clantalk: %s: %s\n", ent->client->pers.netname, chatText );
			break;
#endif
		case CADM_CHAT:
			G_LogPrintf( "tellAdmin: %s: %s\n", ent->client->pers.netname, chatText );
			break;
		case SADM_CHAT:
			G_LogPrintf( "sadminchat: %s: %s\n", ent->client->pers.netname, chatText );
			break;
		case AW_CHAT:
			G_LogPrintf( "!AW: %s: %s\n", ent->client->pers.netname, chatText );
			Com_sprintf( string, sizeof(string),"!AW from %s", ent->client->pers.netname);
			NV_adminPrint(string);
			break;
		case AI_CHAT:
			G_LogPrintf( "!AI: %s: %s\n", ent->client->pers.netname, chatText );
			Com_sprintf( string, sizeof(string),"!AI from %s", ent->client->pers.netname);
			NV_adminPrint(string);
			break;

		case AR_CHAT:
			G_LogPrintf( "!AR: %s: %s\n", ent->client->pers.netname, chatText );
			Com_sprintf( string, sizeof(string),"!AR from %s", ent->client->pers.netname);
			NV_adminPrint(string);
			break;
		case AN_CHAT:
			G_LogPrintf( "!AN: %s: %s\n", ent->client->pers.netname, chatText );
			Com_sprintf( string, sizeof(string),"!AN from %s", ent->client->pers.netname);
			NV_adminPrint(string);
			break;
		case AS_CHAT:
			G_LogPrintf( "!AS: %s: %s\n", ent->client->pers.netname, chatText );
			Com_sprintf( string, sizeof(string),"!AS from %s", ent->client->pers.netname);
			NV_adminPrint(string);
			break;
			//Ryan
	}

	// Generate the chat prefix
	G_GetChatPrefix ( ent, target, mode, name, sizeof(name) );

	// Save off the chat text
	Q_strncpyz( text, chatText, sizeof(text) );

	if ( target )
	{
		G_SayTo( ent, target, mode, name, text );
		return;
	}

	// echo the text to the console
	if ( g_dedicated.integer )
	{
		Com_Printf( "%s%s\n", name, text);
	}

	// send it to all the apropriate clients
	for (j = 0; j < level.numConnectedClients; j++)
	{
		other = &g_entities[level.sortedClients[j]];
		G_SayTo( ent, other, mode, name, text );
	}
	
	// GodOrDevil #keyWords for server spam and taunts
//#ifdef _CHAT_KEYWORDS
#ifdef _COMPMODE
	if(g_compMode.integer == 0 && ent->client && mode <= 3 && g_enableChatReponses.integer)
#else
	if(ent->client && mode <= 3 && g_enableChatReponses.integer)
#endif
	{
		NV_ServerKeyWords (ent, text);	
	}
//#endif //_CHAT_KEYWORDS


	//END
}


/*
==================
Cmd_Say_f
==================
*/
void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 ) {

///CJJ - 1.1.2005 - Chat triggers
	char		*p;
	//int			j;

	// GodOrDevil - 01.21.06 - 10:10pm - removed.
	///qboolean	chatx0rs = qfalse;

	///CJJ - 1.1.2005 - #END
	if ( trap_Argc () < 2 && !arg0 )
		return;
	if (arg0)
		p = ConcatArgs( 0 );
	///Ryan
	// GodOrDevil - 03.03.05 - 04:34am #Forcsay Shit
	///else if (mode == ADM_CHAT || mode == ADM_TALK || mode == REF_CHAT || mode == REF_TALK)
	///else if (mode == ADM_CHAT || mode == ADM_TALK || mode == REF_CHAT || mode == REF_TALK || mode == 10 /*|| mode == PLAY_CHAT*/)
	///CJJ - 1.1.2005 - Had to add this for the forcesay command to remove the clientid and the forcesay command from the string
	///else if (mode == 11 /*|| mode == PLAY_CHAT*/)
	///	p = ConcatArgs( 3 );
	///CJJ - 1.1.2005 - #END
	///End - 03.03.05 - 04:35am 
	else if(mode >= ADM_TALK && mode <= CADM_CHAT)
		p = ConcatArgs( 2 );
	else
		p = ConcatArgs( 1 );
	
	if (*p == '!') {
		p++;
		switch(*p)
		{
		case 'a':
		case 'A':
			p++;
			switch(*p)
			{
///
			case 'w':
			case 'W':
				if(ent->client->nvSess.admin < 2){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE ADMIN POWERS^7!\n\""));
					return;
				}
				mode = AW_CHAT;
				p++;
				if(*p == ' ')
					p++;
				break;
			case 'i':
			case 'I':
				if(ent->client->nvSess.admin < 2){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE ADMIN POWERS^7!\n\""));
					return;
				}
				mode = AI_CHAT;
				p++;
				if(*p == ' ')
					p++;
				break;
			case 'r':
			case 'R':
				if(ent->client->nvSess.admin < 2){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE ADMIN POWERS^7!\n\""));
					return;
				}
				mode = AR_CHAT;
				p++;
				if(*p == ' ')
					p++;
				break;
			case 'n':
			case 'N':
				if(ent->client->nvSess.admin < 2){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE ADMIN POWERS^7!\n\""));
					return;
				}
				mode = AN_CHAT;
				p++;
				if(*p == ' ')
					p++;
				break;
///
			case 'c':
			case 'C':
				if(ent->client->nvSess.admin < 2){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE ADMIN POWERS^7!\n\""));
					return;
				}
				p++;
				if(*p){
					if(*p == ' ')
						p++;
					mode = ADM_CHAT;
				}
				else {p--;p--;p--;}
				break;
			case 'T':
			case 't':
				if(ent->client->nvSess.admin < 2){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE ADMIN POWERS^7!\n\""));
					return;
				}
				p++;
				if(*p){
					if(*p == ' ')
						p++;
					mode = ADM_TALK;
				}
				else {p--;p--;p--;}
				break;
			default:
				p--;
				p--;
				break;
			}
			break;
		case 's':
		case 'S':
			p++;
			switch(*p)
			{
			case 'c':
			case 'C':
				if(ent->client->nvSess.admin != 4){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE S-ADMIN POWERS^7!\n\""));
					return;
				}
				p++;
				if(*p){
					if(*p == ' ')
						p++;
					mode = SADM_CHAT;
				}
				else {p--;p--;p--;}
				break;
			default:
				p--;
				p--;
				break;
			}
			break;
#ifdef _COMPMODE
		case 'r':
		case 'R':
			p++;
			switch(*p)
			{
			case 'c':
			case 'C':
				if(!ent->client->nvSess.referee){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE REFEREE POWERS^7!\n\""));
					return;
				}
				p++;
				if(*p){
					if(*p == ' ')
						p++;
					mode = REF_CHAT;
				}
				else {p--;p--;p--;}
				break;
			case 'T':
			case 't':
				if(!ent->client->nvSess.referee){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE REFEREE POWERS^7!\n\""));
					return;
				}
				p++;
				if(*p){
					if(*p == ' ')
						p++;
				mode = REF_TALK;
				}
				else {p--;p--;p--;}
				break;
			default:
				if (g_compMode.integer && ent->client->sess.team != TEAM_SPECTATOR && !ent->client->pers.ready)
				{
					RPM_ReadyUp( ent );
					//break;
				}
				p--;
				p--;
				break;
			}
			break;
		case 't':
		case 'T':
			p++;
			switch(*p)
			{
			case 'i':
			case 'I':
				RPM_Timein(ent);
				p--;
				p--;
				break;
			case 'o':
			case 'O':
				if (g_compMode.integer)
				{		
					if(!ent->client->nvSess.referee)
						RPM_Timeout(ent, qfalse);
					else
						RPM_Timeout(ent, qtrue);
				}
				p--;
				p--;
				break;
			default:
				p--;
				p--;
				break;
			}
			break;
#endif
		case 'b':
		case 'B':
			p++;
			switch(*p)
			{
			case 'c':
			case 'C':
				if(ent->client->nvSess.admin < 3){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE ADMIN POWERS^7!\n\""));
					return;
				}
				p++;
				if(*p == ' ') p++;
				RPM_Tokens(ent, p, mode);
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^6*^3*^7 %s ^3*^6*", level.time + 7000,	p));
				RPM_GlobalSound(G_SoundIndex("sound/misc/c4/beep.mp3"));
				G_LogPrintf( "!bc: %s: %s\n", ent->client->pers.netname, p );
				{	
					char	string[128];
					Com_sprintf( string, sizeof(string),"!BC from %s", ent->client->pers.netname);
					NV_adminPrint(string);
				}

				return;
			default:
				p--;
				p--;
				break;
			}
			break;
		case 'c':
		case 'C':
			//p++;
			//switch(*p)
			//{
			//case 'c':
			//case 'C':
			//	if(!ent->client->sess.clanMember){
			//		trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                    ^3YOU ARE NOT A CLAN MEMBER^7!\n\""));
			//		return;
			//	}
			//	p++;
			//	if(*p){
			//		if(*p == ' ')
			//			p++;
			//		mode = CLAN_CHAT;
			//	}
			//	else {p--;p--;p--;}
			//	break;
			//case 't':
			//case 'T':
			//	if(!ent->client->sess.clanMember){
			//		trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                    ^3YOU ARE NOT A CLAN MEMBER^7!\n\""));
			//		return;
			//	}
			//	p++;
			//	if(*p){
			//		if(*p == ' ')
			//			p++;
			//		mode = CLAN_TALK;
			//	}
			//	else {p--;p--;p--;}
			//	break;						
			p++;
			switch(*p)
			{
				
#ifdef _CLANMEMBER
			case 'c':
			case 'C':
				if(!ent->client->sess.clanMember){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                    ^3YOU ARE NOT A CLAN MEMBER^7!\n\""));
					return;
				}
				p++;
				if(*p){
					if(*p == ' ')
						p++;
					mode = CLAN_CHAT;
				}
				else {p--;p--;p--;}
				break;
			case 'T':
			case 't':
				if(!ent->client->sess.clanMember){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                    ^3YOU ARE NOT A CLAN MEMBER^7!\n\""));
					return;
				}
				p++;
				if(*p){
					if(*p == ' ')
						p++;
				mode = CLAN_TALK;
				}
				else {p--;p--;p--;}
				break;
#endif
			case 'a':
			case 'A':
				p++;
				if(*p){
					if(*p == ' ')
						p++;
					mode = CADM_CHAT;
				}
				else {p--;p--;p--;}
				break;
			case 'p':
			case 'P':
				if(ent->client->nvSess.admin < 3){
					trap_SendServerCommand( ent - g_entities, va("chat -1 \"^1                                 ^3YOU DO NOT HAVE ADMIN POWERS^7!\n\""));
					return;
				}
				p++;
				if(*p == ' ') p++;
				RPM_Tokens(ent, p, mode);
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,%s",	level.time + 5000,	p));
				G_LogPrintf( "!cp: %s: %s\n", ent->client->pers.netname, p );
				{	
					char	string[128];
					Com_sprintf( string, sizeof(string),"!CP from %s", ent->client->pers.netname);
					NV_adminPrint(string);
				}				
				return;
			default:
				p--;
				p--;
				break;
			}
			break;

		case 'y':
		case 'Y':		
			p++;
			switch(*p)
			{	
				case '\0':
				case 'e':
				case 'E':
				case ' ':
				{
					//if((*p && (*p == 'e' || *p == 'E'  || *p == ' ')) || !*p)
					//{
					if(g_allowSpecVote.integer <= 1 && ent->client->sess.team == TEAM_SPECTATOR)
					{
						trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote as spectator.\n\"" );
						return;
					}
					if ( !level.voteTime )
					{
						trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
						trap_SendServerCommand( ent - g_entities, va("chat -1 \"server: ^3No Vote in progress^7!\n\""));
						return;
					}
					if ( ent->client->ps.eFlags & EF_VOTED )
					{
						trap_SendServerCommand( ent-g_entities, "print \"Vote already cast.\n\"" );
						trap_SendServerCommand( ent - g_entities, va("chat -1 \"server: ^3You have already voted^7!\n\""));
						return;
					}
					trap_SendServerCommand( -1, va("print \"^5%s voted ^3YES\n\"", ent->client->pers.netname));
					ent->client->ps.eFlags |= EF_VOTED;
					level.voteYes++;
					trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
					p--;
					p--;
					break;
				}
				default:
					p--;
					p--;
					break;
			}
			break;

		case 'n':
		case 'N':					
			p++;
			switch(*p)
			{	
				case '\0':
				case 'o':
				case 'O':
				case ' ':
				{
				//if((*p && (*p == 'o' || *p == 'O'  || *p == ' ')) || !*p)
				//{
					if(g_allowSpecVote.integer <= 1 && ent->client->sess.team == TEAM_SPECTATOR)
					{
						trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote as spectator.\n\"" );
						return;
					}
					if ( !level.voteTime )
					{
						trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
						trap_SendServerCommand( ent-g_entities, va("chat -1 \"server: ^3No Vote in progress^7!\n\""));
						return;
					}
					if ( ent->client->ps.eFlags & EF_VOTED )
					{
						trap_SendServerCommand( ent-g_entities, "print \"Vote already cast.\n\"" );
						trap_SendServerCommand( ent-g_entities, va("chat -1 \"server: ^3You have already voted^7!\n\""));
						return;
					}
					trap_SendServerCommand( -1, va("print \"^5%s voted ^1NO\n\"", ent->client->pers.netname));
					ent->client->ps.eFlags |= EF_VOTED;
					level.voteNo++;
					trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );
					p--;
					p--;
					break;
				}
				default:
					p--;
					p--;
					break;
			}
			break;
		
		default:
			p--;
			break;
		}

	}
	///End  - 02.04.05 - 12:44pm

	///Ryan
	RPM_Tokens(ent, p, mode); 
	///End

	G_Say( ent, NULL, mode, p );
}

/*
==================
Cmd_Tell_f
==================
*/
static void Cmd_Tell_f( gentity_t *ent ) {
	int			targetNum;
	gentity_t	*target;
	char		*p;
	char		arg[MAX_TOKEN_CHARS];

	if ( trap_Argc () < 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	targetNum = atoi( arg );
	if ( targetNum < 0 || targetNum >= level.maxclients ) {
		return;
	}

	target = &g_entities[targetNum];
	if ( !target || !target->inuse || !target->client ) {
		return;
	}

	p = ConcatArgs( 2 );

	//Ryan june 10 2003
	RPM_Tokens(ent, p, SAY_TELL);
	//Ryan

	G_LogPrintf( "tell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, p );
	G_Say( ent, target, SAY_TELL, p );
	// don't tell to the player self if it was already directed to this player
	// also don't send the chat back to a bot
	if ( ent != target && !(ent->r.svFlags & SVF_BOT)) {
		G_Say( ent, ent, SAY_TELL, p );
	}
}


static void G_VoiceTo ( gentity_t *ent, gentity_t *other, int mode, const char* name, const char *id, qboolean voiceonly )
{
	// Only team say is supported right now for voice chatting
	if (mode != SAY_TEAM)
	{
		return;
	}

	if (!other || !other->inuse || !other->client)
	{
		return;
	}

	if ( !OnSameTeam(ent, other) )
	{
		return;
	}

	trap_SendServerCommand( other-g_entities, va("%s %d %d \"%s\" \"%s\"", "vtchat", voiceonly, ent->s.number, name, id));
}

/*
==================
G_CanVoiceGlobal

Can we globaly speak right now
==================
*/
qboolean G_CanVoiceGlobal ( void )
{
	if ( level.gametypeData->teams && level.time - level.globalVoiceTime > 5000 )
	{
		return qtrue;
	}

	return qfalse;
}

/*
==================
G_VoiceGlobal

says something out loud that everyone in the radius can hear
==================
*/
void G_VoiceGlobal ( gentity_t* ent, const char* id, qboolean force )
{

	if ( !ent )
	{
		return;
	}

	if ( !level.gametypeData->teams )
	{
		return;
	}

	if ( !force && level.time - level.globalVoiceTime < 5000 )
	{
		return;
	}

	level.globalVoiceTime = level.time;

	trap_SendServerCommand( -1, va("vglobal %d \"%s\"", ent->s.number, id));
}

/*
==================
G_Voice
==================
*/
void G_Voice( gentity_t *ent, gentity_t *target, int mode, const char *id, qboolean voiceonly )
{
	int			j;
	gentity_t	*other;
	char		name[MAX_SAY_TEXT];

	// GodOrDevil - 10.23.05 - 10:05pm #crashFix
	if(strlen(id) > 64){
		char	text[32] = "\0";
		Q_strncpyz(text, id, sizeof(text));
		NV_crashLog (va("%s - %s - vsay_team: %s", ent->client->nvSess.cleanName, ent->client->nvSess.ip, text)) ;
		trap_SendServerCommand( -1, va("print \"Possible ^3CRASH Attempt ^7Client IP & NAME Logged: %s\n\"", ent->client->pers.netname));
	//	trap_SendServerCommand( ent->s.number,  va("chat -1 \"The server has detected a possible ^3crash attempt ^7from you [^3%s^7]. Your ip and information has been logged.\n\"", ent->client->pers.netname));
		return;
	}
	//End  - 10.23.05 - 10:08pm

	// Spectators and ghosts dont talk
	if ( ent->client->ps.pm_type == PM_DEAD || G_IsClientSpectating ( ent->client ) )
	{
		return;
	}

	// Voice flooding protection on?
	if ( g_voiceFloodCount.integer )
	{
		// If this client has been penalized for voice chatting to much then dont allow the voice chat
		if ( ent->client->voiceFloodPenalty )
		{
			if ( ent->client->voiceFloodPenalty > level.time )
			{
				return;
			}

			// No longer penalized
			ent->client->voiceFloodPenalty = 0;
		}

		// See if this client flooded with voice chats
		ent->client->voiceFloodCount++;
		if ( ent->client->voiceFloodCount >= g_voiceFloodCount.integer )
		{
			ent->client->voiceFloodCount = 0;
			ent->client->voiceFloodTimer = 0;
			ent->client->voiceFloodPenalty = level.time + g_voiceFloodPenalty.integer * 1000;

			trap_SendServerCommand( ent-g_entities, va("print \"Voice chat flooded, you will be able use voice chats again in (%d) seconds\n\"", g_voiceFloodPenalty.integer ) );

			return;
		}
	}

	G_GetChatPrefix ( ent, target, mode, name, sizeof(name) );

	if ( target )
	{
		G_VoiceTo( ent, target, mode, name, id, voiceonly );
		return;
	}

	// send it to all the apropriate clients
	for (j = 0; j < level.maxclients; j++)
	{
		other = &g_entities[j];
		G_VoiceTo( ent, other, mode, name, id, voiceonly );

	}
}

/*
==================
Cmd_Voice_f
==================
*/
static void Cmd_Voice_f( gentity_t *ent, int mode, qboolean arg0, qboolean voiceonly )
{
	char		*p;

	if ( trap_Argc () < 2 && !arg0 ) {
		return;
	}

	if (arg0)
	{
		p = ConcatArgs( 0 );
	}
	else
	{
		p = ConcatArgs( 1 );
	}

	G_Voice( ent, NULL, mode, p, voiceonly );
}

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent )
{
	trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", vtos( ent->s.origin ) ) );
}

/*
============
G_VoteDisabled

determins if the given vote is disabled
============
*/
int G_VoteDisabled ( const char* callvote )
{
	return trap_Cvar_VariableIntegerValue( va("novote_%s", callvote) );
}

#ifdef _COMPMODE
void Cmd_COMP_CallVote_f( gentity_t *ent )
{
	int		i;
	char	arg1[MAX_STRING_TOKENS] = "\0";
	char	arg2[MAX_STRING_TOKENS] = "\0";

	// GodOrDevil - 04.22.05 - 09:46pm #AdminVotes
	//// Admins and refs are always allowed to call votes
	if (!(ent->client->nvSess.admin >= g_callVote.integer || ent->client->nvSess.referee)){
		if ( !g_allowVote.integer ){
		////if(!g_allowVote.integer && !(ent->client->nvSess.admin >= g_callVote.integer || ent->client->nvSess.referee))
			trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
			return;
		}
		if ( level.intermissiontime || level.intermissionQueued){
			trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed during intermission.\n\"" );
			return;
		}
		//Ryan june 15 2003
		if(level.pause) {
			trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed while paused.\n\"" );
			return;
		}
		//End
		//// No voting within the minute of a map change
		////if ( level.time - level.startTime < 1000 * 60 )
		if ( level.time - level.startTime < 1000 * 60 && !g_allowMinuteVote.integer ){
			trap_SendServerCommand( ent-g_entities, "print \"Cannot vote within the first minute of a map change.\n\"" );
			return;
		}
		////Moved Down
		////if ( level.voteTime )
		////{
		////	trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress.\n\"" );
		////	return;
		////}

	// Ryan Aug 15 2004
	// Admins and Refs dont have a vote limit
	//if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT )
	if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT && !(ent->client->nvSess.admin || ent->client->nvSess.referee)){
	// Ryan
			trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of votes.\n\"" );
			return;
		}
		//Ryan march 24 2003		can spectators call votes
		////if ( ent->client->sess.team == TEAM_SPECTATOR )
//	if(!g_allowSpecVote.integer && ent->client->sess.team == TEAM_SPECTATOR)
	if(!g_allowSpecVote.integer && ent->client->sess.team == TEAM_SPECTATOR && !(ent->client->nvSess.admin || ent->client->nvSess.referee))
	{
			trap_SendServerCommand( ent-g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
			return;
		}

	// GodOrDevil - 04.02.05 - 08:34pm - Admins/refs get no failed vote time
	if ( ent->client->voteDelayTime > level.time  && !(ent->client->nvSess.admin || ent->client->nvSess.referee)){
		trap_SendServerCommand( ent-g_entities, va("print \"You are not allowed to vote within %d minute of a failed vote.\n\"", g_failedVoteDelay.integer ) );
			return;
		}
	}
	//End  - 04.22.05 - 09:59pm

	if ( level.voteTime ) {
		trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress.\n\"" );
		return;
	}

	// Save the voting client id
	level.voteClient = ent->s.number;

	// make sure it is a valid command to vote on
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	if( strchr( arg1, ';' ) || strchr( arg2, ';' ) )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
		return;
	}

	if ( !Q_stricmp( arg1, "map_restart" ) ) {
	} else if ( !Q_stricmp( arg1, "kick" ) ) {
	} else if ( !Q_stricmp( arg1, "referee" ) ) {
	} else if ( !Q_stricmp( arg1, "unreferee" ) ) {
	} else if ( !Q_stricmp( arg1, "swap_teams" ) ) {
	} else if ( !Q_stricmp( arg1, "swapteams" ) ) {
	} else if ( !Q_stricmp( arg1, "start_match" ) ) {
	} else if ( !Q_stricmp( arg1, "startmatch" ) ) {
	///End  - 01.21.06 - 06:46pm
	// GodOrDevil
	}else if ( !Q_stricmp( arg1, "?" ) )
	{
		trap_SendServerCommand( ent-g_entities, "print \"^3Vote commands are^7:\n======================\n^5 map_restart\n^5 swap_teams\n^5 start_match\n^5 referee <clientnum>\n^5 unreferee <clientnum>\n\"" );
		return;
	}
	else
	{
		trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
		trap_SendServerCommand( ent-g_entities, "print \"^3Vote commands are^7:\n======================\n^5 map_restart\n^5 swap_teams\n^5 start_match\n^5 referee <clientnum>\n^5 unreferee <clientnum>\n\"" );
		return;
	}

	if (!(ent->client->nvSess.admin >= g_callVote.integer || ent->client->nvSess.referee) && G_VoteDisabled ( arg1 ) )
	{
		trap_SendServerCommand( ent-g_entities, va("print \"The '%s' vote has been disabled on this server.\n\"", arg1) );
		return;
	}

	// if there is still a vote to be executed
	if ( level.voteExecuteTime )
	{
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
	}
	else if ( !Q_stricmp ( arg1, "kick" ) )
	{
		int clientid = G_ClientNumberFromName ( arg2 );

		if ( clientid == -1 )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"there is no client named '%s' currently on the server.\n\"", arg2 ) );
			return;
		}

        //Ryan march 24 2003
		//if theyre on the nokick list or admins list dont kick them
// GodOrDevil - 1.04.2005 - #ClanList No Kick - RPM GOLD
//RM    if (g_entities[clientid].client->nvSess.admin || g_entities[clientid].client->sess.nokick)
		
#ifdef _CLANMEMBER
		if (g_entities[clientid].client->nvSess.admin || g_entities[clientid].client->sess.nokick || g_entities[clientid].client->sess.clanMember)
#else
		if (g_entities[clientid].client->nvSess.admin || g_entities[clientid].client->sess.nokick )

#endif
//End
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s CANNOT BE KICKED FROM THIS SERVER!!!!.\n\"", g_entities[clientid].client->pers.netname ) );
			return;
		}
	    //Ryan

		Com_sprintf ( level.voteString, sizeof(level.voteString ), "clientkick %d", clientid );
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "kick %s", g_entities[clientid].client->pers.netname );
	}

	else if ( !Q_stricmp ( arg1, "referee" ))
	{
		int n;

		if (arg2[0] >= '0' && arg2[0] <= '9')
		{
			n = atoi( arg2 );
		}
		else
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Bad client slot: %s\n\"", arg2));
			trap_SendServerCommand( ent-g_entities, va("print \"Usage: callvote referee <id number>\n\""));
			return;
		}

		if ( n< 0 || n >= g_maxclients.integer )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Invalid client number %d.\n\"", n));
			return;
		}

		if ( g_entities[n].client->pers.connected == CON_DISCONNECTED )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"there is no client with the client number %d.\n\"", n ) );
			return;
		}
		if ( g_entities[n].client->nvSess.admin)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s is an Admin\n\"", g_entities[n].client->pers.netname) );
			return;
		}
		if ( g_entities[n].client->nvSess.referee)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s is already a referee\n\"", g_entities[n].client->pers.netname) );
			return;
		}

		Com_sprintf ( level.voteString, sizeof(level.voteString ), "%d", n );
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "Referee: %s", g_entities[n].client->pers.netname );
	}

	else if ( !Q_stricmp ( arg1, "unreferee" ))
	{
		int n;

		if (arg2[0] >= '0' && arg2[0] <= '9')
		{
			n = atoi( arg2 );
		}
		else
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Bad client slot: %s\n\"", arg2));
			trap_SendServerCommand( ent-g_entities, va("print \"Usage: callvote unreferee <id number>\n\""));
			return;
		}

		if ( n< 0 || n >= g_maxclients.integer )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Invalid client number %d.\n\"", n));
			return;
		}
		if ( g_entities[n].client->pers.connected == CON_DISCONNECTED )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"there is no client with the client number %d.\n\"", n ) );
			return;
		}
		if ( g_entities[n].client->nvSess.admin)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s is an Admin\n\"", g_entities[n].client->pers.netname) );
			return;
		}
		if (!g_entities[n].client->nvSess.referee)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s is not a referee\n\"", g_entities[n].client->pers.netname) );
			return;
		}

		Com_sprintf ( level.voteString, sizeof(level.voteString ), "%d", n );
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "UnReferee: %s", g_entities[n].client->pers.netname );
	}
	else if ( !Q_stricmp ( arg1, "swap_teams" ))
	{
		Com_sprintf( level.voteString, sizeof( level.voteString ), "swap_teams");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Swap Teams");
	}
	else if ( !Q_stricmp ( arg1, "start_match" ))
	{
		if(g_doWarmup.integer != 2)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Ready-Up mode is not enabled on this server.\n\""));
			return;
		}
		if(level.warmupTime > -1)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"The match has already started.\n\""));
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "start_match");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Start Match");
	}

	else
	{
		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	}

	trap_SendServerCommand( -1, va("print \"%s called a vote.\n\"", ent->client->pers.netname ) );

	NV_Log(3, va("%s (%s) - VOTE: %s %s", ent->client->nvSess.cleanName, ent->client->nvSess.ip, arg1, arg2)) ;
	NV_SQLAdminLog(ent, NULL, va("VOTE: %s %s", arg1, arg2));

	// start the voting, the caller autoamtically votes yes
	level.voteTime = level.time;
	level.voteYes = 1;
	level.voteNo = 0;

	for ( i = 0 ; i < level.maxclients ; i++ )
	{
		level.clients[i].ps.eFlags &= ~EF_VOTED;
	}
	ent->client->ps.eFlags |= EF_VOTED;

	trap_SetConfigstring( CS_VOTE_TIME, va("%i,%i", level.voteTime, g_voteDuration.integer*1000 ) );
	trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );
	trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
	trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );
	trap_SetConfigstring( CS_VOTE_NEEDED, va("%i", level.numVotingClients / 2 ) );

	RPM_GlobalSound(G_SoundIndex("sound/movers/buttons/button02.mp3"));
}
#endif



#ifndef _NEW_VOTING
void Cmd_CallVote_f( gentity_t *ent )
{
	int		i;
	char	arg1[MAX_STRING_TOKENS] = "\0";
	char	arg2[MAX_STRING_TOKENS] = "\0";

#ifdef _COMPMODE
	if(g_compMode.integer)
	{
		Cmd_COMP_CallVote_f (ent);
		return;
	}
#endif

	// GodOrDevil - 04.22.05 - 09:46pm #AdminVotes
	//// Admins and refs are always allowed to call votes
	if (!(ent->client->nvSess.admin >= g_callVote.integer || ent->client->nvSess.referee)){
		if ( !g_allowVote.integer ){
		////if(!g_allowVote.integer && !(ent->client->nvSess.admin >= g_callVote.integer || ent->client->nvSess.referee))
			trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
			return;
		}
		if ( level.intermissiontime || level.intermissionQueued){
			trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed during intermission.\n\"" );
			return;
		}
		//Ryan june 15 2003
		if(level.pause) {
			trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed while paused.\n\"" );
			return;
		}
		//End
		//// No voting within the minute of a map change
		////if ( level.time - level.startTime < 1000 * 60 )
		if ( level.time - level.startTime < 1000 * 60 && !g_allowMinuteVote.integer ){
			trap_SendServerCommand( ent-g_entities, "print \"Cannot vote within the first minute of a map change.\n\"" );
			return;
		}
		////Moved Down
		////if ( level.voteTime )
		////{
		////	trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress.\n\"" );
		////	return;
		////}

	// Ryan Aug 15 2004
	// Admins and Refs dont have a vote limit
	//if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT )
	if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT && !(ent->client->nvSess.admin || ent->client->nvSess.referee)){
	// Ryan
			trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of votes.\n\"" );
			return;
		}
		//Ryan march 24 2003		can spectators call votes
		////if ( ent->client->sess.team == TEAM_SPECTATOR )
//	if(!g_allowSpecVote.integer && ent->client->sess.team == TEAM_SPECTATOR)
	if(!g_allowSpecVote.integer && ent->client->sess.team == TEAM_SPECTATOR && !(ent->client->nvSess.admin || ent->client->nvSess.referee))
	{
			trap_SendServerCommand( ent-g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
			return;
		}

	// GodOrDevil - 04.02.05 - 08:34pm - Admins/refs get no failed vote time
	if ( ent->client->voteDelayTime > level.time  && !(ent->client->nvSess.admin || ent->client->nvSess.referee)){
		trap_SendServerCommand( ent-g_entities, va("print \"You are not allowed to vote within %d minute of a failed vote.\n\"", g_failedVoteDelay.integer ) );
			return;
		}
	}
	//End  - 04.22.05 - 09:59pm

	if ( level.voteTime ) {
		trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress.\n\"" );
		return;
	}

	// Save the voting client id
	level.voteClient = ent->s.number;

	// make sure it is a valid command to vote on
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	if( strchr( arg1, ';' ) || strchr( arg2, ';' ) )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
		return;
	}

	if ( !Q_stricmp( arg1, "map_restart" ) ) {
	} else if ( !Q_stricmp( arg1, "mapcycle" ) ) {
	} else if ( !Q_stricmp( arg1, "map" ) ) {
	} else if ( !Q_stricmp( arg1, "rmgmap" ) ) {
	} else if ( !Q_stricmp( arg1, "g_gametype" ) ) {
	} else if ( !Q_stricmp( arg1, "kick" ) ) {
	} else if ( !Q_stricmp( arg1, "clientkick" ) ) {
	} else if ( !Q_stricmp( arg1, "g_doWarmup" ) ) {
	} else if ( !Q_stricmp( arg1, "timelimit" ) ) {
	} else if ( !Q_stricmp( arg1, "timeextension" ) ) {
	} else if ( !Q_stricmp( arg1, "scorelimit" ) ) {
	//Ryan
	} else if ( !Q_stricmp( arg1, "friendlyfire" ) ) {
#ifdef _RPM_WEAPONMOD
	} else if ( !Q_stricmp( arg1, "damagemod" ) ) {
	} else if ( !Q_stricmp( arg1, "defaultdamage" ) ) {
	} else if ( !Q_stricmp( arg1, "ammomod" ) ) {
	} else if ( !Q_stricmp( arg1, "defaultammo" ) ) {
	} else if ( !Q_stricmp( arg1, "weaponmod" ) ) {
	} else if ( !Q_stricmp( arg1, "defaultweapons" ) ) {
#endif
		// GodOrDevil - 01.21.06 - 06:46pm #vote #Clanvsall
#ifdef _CLANMEMBER	
	} else if ( !Q_stricmp( arg1, "clanvsall" ) ) {
#endif
#ifdef _COMPMODE
	} else if ( !Q_stricmp( arg1, "compmode" ) ) {
	} else if ( !Q_stricmp( arg1, "referee" ) ) {
	} else if ( !Q_stricmp( arg1, "unreferee" ) ) {
	} else if ( !Q_stricmp( arg1, "swap_teams" ) ) {
	} else if ( !Q_stricmp( arg1, "swapteams" ) ) {
	} else if ( !Q_stricmp( arg1, "start_match" ) ) {
	} else if ( !Q_stricmp( arg1, "startmatch" ) ) {
	///End  - 01.21.06 - 06:46pm
	// GodOrDevil
	} else if ( g_compMode.integer )  {
		if ( !Q_stricmp( arg1, "?" ) ) {
			trap_SendServerCommand( ent-g_entities, "print \"^3Vote commands are^7: map_restart, swap_teams, start_match, referee <clientnum>, unreferee <clientnum>\n\"" );
			return;
		}
		else
		{
			trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
			trap_SendServerCommand( ent-g_entities, "print \"Vote commands are: map_restart, swap_teams, start_match, referee <clientnum>, unreferee <clientnum>\n\"" );
			return;
		}
#endif
	// END
	} else if ( !Q_stricmp( arg1, "?" ) ) {
		trap_SendServerCommand( ent-g_entities, "print \"Vote commands are: map_restart, nextmap, map <mapname>, g_gametype <n>, kick <player>, clientkick <clientnum>, g_doWarmup, timelimit <time>, scorelimit <score>.\n\"" );
#ifdef _RPM_WEAPONMOD
		trap_SendServerCommand( ent-g_entities, "print \"^3RPM Vote commands are: ^7friendlyfire, compmode, swap_teams, start_match\nreferee <clientnum>, unreferee <clientnum>, damagemod, defaultdamage\ammomod, defaultammo, weaponmod, defaultweapons.\n\"" );
#endif
		return;
	}
	//Ryan
	else
	{
		trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
		trap_SendServerCommand( ent-g_entities, "print \"Vote commands are: map_restart, nextmap, map <mapname>, g_gametype <n>, kick <player>, clientkick <clientnum>, g_doWarmup, timelimit <time>, scorelimit <score>.\n\"" );
		//Ryan
#ifdef _RPM_WEAPONMOD
		trap_SendServerCommand( ent-g_entities, "print \"^3RPM Vote commands are: ^7friendlyfire, compmode, swap_teams, start_match\nreferee <clientnum>, unreferee <clientnum>, damagemod, defaultdamage\nammomod, defaultammo, weaponmod, defaultweapons.\n\"" );
#endif
		//Ryan
		return;
	}
//#GOLD
	// see if this particular vote is disabled
	// Ryan Dec 14 2004
	// Admins and Refs can call any vote they want
	//if ( G_VoteDisabled ( arg1 ) )
	if (!(ent->client->nvSess.admin >= g_callVote.integer || ent->client->nvSess.referee) && G_VoteDisabled ( arg1 ) )
	// Ryan
//#ENDGOLD
	{
		trap_SendServerCommand( ent-g_entities, va("print \"The '%s' vote has been disabled on this server.\n\"", arg1) );
		return;
	}

	// if there is still a vote to be executed
	if ( level.voteExecuteTime )
	{
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
	}

	// special case for g_gametype, check for bad values
	if ( !Q_stricmp( arg1, "g_gametype" ) )
	{
		// Verify the gametype
		i = BG_FindGametype ( arg2 );
		if ( i < 0 )
		{
			trap_SendServerCommand( ent-g_entities, "print \"Invalid gametype.\n\"" );
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s; map_restart 5", arg1, arg2 );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s %s", arg1, bg_gametypeData[i].name );
	}
	else if ( !Q_stricmp( arg1, "map" ) )
	{
		if ( !G_DoesMapExist ( arg2 ) )
		{
			trap_SendServerCommand( ent-g_entities, "print \"This server does not have that map!\n\"" );
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	}
	else if ( !Q_stricmp( arg1, "rmgmap" ) )
	{
		char	arg3[MAX_STRING_TOKENS];
		char	arg4[MAX_STRING_TOKENS];

		trap_Argv( 3, arg3, sizeof( arg3 ) );
		trap_Argv( 4, arg4, sizeof( arg4 ) );

		Com_sprintf( level.voteString, sizeof( level.voteString ), "rmgmap 1 %s 2 %s 3 %s 4 \"%s\" 0", arg2, arg3, arg4, ConcatArgs ( 5 ) );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	}
	else if ( !Q_stricmp( arg1, "mapcycle" ) )
	{
		if (!*g_mapcycle.string || !Q_stricmp ( g_mapcycle.string, "none" ) )
		{
			trap_SendServerCommand( ent-g_entities, "print \"there is no map cycle ccurently set up.\n\"" );
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "mapcycle");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "nextmap" );
	}
	else if ( !Q_stricmp ( arg1, "clientkick" ) )
	{
		int n = atoi ( arg2 );

		if ( n < 0 || n >= MAX_CLIENTS )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"invalid client number %d.\n\"", n ) );
			return;
		}

		if ( g_entities[n].client->pers.connected == CON_DISCONNECTED )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"there is no client with the client number %d.\n\"", n ) );
			return;
		}

		//Ryan march 24 2003
		//if theyre on the nokick list or admins list dont kick them
// GodOrDevil - 1.04.2005 - #ClanList No Kick - RPM GOLD
//RM    if (g_entities[n].client->nvSess.admin || g_entities[n].client->sess.nokick)
#ifdef _CLANMEMBER
		if (g_entities[n].client->nvSess.admin || g_entities[n].client->sess.nokick || g_entities[n].client->sess.clanMember)

#else
		if (g_entities[n].client->nvSess.admin || g_entities[n].client->sess.nokick)
#endif
			//End
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s CANNOT BE KICKED FROM THIS SERVER!!!!.\n\"", g_entities[n].client->pers.netname ) );
			return;
		}
	    //Ryan

		Com_sprintf ( level.voteString, sizeof(level.voteString ), "%s %s", arg1, arg2 );
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "kick %s", g_entities[n].client->pers.netname );
	}
	else if ( !Q_stricmp ( arg1, "kick" ) )
	{
		int clientid = G_ClientNumberFromName ( arg2 );

		if ( clientid == -1 )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"there is no client named '%s' currently on the server.\n\"", arg2 ) );
			return;
		}

        //Ryan march 24 2003
		//if theyre on the nokick list or admins list dont kick them
// GodOrDevil - 1.04.2005 - #ClanList No Kick - RPM GOLD
//RM    if (g_entities[clientid].client->nvSess.admin || g_entities[clientid].client->sess.nokick)
#ifdef _CLANMEMBER
		if (g_entities[clientid].client->nvSess.admin || g_entities[clientid].client->sess.nokick || g_entities[clientid].client->sess.clanMember)
#else
		if (g_entities[clientid].client->nvSess.admin || g_entities[clientid].client->sess.nokick)
#endif
//End
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s CANNOT BE KICKED FROM THIS SERVER!!!!.\n\"", g_entities[clientid].client->pers.netname ) );
			return;
		}
	    //Ryan

		Com_sprintf ( level.voteString, sizeof(level.voteString ), "clientkick %d", clientid );
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "kick %s", g_entities[clientid].client->pers.netname );
	}
	else if ( !Q_stricmp ( arg1, "timeextension" ) )
	{
		if ( !g_timelimit.integer )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"There is no timelimit to extend.\n\"") );
			return;
		}

		if ( !g_timeextension.integer )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"This server does not allow time extensions.\n\"") );
			return;
		}
		Com_sprintf ( level.voteString, sizeof(level.voteString ), "extendtime %d", g_timeextension.integer );
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "extend timelimit by %d minutes", g_timeextension.integer );
	}

	//Ryan
	else if ( !Q_stricmp ( arg1, "friendlyfire" ) )
	{
		if(g_friendlyFire.integer)
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "g_friendlyFire \"0\"");
			Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Friendly Fire OFF");
		}
		else
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "g_friendlyFire \"1\"");
			Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Friendly Fire ON");
		}
	}

#ifdef _COMPMODE
	else if ( !Q_stricmp ( arg1, "compmode" ) )
	{
		if(g_compMode.integer)
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "g_compMode \"0\"");
			Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Competition Mode OFF");
		}
		else
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "g_compMode \"1\"");
			Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Competiton Mode ON");
		}
	}

	else if ( !Q_stricmp ( arg1, "referee" ))
	{
		int n;

		if (arg2[0] >= '0' && arg2[0] <= '9')
		{
			n = atoi( arg2 );
		}
		else
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Bad client slot: %s\n\"", arg2));
			trap_SendServerCommand( ent-g_entities, va("print \"Usage: callvote referee <id number>\n\""));
			return;
		}

		if ( n< 0 || n >= g_maxclients.integer )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Invalid client number %d.\n\"", n));
			return;
		}

		if ( g_entities[n].client->pers.connected == CON_DISCONNECTED )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"there is no client with the client number %d.\n\"", n ) );
			return;
		}
		if ( g_entities[n].client->nvSess.admin)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s is an Admin\n\"", g_entities[n].client->pers.netname) );
			return;
		}
		if ( g_entities[n].client->nvSess.referee)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s is already a referee\n\"", g_entities[n].client->pers.netname) );
			return;
		}

		Com_sprintf ( level.voteString, sizeof(level.voteString ), "%d", n );
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "Referee: %s", g_entities[n].client->pers.netname );
	}

	else if ( !Q_stricmp ( arg1, "unreferee" ))
	{
		int n;

		if (arg2[0] >= '0' && arg2[0] <= '9')
		{
			n = atoi( arg2 );
		}
		else
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Bad client slot: %s\n\"", arg2));
			trap_SendServerCommand( ent-g_entities, va("print \"Usage: callvote unreferee <id number>\n\""));
			return;
		}

		if ( n< 0 || n >= g_maxclients.integer )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Invalid client number %d.\n\"", n));
			return;
		}
		if ( g_entities[n].client->pers.connected == CON_DISCONNECTED )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"there is no client with the client number %d.\n\"", n ) );
			return;
		}
		if ( g_entities[n].client->nvSess.admin)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s is an Admin\n\"", g_entities[n].client->pers.netname) );
			return;
		}
		if (!g_entities[n].client->nvSess.referee)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%s is not a referee\n\"", g_entities[n].client->pers.netname) );
			return;
		}

		Com_sprintf ( level.voteString, sizeof(level.voteString ), "%d", n );
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "UnReferee: %s", g_entities[n].client->pers.netname );
	}
	else if ( !Q_stricmp ( arg1, "swap_teams" ))
	{
		Com_sprintf( level.voteString, sizeof( level.voteString ), "swap_teams");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Swap Teams");
	}
	else if ( !Q_stricmp ( arg1, "start_match" ))
	{
		if(g_doWarmup.integer != 2)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Ready-Up mode is not enabled on this server.\n\""));
			return;
		}
		if(level.warmupTime > -1)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"The match has already started.\n\""));
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "start_match");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Start Match");
	}
#endif

#ifdef _RPM_WEAPONMOD
	else if( !Q_stricmp ( arg1, "damagemod" ))
	{
		if(g_weaponModFlags.integer & DAMAGE_MOD)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Servers modified Damage amounts are already loaded\n\""));
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "mod d");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Modify Damage amounts");
	}
	else if( !Q_stricmp ( arg1, "defaultdamage" ))
	{
		if(!(g_weaponModFlags.integer & DAMAGE_MOD))
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Server is not currently running with modified damage\n\""));
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "unmod d");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Restore Default Damage amounts");
	}
	else if( !Q_stricmp ( arg1, "ammomod" ))
	{
		if(g_weaponModFlags.integer & AMMO_MOD)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Servers modified Ammo amounts are already loaded\n\""));
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "mod a");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Modify Ammo amounts");
	}
	else if( !Q_stricmp ( arg1, "defaultammo" ))
	{
		if(!(g_weaponModFlags.integer & AMMO_MOD))
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Server is not currently running with modified ammo\n\""));
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "unmod a");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Restore default Ammo amounts");
	}
	else if( !Q_stricmp ( arg1, "weaponmod" ))
	{
		if(g_weaponModFlags.integer == 3) // 3 == AMMO_MOD & DAMAGE_MOD
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Servers Weapon mod values are already loaded\n\""));
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "mod w");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Modify weapon Damage and Ammo amounts");
	}
	else if( !Q_stricmp ( arg1, "defaultweapons" ))
	{
		if(g_weaponModFlags.integer == 0)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Server is not currently running with modified weapon values\n\""));
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "unmod w");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Restore default Damage and Ammo amounts");
	}
#endif
	//Ryan

	// GodOrDevil - 01.21.06 - 06:32pm #vote #Clanvsall
#ifdef _CLANMEMBER	
	else if ( !Q_stricmp ( arg1, "clanvsall" ) ){
		Com_sprintf( level.voteString, sizeof( level.voteString ), "clanvsall");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Clan vs ALL");
	}
#endif
	///End  - 01.21.06 - 06:33pm

	else
	{
		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	}

	trap_SendServerCommand( -1, va("print \"%s called a vote.\n\"", ent->client->pers.netname ) );
	
#ifdef _DB_ENABLED
	if(g_enableDatabase.integer)
		NV_SQLAdminLog(ent, NULL, va("VOTE: %s %s", arg1, arg2));
	//else
#endif
	NV_Log(3, va("%s (%s) - VOTE: %s %s", ent->client->nvSess.cleanName, ent->client->nvSess.ip, arg1, arg2)) ;

	// start the voting, the caller autoamtically votes yes
	level.voteTime = level.time;
	level.voteYes = 1;
	level.voteNo = 0;

	for ( i = 0 ; i < level.maxclients ; i++ )
	{
		level.clients[i].ps.eFlags &= ~EF_VOTED;
	}
	ent->client->ps.eFlags |= EF_VOTED;

	trap_SetConfigstring( CS_VOTE_TIME, va("%i,%i", level.voteTime, g_voteDuration.integer*1000 ) );
	trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );
	trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
	trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );
	trap_SetConfigstring( CS_VOTE_NEEDED, va("%i", level.numVotingClients / 2 ) );

	RPM_GlobalSound(G_SoundIndex("sound/movers/buttons/button02.mp3"));
}
#endif

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f( gentity_t *ent )
{
	char msg[64];

	if ( !level.voteTime )
	{
		trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
		return;
	}

	if ( ent->client->ps.eFlags & EF_VOTED )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Vote already cast.\n\"" );
		return;
	}

    //Ryan march 25 2003		Allow spectators to vote?
	//if ( ent->client->sess.team == TEAM_SPECTATOR )
	if(g_allowSpecVote.integer <= 1 && ent->client->sess.team == TEAM_SPECTATOR)
	{
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote as spectator.\n\"" );
		return;
	}
	//Ryan

	trap_SendServerCommand( ent-g_entities, "print \"Vote cast.\n\"" );

	ent->client->ps.eFlags |= EF_VOTED;

	trap_Argv( 1, msg, sizeof( msg ) );

	if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' )
	{
		level.voteYes++;
		trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
		trap_SendServerCommand( -1, va("print \"^5%s voted ^3YES\n\"", ent->client->pers.netname));
	}
	else
	{
		level.voteNo++;
		trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );
		trap_SendServerCommand( -1, va("print \"^5%s voted ^1NO\n\"", ent->client->pers.netname));
	}

	// a majority will be determined in CheckVote, which will also account
	// for players entering or leaving
}


/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent )
{
	vec3_t		origin, angles;
	char		buffer[MAX_TOKEN_CHARS];
	int			i;

	if ( !g_cheats.integer )
	{
		trap_SendServerCommand( ent-g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return;
	}

	if ( trap_Argc() != 5 )
	{
		trap_SendServerCommand( ent-g_entities, va("print \"usage: setviewpos x y z yaw\n\""));
		return;
	}

	VectorClear( angles );
	for ( i = 0 ; i < 3 ; i++ )
	{
		trap_Argv( i + 1, buffer, sizeof( buffer ) );
		origin[i] = atof( buffer );
	}

	trap_Argv( 4, buffer, sizeof( buffer ) );
	angles[YAW] = atof( buffer );

	TeleportPlayer( ent, origin, angles );
}

/*
=================
ClientCommand
=================
*/
void ClientCommand(int clientNum) {
	gentity_t *ent;
	char	cmd[896];

	ent = g_entities + clientNum;
	if (!ent->client) {
		return;		// not fully in game yet
	}
	trap_Argv(0, cmd, sizeof(cmd));


#ifndef _CRS_BOTS
#ifdef _SOF2_BOTS
	//rww - redirect bot commands
	if (strstr(cmd, "bot_") && AcceptBotCommand(cmd, ent))
	{
		return;
	}
	//end rww
#endif
#endif

	if (Q_stricmp(cmd, "say") == 0) {

		//Ryan april 5 2003		check to see if they were muted
		if (ent->client->sess.mute){
			trap_SendServerCommand(ent - g_entities, va("print \"You are currently muted by admin.\n\""));
			return;
		}
		//Ryan

		Cmd_Say_f(ent, SAY_ALL, qfalse);
		return;
	}
	if (Q_stricmp(cmd, "say_team") == 0) {

		//Ryan april 5 2003		check to see if they were muted
		if (ent->client->sess.mute){
			trap_SendServerCommand(ent - g_entities, va("print \"You are currently muted by admin.\n\""));
			return;
		}
		//Ryan

		Cmd_Say_f(ent, SAY_TEAM, qfalse);
		return;
	}
	if (Q_stricmp(cmd, "tell") == 0)
	{
		//Ryan april 5 2003		check to see if they were muted
		if (ent->client->sess.mute){
			trap_SendServerCommand(ent - g_entities, va("print \"You are currently muted by admin.\n\""));
			return;
		}
		//Ryan

		Cmd_Tell_f(ent);
		return;
	}

	if (Q_stricmp(cmd, "vsay_team") == 0)
	{
		//Ryan april 5 2003		check to see if they were muted
		if (ent->client->sess.mute){
			trap_SendServerCommand(ent - g_entities, va("print \"You are currently muted by admin.\n\""));
			return;
		}
		//Ryan

		Cmd_Voice_f(ent, SAY_TEAM, qfalse, qfalse);
		return;
	}

	if (Q_stricmp(cmd, "score") == 0) {
		Cmd_Score_f(ent);
		return;
	}

	if (Q_stricmp(cmd, "team") == 0)
	{
		Cmd_Team_f(ent);
		return;
	}

	// ignore all other commands when at intermission
	if (level.intermissiontime)
	{
		//		Cmd_Say_f (ent, qfalse, qtrue);
		return;
	}


	if (Q_stricmp(cmd, "drop") == 0)
		Cmd_Drop_f(ent);
	else if (Q_stricmp(cmd, "give") == 0)
		Cmd_Give_f(ent);
	else if (Q_stricmp(cmd, "god") == 0)
		Cmd_God_f(ent);
	else if (Q_stricmp(cmd, "notarget") == 0)
		Cmd_Notarget_f(ent);
	else if (Q_stricmp(cmd, "noclip") == 0)
		Cmd_Noclip_f(ent);
	else if (Q_stricmp(cmd, "kill") == 0)
		Cmd_Kill_f(ent);
	else if (Q_stricmp(cmd, "levelshot") == 0)
		Cmd_LevelShot_f(ent);
	else if (Q_stricmp(cmd, "follow") == 0)
		Cmd_Follow_f(ent);
	else if (Q_stricmp(cmd, "follownext") == 0)
		Cmd_FollowCycle_f(ent, 1);
	else if (Q_stricmp(cmd, "followprev") == 0)
		Cmd_FollowCycle_f(ent, -1);
	else if (Q_stricmp(cmd, "where") == 0)
		Cmd_Where_f(ent);
	else if (Q_stricmp(cmd, "callvote") == 0)
	{
#ifndef _NEW_VOTING
		Cmd_CallVote_f(ent);
#else
		char	cmd1[MAX_TOKEN_CHARS];
		trap_Argv(1, cmd1, sizeof(cmd1));
		NV_voteCommands(ent, cmd1);
#endif
	}
	else if (Q_stricmp(cmd, "vote") == 0)
		Cmd_Vote_f(ent);
	else if (Q_stricmp(cmd, "setviewpos") == 0)
		Cmd_SetViewpos_f(ent);

	///Ryan april 7 2003
	else if (Q_stricmp(cmd, "stats") == 0)
		Cmd_Stats_f(ent);
	else if (Q_stricmp(cmd, "players") == 0)
		RPM_Players(ent);
	else if (Q_stricmp(cmd, "motd") == 0 && !level.intermissionQueued){
		RPM_Motd(ent);
		ent->client->sess.motdStartTime = level.time;
		ent->client->sess.motdStopTime = level.time + 5000;
	}
	else if (Q_stricmp(cmd, "tokens") == 0)
		RPM_Display_Tokens(ent);

#ifdef _COMPMODE
	else if (Q_stricmp(cmd, "?") == 0)
		RPM_DisplayPlayer_Cmds(ent);
	else if (Q_stricmp(cmd, "ready") == 0)
		RPM_ReadyUp(ent);
	else if (Q_stricmp(cmd, "tcmd") == 0)
		RPM_Tcmd(ent);
#endif
#ifdef _NV_ADMIN
	else if (Q_stricmp(cmd, "adm") == 0)
	{
		char	cmd1[MAX_TOKEN_CHARS];
		trap_Argv(1, cmd1, sizeof(cmd1));
		NV_cmdsAreAllowed(ent, cmd1);
	}
#else
	else if (Q_stricmp(cmd, "adm") == 0)
		Cmd_adm_f(ent);
	else if (Q_stricmp(cmd, "ref") == 0)
		RPM_ref_cmd(ent);
#endif
	else if (Q_stricmp(cmd, "sounds") == 0)
		RPM_Display_Sounds(ent);
	else if (Q_stricmp(cmd, "sounds2") == 0)
		RPM_Display_Sounds2(ent);
	else if (Q_stricmp(cmd, "extrasounds") == 0)
		RPM_Display_ExtraSounds(ent);
	else if (Q_stricmp(cmd, "refresh") == 0)
		RPM_Refresh(ent);
	else if (Q_stricmp(cmd, "roundlimits") == 0)
		RPM_Display_RoundLimits(ent);

	// GodOrDevil - 03.01.05 - 05:57pm #serverinfo
	else if (Q_stricmp(cmd, "dropitem") == 0)
		Cmd_DropItem_f(ent);
	else if (Q_stricmp(cmd, "info") == 0)
		SC_Server_Info(ent);
	else if (Q_stricmp(cmd, "misc") == 0)
		SC_miscCvarDisplay(ent);

	// GodOrDevil - GAMETYPE - MAPLIST
#ifdef _NEW_MAP_SETUP
	else if (Q_stricmp(cmd, "maplist") == 0)
		NV_printMapList(ent);
#endif

#ifdef _DEBUG
#ifdef _NEW_MAP_SETUP
	else if (Q_stricmp(cmd, "fakevote") == 0)
	{
		level.randMapNum = 0;
		//NV_getRandomMapList(qfalse);
		NV_callVoteSkipNextMap();
	}
#endif


#ifdef _EFFECTS
	else if (Q_stricmp(cmd, "efx+") == 0)
		NV_effectsInc(ent);
	else if (Q_stricmp(cmd, "efx-") == 0)
		NV_effectsDec(ent);
#endif
#ifdef _ZOMBIES
	else if (Q_stricmp(cmd, "makeZombie") == 0)
	{
		ent->s.number = ent->client->nvSess.zombie = qtrue;
		SetTeam(ent, "RED", "NPC_Virus_Male/virus_male");
	}
#endif

	//	else if (Q_stricmp (cmd, "shake" ) == 0 )
	//	{
	//		vec3_t o;
	//		VectorCopy(ent->s.origin, o);
	//		//o[2] += 100;
	//		G_PlayEffect(G_EffectIndex ( "effects/explosions/helicopter_explosion"), o, ent->s.angles );
	//	}
	//#endif
	// END

	///03.26.05 - 03.33pm #Sound #FIX - Disabled for ]DA[ Update
	/// ///Was admin only but decided to give to all but it can be
	/// ///... disabled by admin (individually or everyone)
	///else if (!Q_stricmp ( cmd, "scplay" )) {
	///	SC_Sound(ent);
	///	return;
	///}
	///else if (!Q_stricmp ( cmd, "scteam" )) {
	///	SC_TeamSound(ent);
	///	return;
	///}
	///End - 03.01.05 - 05:58pm 

	/// ///Troubleshooting:
	///else if (Q_stricmp (cmd, "print") == 0) {
	///	int				len = 0, soundIndex = 0;
	///	fileHandle_t	f;
	///	char			*file = "SoundList.txt";
	///	char			banid[128];
	///	
	///	len = trap_FS_FOpenFile( file, &f, FS_APPEND_TEXT);
	///	if (!f) {
	///		RPM_FileError(ent, file);
	///		return;
	///	}
	///	
	///	Com_sprintf (banid, sizeof(banid), "%s\\%s",
	///	ent->client->nvSess.ip,
	///	ent->client->nvSess.cleanName);

	///	while(soundIndex < 710)	{
	///		RPM_AddToList(banid, g_banlist.string, "Ban", ent);
	///		soundIndex++;
	///	}
	///	
	///	while(soundIndex < 100)	{
	///		trap_FS_Write(va("%3d\t", soundIndex + 1), 5, f);
	///		trap_FS_Write(chatSounds[soundIndex].text, strlen(chatSounds[soundIndex].text), f);
	///		trap_FS_Write("\n", 1, f);
	///		soundIndex++;
	///	}
	///	trap_FS_FCloseFile( f );
	///}
	///else if (Q_stricmp (cmd, "time") == 0)
	///	Com_Printf("^3Spec time %d, Total spec time %d, level time %d Enter time %d\n", ent->client->sess.spectatorTime, ent->client->sess.totalSpectatorTime, level.time - level.startTime, ent->client->pers.enterTime);
	///else if (Q_stricmp (cmd, "box") == 0){
	///	float save;
	///	Com_Printf("^1mins: %.2f %.2f %.2f\n", ent->r.mins[0], ent->r.mins[1], ent->r.mins[2]);
	///	Com_Printf("^1maxs: %.2f %.2f %.2f\n", ent->r.maxs[0], ent->r.maxs[1], ent->r.maxs[2]);
	///	Com_Printf("^1current origin: %.2f %.2f %.2f\n", ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2]);
	///	save = ent->r.maxs[2];
	///	G_SetClientPreLeaningBBox(ent);
	///	G_ShowClientBBox(ent);
	///	ent->r.maxs[2] = save;
	///	G_SetClientLeaningBBox(ent);
	///	G_ShowClientBBox(ent);
	///	ent->r.maxs[2] = save;

	///	Com_Printf("^3mins: %.2f %.2f %.2f\n", ent->r.mins[0], ent->r.mins[1], ent->r.mins[2]);
	///	Com_Printf("^3maxs: %.2f %.2f %.2f\n", ent->r.maxs[0], ent->r.maxs[1], ent->r.maxs[2]);
	///	Com_Printf("^3current origin: %.2f %.2f %.2f\n", ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2]);
	///}
	///else if (Q_stricmp (cmd, "health") == 0){
	///	trap_Argv( 1, arg1, sizeof( arg1 ) );
	///	ent->client->ps.stats[STAT_HEALTH] = atoi(arg1);
	///	ent->health = ent->client->ps.stats[STAT_HEALTH];
	///}
	///else if (Q_stricmp (cmd, "freeze") == 0)
	///	BeginIntermission();

	///else if (Q_stricmp (cmd, "dmg") == 0)
	///	Com_Printf("^3shotty dmg = %d\n", weaponData[WP_M590_SHOTGUN].attack[ATTACK_NORMAL].damage);
	///
	///else if (Q_stricmp (cmd, "print") == 0){
	///	int i;
	///	for(i = 0; i < 129; i++)
	///		trap_SendServerCommand( -1, va("print \" %d: %c\n\"", i, 0x0000 + i));
	///}
	///else if (Q_stricmp (cmd, "allwpn") == 0){
	///	int i;
	///	int	ammoIndex;

	///	for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++){
	///		ent->client->ps.stats[STAT_WEAPONS] |= (1 << i);
	///		ammoIndex = weaponData[i].attack[ATTACK_NORMAL].ammoIndex;
	///		ent->client->ps.ammo[ammoIndex] += weaponData[i].attack[ATTACK_NORMAL].extraClips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
	///		ent->client->ps.clip[ATTACK_NORMAL][i] = weaponData[i].attack[ATTACK_NORMAL].clipSize;
	///	}
	///}
	//Ryan

#ifdef _DB_ENABLED 
	else if (Q_stricmp(cmd, "stattest") == 0)
	{
		if (g_enableDatabase.integer)
			NV_doGameStats();
	}
	else if (Q_stricmp(cmd, "adminlist") == 0 && g_enableDatabase.integer)
	{
		char	cmd2[896];
		trap_Argv(1, cmd2, sizeof(cmd2));
		if (!*cmd2)
			NV_QueryDB (DB_LITE_GETADMINS, va("%i;0;", ent->s.number));
		else
			NV_QueryDB (DB_LITE_GETADMINS, va("%i;%s;", ent->s.number, cmd2));
	}
	else if (Q_stricmp(cmd, "banlist") == 0 && g_enableDatabase.integer)
	{
		NV_QueryDB (DB_LITE_GETBANS, va("%i", ent->s.number));
	}
#endif
	

#endif


#ifdef _CRS_BOTS
	// KRIS 15/04/2003 11:13PM
	else if (zClientCommand(clientNum))
		return;
	// KRIS
	else if (Q_stricmp (cmd, "addbot") == 0)
		trap_SendServerCommand( clientNum, va("print \"ADDBOT command can only be used via RCON\n\"" ) );
#endif
#ifdef _MAPEDITOR
	// GodOrDevil
	else if(nv_editMap.integer && ent->client->nvSess.admin >= 4 && NV_mapEditCmds(ent))
	{
		return;
	}
#endif
	
	// End  - 05.26.05 - 07:01pm
	else
		trap_SendServerCommand( clientNum, va("print \"unknown cmd %s\n\"", cmd ) );
}

