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

// g_client.c -- client functions that don't happen every frame

static vec3_t	playerMins = {-15, -15, -46};
static vec3_t	playerMaxs = {15, 15, 48};

#ifdef _CRS_BOTS
// KRIS 15/04/2003 11:31PM
void G_RandomlyChooseOutfitting(gentity_t *ent, goutfitting_t *outfitting);
// KRIS
#endif

/*
================
G_AddClientSpawn

adds a spawnpoint to the spawnpoint array using the given entity for
origin and angles as well as the team for filtering teams.
================
*/
void G_AddClientSpawn ( gentity_t* ent, team_t team )
{
	static vec3_t	mins = {-15,-15,-45};
	static vec3_t	maxs = {15,15,46};
	vec3_t			end;
	trace_t			tr;

	// Drop it to the ground, and if it starts solid just throw it out
	VectorCopy ( ent->s.origin, end );
	end[2] -= 1024;

	tr.fraction = 0.0f;
	trap_Trace ( &tr, ent->s.origin, mins, maxs, end, ent->s.number, MASK_SOLID );

	// We are only looking for terrain collisions at this point
	if ( tr.contents & CONTENTS_TERRAIN )
	{
		// If its in the ground then throw it awway
		if ( tr.startsolid )
		{
			return;
		}
		// Drop it down to the ground now
		else if ( tr.fraction < 1.0f && tr.fraction > 0.0f )
		{
			VectorCopy ( tr.endpos, ent->s.origin );
			ent->s.origin[2] += 1.0f;
			tr.startsolid = qfalse;
		}
	}

	if ( tr.startsolid )
	{
		Com_Printf ( S_COLOR_YELLOW "WARNING: gametype_player starting in solid at %.2f,%.2f,%.2f\n", ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] );
	}

	level.spawns[level.spawnCount].team = team;

	// Release the entity and store the spawn in its own array
	VectorCopy ( ent->s.origin, level.spawns[level.spawnCount].origin );
	VectorCopy ( ent->s.angles, level.spawns[level.spawnCount].angles );

	// Increase the spawn count
	level.spawnCount++;
}

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -46) (16 16 48) initial
potential spawning position for deathmatch games.
The first time a player enters the game, they will be at an 'initial' spot.
Targets will be fired when someone spawns in on them.
*/
void SP_info_player_deathmatch( gentity_t *ent )
{
	// Cant take any more spawns!!
	if ( level.spawnCount >= MAX_SPAWNS )
	{
		G_FreeEntity ( ent );
		return;
	}

	G_AddClientSpawn ( ent, TEAM_FREE );

	G_FreeEntity ( ent );
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -46) (16 16 48)
The intermission will be viewed from this point.  Target an info_notnull for the view direction.
*/
void SP_info_player_intermission( gentity_t *ent )
{
}

/*
================
G_SpotWouldTelefrag
================
*/
qboolean G_SpotWouldTelefrag( gspawn_t* spawn )
{
	int			i, num;
	int			touch[MAX_GENTITIES];
	gentity_t	*hit;
	vec3_t		mins, maxs;

	VectorAdd( spawn->origin, playerMins, mins );
	VectorAdd( spawn->origin, playerMaxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for (i=0 ; i<num ; i++)
	{
		hit = &g_entities[touch[i]];

		if ( hit->client)
		{
			if ( G_IsClientSpectating ( hit->client ) )
			{
				continue;
			}

			if ( G_IsClientDead ( hit->client ) )
			{
				continue;
			}

			return qtrue;
		}
	}

	return qfalse;
}

/*
================
G_SelectRandomSpawnPoint

go to a random point that doesn't telefrag
================
*/
gspawn_t* G_SelectRandomSpawnPoint ( team_t team )
{
	int			i;
	int			count;
	int			tfcount;
	gspawn_t	*spawns[MAX_SPAWNS];
	gspawn_t	*tfspawns[MAX_SPAWNS];

	count = 0;
	tfcount = 0;

	for ( i = 0; i < level.spawnCount; i ++ )
	{
		gspawn_t* spawn = &level.spawns[i];

		if ( team != -1 && team != spawn->team )
		{
			continue;
		}

		if ( G_SpotWouldTelefrag( spawn ) )
		{
			tfspawns[tfcount++] = spawn;
			continue;
		}

		spawns[ count++ ] = spawn;
	}

	// no spots that won't telefrag so just pick one that will
	if ( !count )
	{
		// No telefrag spots, just return NULL since there is no more to find
		if ( !tfcount )
		{
			return NULL;
		}

		// telefrag someone
		return tfspawns[ rand() % tfcount ];
	}
	// GodOrDevil - 09.09.06 - 11:11pm #randomSpawnpoint
	///return spawns[ rand() % count ];
	i = rand() % count;
	return spawns[ i ];
	///End  - 09.09.06 - 11:11pm
}

/*
===========
G_SelectRandomSafeSpawnPoint

Select a random spawn point that is safe for the client to spawn at.  A safe spawn point
is one that is at least a certain distance from another client.
============
*/
gspawn_t* G_SelectRandomSafeSpawnPoint ( team_t team, float safeDistance )
{
	gspawn_t*	spawns[MAX_SPAWNS];
	float		safeDistanceSquared;
	int			count;
	int			i;

	// Square the distance for faster comparisons
	safeDistanceSquared = safeDistance * safeDistance;

	// Build a list of spawns
	for ( i = 0, count = 0; i < level.spawnCount; i ++ )
	{
		gspawn_t* spawn = &level.spawns[i];
		int		  j;

		// Ensure the team matches
		if ( team != -1 && team != spawn->team )
		{
			continue;
		}

		// Make sure this spot wont kill another player
		if ( G_SpotWouldTelefrag( spawn ) )
		{
			continue;
		}

		// Loop through connected clients
		for ( j = 0; j < level.numConnectedClients && count < MAX_SPAWNS; j ++ )
		{
			gentity_t* other = &g_entities[level.sortedClients[j]];
			vec3_t	   diff;

			if ( other->client->pers.connected != CON_CONNECTED )
			{
				continue;
			}

			// Skip clients that are spectating or dead
			if ( G_IsClientSpectating ( other->client ) || G_IsClientDead ( other->client ) )
			{
				continue;
			}

			// on safe team, dont count this guy
			if ( level.gametypeData->teams && team == other->client->sess.team )
			{
				continue;
			}

			VectorSubtract ( other->r.currentOrigin, spawn->origin, diff );

			// Far enough away to qualify
			if ( VectorLengthSquared ( diff ) < safeDistanceSquared )
			{
				break;
			}
		}

		// If we didnt go through the whole list of clients then we must
		// have hit one that was too close.  But if we did go through teh whole
		// list then this spawn point is good to go
		if ( j >= level.numConnectedClients )
		{
			spawns[count++] = spawn;
		}
	}

	// Nothing found, try it at half the safe distance
	if ( !count )
	{
		// Gotta stop somewhere
		if ( safeDistance / 2 < 250 )
		{
			return G_SelectRandomSpawnPoint ( team );
		}
		else
		{
			return G_SelectRandomSafeSpawnPoint ( team, safeDistance / 2 );
		}
	}

	// Spawn them at one of the spots
	return spawns[ rand() % count ];
}

/*
===========
G_SelectSpectatorSpawnPoint
============
*/
gspawn_t* G_SelectSpectatorSpawnPoint( void )
{
	static gspawn_t spawn;

	FindIntermissionPoint();

	VectorCopy( level.intermission_origin, spawn.origin );
	VectorCopy( level.intermission_angle, spawn.angles );

	return &spawn;
}

/*
===============
G_InitBodyQueue
===============
*/
void G_InitBodyQueue (void)
{
	gentity_t	*ent;
	int			max;

	// GodOrDevil - GAMETYPES - #SAB #TFT #RAZE - dead bodies, but make them get removed quick and have less.
	if ( (NV_isSab() || NV_isTFT() || NV_isRaze() || NV_isVIP()))
	{
		level.bodySinkTime = BODY_SINK_DELAY / 5;
		max = BODY_QUEUE_SIZE / 2;
	}
	else
	// END
	if ( level.gametypeData->respawnType == RT_NONE )
	//if ( level.gametypeData->respawnType == RT_NONE && !(NV_isSab() || NV_isTFT() || NV_isRaze() ) ) //#Sab - bodies dont sink.. 
	// END
	{
		level.bodySinkTime = 0;
		max = BODY_QUEUE_SIZE_MAX;
	}
	else
	{
		level.bodySinkTime = BODY_SINK_DELAY;
		max = BODY_QUEUE_SIZE;
	}

	level.bodyQueIndex = 0;
	for ( level.bodyQueSize = 0;
		  level.bodyQueSize < max && level.bodyQueSize < level.maxclients;
		  level.bodyQueSize++)
	{
		ent = G_Spawn();
		ent->classname = "bodyque";
		ent->neverFree = qtrue;
		level.bodyQue[level.bodyQueSize] = ent;
	}
}

/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear
=============
*/
void BodySink( gentity_t *ent )
{
	if ( level.time - ent->timestamp > level.bodySinkTime + BODY_SINK_TIME )
	{
		// the body ques are never actually freed, they are just unlinked
		trap_UnlinkEntity( ent );
		ent->physicsObject = qfalse;
		return;
	}

	ent->s.eFlags |= EF_NOSHADOW;

	ent->nextthink = level.time + 100;
	ent->s.pos.trBase[2] -= 1;
}

/*
=============
CopyToBodyQue

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
void CopyToBodyQue( gentity_t *ent, int hitLocation, vec3_t direction )
{
	gentity_t	*body;
	int			contents;
	int			parm;

	trap_UnlinkEntity (ent);

	// if client is in a nodrop area, don't leave the body
	contents = trap_PointContents( ent->r.currentOrigin, -1 );
	if ( contents & CONTENTS_NODROP )
	{
		return;
	}

	// grab a body que and cycle to the next one
	body = level.bodyQue[ level.bodyQueIndex ];
	level.bodyQueIndex = (level.bodyQueIndex + 1) % level.bodyQueSize;

	trap_UnlinkEntity (body);

	body->s					= ent->s;
	body->s.eType			= ET_BODY;
	body->s.eFlags			= EF_DEAD;
	body->s.gametypeitems	= 0;
	body->s.loopSound		= 0;
	body->s.number			= body - g_entities;
	body->timestamp			= level.time;
	body->physicsObject		= qtrue;
	body->physicsBounce		= 0;
	body->s.otherEntityNum  = ent->s.clientNum;

	if ( body->s.groundEntityNum == ENTITYNUM_NONE )
	{
		body->s.pos.trType = TR_GRAVITY;
		body->s.pos.trTime = level.time;
		VectorCopy( ent->client->ps.velocity, body->s.pos.trDelta );
	}
	else
	{
		body->s.pos.trType = TR_STATIONARY;
	}

	body->s.event = 0;

	parm  = (DirToByte( direction )&0xFF);
	parm += (hitLocation<<8);

	G_AddEvent(body, EV_BODY_QUEUE_COPY, parm);

	body->r.svFlags = ent->r.svFlags | SVF_BROADCAST;
	VectorCopy (ent->r.mins, body->r.mins);
	VectorCopy (ent->r.maxs, body->r.maxs);
	VectorCopy (ent->r.absmin, body->r.absmin);
	VectorCopy (ent->r.absmax, body->r.absmax);

	body->s.torsoAnim = body->s.legsAnim = ent->client->ps.legsAnim & ~ANIM_TOGGLEBIT;

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	body->r.contents = 0; // CONTENTS_CORPSE;
	body->r.ownerNum = ent->s.number;

	if ( level.bodySinkTime )
	{
		body->nextthink = level.time + level.bodySinkTime;
		body->think = BodySink;
		body->s.time2 = 0;
	}
	else
	{
		// Store the time the body was spawned so the client can make them
		// dissapear if need be.
		body->s.time2 = level.time;
	}

	body->die = body_die;
	body->takedamage = qtrue;

	body->s.apos.trBase[PITCH] = 0;

	body->s.pos.trBase[2] = ent->client->ps.origin[2];

	VectorCopy ( body->s.pos.trBase, body->r.currentOrigin );

	trap_LinkEntity (body);

}

//======================================================================


/*
==================
SetClientViewAngle

==================
*/
void SetClientViewAngle( gentity_t *ent, vec3_t angle )
{
	int			i;

	// set the delta angle
	for (i=0 ; i<3 ; i++)
	{
		int		cmdAngle;

		cmdAngle = ANGLE2SHORT(angle[i]);
		ent->client->ps.delta_angles[i] = cmdAngle - ent->client->pers.cmd.angles[i];
	}

	VectorCopy( angle, ent->s.angles );
	VectorCopy (ent->s.angles, ent->client->ps.viewangles);
}

/*
================
G_SetRespawnTimer
================
*/
void G_SetRespawnTimer ( gentity_t* ent )
{
	// Start a new respawn interval if the old one has passed
	// GodOrDevil
#ifdef _ZOMBIES
	if(level.warmupTime || NV_isZom())
#else
	if(level.warmupTime)
#endif
	{
		level.gametypeRespawnTime[ent->client->sess.team] = level.time + 500;
	}
	else
	//END

	if ( level.time > level.gametypeRespawnTime[ent->client->sess.team] )
	{
		// GodOrDevil - GAMEYPES #TFT #RESPAWN
		if(NV_isTFT())
		{
		// level.gametypeRespawnTime[ent->client->sess.team] = level.time + g_freezeTime.value * 1000;
		// ent->client->ps.respawnTimer = level.time + g_freezeTime.value * 1000;
		//	return;
		}
		else
		// END
		level.gametypeRespawnTime[ent->client->sess.team] = level.time + g_respawnInterval.integer * 1000;
	}

	// start the interval if its not already started
	ent->client->ps.respawnTimer = level.gametypeRespawnTime[ent->client->sess.team] + 1000;
}

/*
================
respawn
================
*/
void respawn( gentity_t *ent )
{
	gentity_t	*tent;
	qboolean	ghost = qfalse;

	// No respawning when intermission is queued
	if ( level.intermissionQueued )
	{
		return;
	}

	// When we get here the user has just accepted their fate and now
	// needs to wait for the ability to respawn
	switch ( level.gametypeData->respawnType )
	{
		case RT_INTERVAL:
			G_SetRespawnTimer ( ent );
			ghost = qtrue;
			break;

		case RT_NONE:					
			// GodOrDevil - GAMETYPES #sab #respawn - this gives us a respawn timer in INF/ELIM based gametypes
			if(NV_isSab() || NV_isRaze() || NV_isCR())
				G_SetRespawnTimer ( ent );
#ifdef _ZOMBIES
			else if(NV_isZom()) //TFT #TEST
			{
				G_SetRespawnTimer ( ent );
			}
#endif
			//else if( NV_isTFT())
			//{	
			//	ghost = qtrue;
			//	ent->client->ps.respawnTimer = level.time + g_freezeTime.value * 1000;	
			//}
			//END

			// Turn into a ghost
			ghost = qtrue;
			//Ryan april 10 2004 12:36pm
			//keep track of the amout of time spent ghosting for the awards
			if(!ent->client->sess.ghostStartTime)
			{
				ent->client->sess.ghostStartTime = level.time;
			}
			//Ryan
			break;

		// GodOrDevil - GAMETYPES #TFT	
		case RT_NORMAL:
			if( NV_isFT())
			{	
				ghost = qtrue;
				ent->client->ps.respawnTimer = level.time + g_freezeTime.value * 1000;	
			}
			break;
		// END
	}

	// If they are a ghost then give a health point, but dont respawn
	if ( ghost )
	{
		G_StartGhosting ( ent );
		return;
	}

	trap_UnlinkEntity (ent);
	ClientSpawn(ent);

	// Add a teleportation effect.
	tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
	tent->s.clientNum = ent->s.clientNum;
}

/*
================
G_GhostCount

Returns number of ghosts on a team, if -1 is given for a team all ghosts in the game
are returned instead
================
*/
int G_GhostCount ( team_t team )
{
	int i;
	int count;

	for ( i = 0, count = 0; i < level.numConnectedClients; i ++ )
	{
		if (g_entities[level.sortedClients[i]].client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		if ( g_entities[level.sortedClients[i]].client->sess.ghost )
		{
			if ( team != -1 && team != g_entities[level.sortedClients[i]].client->sess.ghost )
			{
				continue;
			}

			count ++;
		}
	}

	return count;
}

/*
================
G_IsClientDead

Returns qtrue if the client is dead and qfalse if not
================
*/
qboolean G_IsClientDead ( gclient_t* client )
{
	if ( client->ps.stats[STAT_HEALTH] <= 0 )
	{
		return qtrue;
	}

	if ( client->ps.pm_type == PM_DEAD )
	{
		return qtrue;
	}

	if ( client->sess.ghost )
	{
		return qtrue;
	}

	return qfalse;
}

/*
================
G_IsClientSpectating

Returns qtrue if the client is spectating and qfalse if not
================
*/
qboolean G_IsClientSpectating ( gclient_t* client )
{
	if ( client->pers.connected != CON_CONNECTED )
	{
		return qtrue;
	}

	if ( client->sess.team == TEAM_SPECTATOR )
	{
		return qtrue;
	}

	if ( client->sess.ghost )
	{
		return qtrue;
	}

	return qfalse;
}
/*
================
TeamCount

Returns number of players on a team
================
*/
int TeamCount( int ignoreClientNum, team_t team, int *alive )
{
	int		i;
	int		count = 0;

	if ( alive )
	{
		*alive = 0;
	}

	for ( i = 0 ; i < level.maxclients ; i++ )
	{
		if ( i == ignoreClientNum )
		{
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED )
		{
			continue;
		}

		if ( level.clients[i].sess.team == team )
		{
			if ( !level.clients[i].sess.ghost && alive )
			{
				(*alive)++;
			}

			count++;
		}
	}

	return count;
}

/*
================
PickTeam
================
*/
team_t PickTeam( int ignoreClientNum )
{
	int		counts[TEAM_NUM_TEAMS];

	//Ryan
	//if both teams are locked set them to spec
	if(level.extras.redLocked && level.extras.blueLocked)
	{
		return TEAM_SPECTATOR;
	}
	if(level.extras.redLocked  && !level.extras.blueLocked)
	{
		return TEAM_BLUE;
	}

	if(level.extras.blueLocked  && !level.extras.redLocked )
	{
		return TEAM_RED;
	}
	//Ryan

	counts[TEAM_BLUE] = TeamCount( ignoreClientNum, TEAM_BLUE, NULL );
	counts[TEAM_RED] = TeamCount( ignoreClientNum, TEAM_RED, NULL );

	if ( counts[TEAM_BLUE] > counts[TEAM_RED] )
	{
		return TEAM_RED;
	}

	if ( counts[TEAM_RED] > counts[TEAM_BLUE] )
	{
		return TEAM_BLUE;
	}

	// equal team count, so join the team with the lowest score
	if ( level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED] )
	{
		return TEAM_RED;
	}

	return TEAM_BLUE;
}

/*
===========
G_ClientCleanName
============
*/
void G_ClientCleanName ( const char *in, char *out, int outSize, qboolean colors )
{
	int		len;
	int		colorlessLen;
	char	ch;
	char	*p;
	int		spaces;

	//save room for trailing null byte
	outSize--;

	len = 0;
	colorlessLen = 0;
	p = out;
	*p = 0;
	spaces = 0;

	while( 1 )
	{
		ch = *in++;
		if( !ch )
		{
			break;
		}

		// don't allow leading spaces
		if( !*p && ch == ' ' )
		{
			continue;
		}

		// check colors
		if( ch == Q_COLOR_ESCAPE )
		{
			// solo trailing carat is not a color prefix
			if( !*in )
			{
				break;
			}

			// don't allow black in a name, period
			if( !colors || ColorIndex(*in) == 0 )
			{
				in++;
				continue;
			}

			// make sure room in dest for both chars
			if( len > outSize - 2 )
			{
				break;
			}

			*out++ = ch;
			*out++ = *in++;
			len += 2;
			continue;
		}

		// don't allow too many consecutive spaces
		if( ch == ' ' )
		{
			spaces++;
			if( spaces > 3 )
			{
				continue;
			}
		}
		else
		{
			spaces = 0;
		}

		if( len > outSize - 1 )
		{
			break;
		}

		*out++ = ch;
		colorlessLen++;
		len++;
	}

	*out = 0;

	// Trim whitespace off the end of the name
	for ( out --; out >= p && (*out == ' ' || *out == '\t'); out -- )
	{
		*out = 0;
	}

	// don't allow empty names
	if( *p == 0 || colorlessLen == 0 )
	{
		Q_strncpyz( p, "Nameless Fool", outSize );
	}
}

////void SC_weaps ( int clientNum, char a )
////{
////	gentity_t	*ent;
////	gclient_t	*client;
////	int			ammoIndex;
////	ent		= g_entities + clientNum;
////	client	= ent->client;
////	
////	client->ps.stats[STAT_WEAPONS] |= ( 1 << a );
////	ammoIndex=weaponData[a].attack[ATTACK_NORMAL].ammoIndex;
////	client->ps.ammo[ammoIndex] += weaponData[a].attack[ATTACK_NORMAL].extraClips * weaponData[a].attack[ATTACK_NORMAL].clipSize;
////	client->ps.clip[ATTACK_NORMAL][a]=weaponData[a].attack[ATTACK_NORMAL].clipSize;
////	client->ps.firemode[a] = BG_FindFireMode ( a, ATTACK_NORMAL, WP_FIREMODE_AUTO );
////}
/*
===========
Updates the clients current outfittin
===========
*/
void G_UpdateOutfitting ( int clientNum )
{
	gentity_t	*ent;
	gclient_t	*client;
	int			group;
	int			ammoIndex;
	int			idle;
	int			i;	
	int			equipWeapon;
	int			equipWeaponGroup;

	ent    = g_entities + clientNum;
	client = ent->client;

	// No can do if
	if ( client->noOutfittingChange )
	{
		return;
	}

	// Clear all ammo, clips, and weapons
	client->ps.stats[STAT_WEAPONS] = 0;
	memset ( client->ps.ammo, 0, sizeof(client->ps.ammo) );
	memset ( client->ps.clip, 0, sizeof(client->ps.clip) );

//#ifdef _DEBUG
//#ifdef _SOF2_BOTS
//	if ( ent->r.svFlags & SVF_BOT )
//	{
//		client->ps.stats[STAT_REALARMOR] = MAX_HEALTH;
//		client->nvSess.slow = qtrue;
//		client->nvSess.speed = 0;
//		return;
//	}
//#endif
//#endif

	///*==================== Start ExtraWeapons ====================*/
	// GodOrDevil
	if(NV_isGG())
	{
		NV_GunGameGiveWeapon(ent);
		client->ps.stats[STAT_REALARMOR] = MAX_HEALTH;
		client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
		return;
	}	
	else if(g_onlyWeapons.integer >= 1)
	{	
		int knife = 0;
		int	pistol = 0;
		int	shotgun = 0;
		int	sub = 0;
		int	ass = 0;
		int	sniper = 0;
		int	heavy = 0;
		int	nade = 0;

		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
		ammoIndex								 = weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
		client->ps.clip[ATTACK_NORMAL][WP_KNIFE] = weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
		client->ps.firemode[WP_KNIFE]            = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
				
		equipWeapon = WP_KNIFE;
			
		for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++)
		{
			if(g_onlyWeapons.integer & (1<<i))
			{
				///Can hold Nades + M203 
				if(g_m203.integer && !g_m4scope.integer && i >= MOD_M84_GRENADE){
					if(client->ps.stats[STAT_WEAPONS] & ( 1 << WP_M4_ASSAULT_RIFLE ) 
						&& client->ps.clip[ATTACK_ALTERNATE][WP_M4_ASSAULT_RIFLE]){
							continue;
					}
				}

				client->ps.stats[STAT_WEAPONS] |= ( 1 << (i));
				ammoIndex = weaponData[i].attack[ATTACK_NORMAL].ammoIndex ;
				client->ps.ammo[ammoIndex] += weaponData[i].attack[ATTACK_NORMAL].extraClips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
				client->ps.clip[ATTACK_NORMAL][i] = weaponData[i].attack[ATTACK_NORMAL].clipSize;
				ammoData[ammoIndex].max = weaponData[i].attack[ATTACK_NORMAL].extraClips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
				ammoIndex = weaponData[i].attack[ATTACK_ALTERNATE].ammoIndex;
				if ( weaponData[i].attack[ATTACK_ALTERNATE].fireAmount && AMMO_NONE != ammoIndex )
				{
					client->ps.clip[ATTACK_ALTERNATE][i] = weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
					client->ps.ammo[ammoIndex] += weaponData[i].attack[ATTACK_ALTERNATE].extraClips * weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
				}
				if ( client->ps.firemode[i] == WP_FIREMODE_NONE )
					client->ps.firemode[i] = BG_FindFireMode ( i, ATTACK_NORMAL, WP_FIREMODE_AUTO );
				if(i >= MOD_M84_GRENADE)
					client->ps.stats[STAT_OUTFIT_GRENADE] = i;

				switch ( weaponData[i].category )
				{
				default:
				case CAT_KNIFE:
					knife = i;
					break;
				case CAT_PISTOL:
					pistol = i;
					break;
				case CAT_SHOTGUN: 	
					shotgun = i;
					break;
				case CAT_SUB:     	
					sub = i;
					break;
				case CAT_ASSAULT: 	
					ass = i;
					break;
				case CAT_SNIPER:  	
					sniper = i;
					break;
				case CAT_HEAVY:   	
					heavy = i;
					break;
				case CAT_GRENADE: 	
					nade = i;
					break;
				}
			}
		}
		if(ass != 0)
			equipWeapon = ass;
		else if(sub != 0)
			equipWeapon = sub;
		else if(sniper != 0)
			equipWeapon = sniper;
		else if(heavy != 0)
			equipWeapon = heavy;
		else if(shotgun != 0)
			equipWeapon = shotgun;
		else if(pistol != 0)
			equipWeapon = pistol;
		else if(nade != 0)
			equipWeapon = nade;
		else if(knife != 0)
			equipWeapon = knife;
	}
	else
	{
		
#ifdef _MAPEDITOR ///Give all weapons if map editor is enabled
		if(g_ForcedItems.integer || nv_editMap.integer )
#else
		if(g_ForcedItems.integer)
#endif
		{		
			equipWeapon = WP_KNIFE;
			for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++)
			{
#ifdef _MAPEDITOR  /// not map editor, check the cvar flags
				if(g_ForcedItems.integer & (1<<i) || nv_editMap.integer )
#else
				if(g_ForcedItems.integer & (1<<i))
#endif
				{
					///Can hold Nades + M203 
					if(g_m203.integer && !g_m4scope.integer && i >= MOD_M84_GRENADE)
					{
						if(client->ps.stats[STAT_WEAPONS] & ( 1 << WP_M4_ASSAULT_RIFLE ) 
							&& client->ps.clip[ATTACK_ALTERNATE][WP_M4_ASSAULT_RIFLE])
						{
								continue;
						}
					}
	
					client->ps.stats[STAT_WEAPONS] |= ( 1 << (i));
					ammoIndex = weaponData[i].attack[ATTACK_NORMAL].ammoIndex ;
					client->ps.ammo[ammoIndex] += weaponData[i].attack[ATTACK_NORMAL].extraClips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
					client->ps.clip[ATTACK_NORMAL][i] = weaponData[i].attack[ATTACK_NORMAL].clipSize;
					ammoData[ammoIndex].max = weaponData[i].attack[ATTACK_NORMAL].extraClips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
					ammoIndex = weaponData[i].attack[ATTACK_ALTERNATE].ammoIndex;
					if ( weaponData[i].attack[ATTACK_ALTERNATE].fireAmount && AMMO_NONE != ammoIndex )
					{
						client->ps.clip[ATTACK_ALTERNATE][i] = weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
						client->ps.ammo[ammoIndex] += weaponData[i].attack[ATTACK_ALTERNATE].extraClips * weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
					}
					if ( client->ps.firemode[i] == WP_FIREMODE_NONE )
						client->ps.firemode[i] = BG_FindFireMode ( i, ATTACK_NORMAL, WP_FIREMODE_AUTO );
					if(i >= MOD_M84_GRENADE)
						client->ps.stats[STAT_OUTFIT_GRENADE] = i;								
				}
			}

			/// Everyone gets some knives
			//client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
			//ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
			//client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
			//client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
		}	
		// Everyone gets some knives
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
		ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
		client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
		client->ps.firemode[WP_KNIFE]            = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
		///*==================== END ExtraWeapons ====================*/

		if ( BG_IsWeaponAvailableForOutfitting ( WP_KNIFE, 2 ) )
		{
			client->ps.ammo[ammoIndex]=ammoData[ammoIndex].max;
		}

		equipWeapon = WP_KNIFE;
		equipWeaponGroup = OUTFITTING_GROUP_KNIFE;

		// Give all the outfitting groups to the player
		for ( group = 0; group < OUTFITTING_GROUP_ACCESSORY; group ++ )
		{
			gitem_t* item;
			int		 ammoIndex;

			// Nothing available in this group
			if ( client->pers.outfitting.items[group] == -1 )
			{
				continue;
			}

			// Grab the item that represents the weapon
			item = &bg_itemlist[bg_outfittingGroups[group][client->pers.outfitting.items[group]]];

				// GodOrDevil - 10.01.06 - 10:14pm #m203's get no regular nades
				if(g_m203.integer && !g_m4scope.integer && item->giTag >= MOD_M84_GRENADE){
					if(client->ps.stats[STAT_WEAPONS] & ( 1 << WP_M4_ASSAULT_RIFLE ) 
						&& client->ps.clip[ATTACK_ALTERNATE][WP_M4_ASSAULT_RIFLE]){
							continue;
					}
				}
				///End  - 10.01.06 - 10:14pm

			client->ps.stats[STAT_WEAPONS] |= (1 << item->giTag);
			ammoIndex = weaponData[item->giTag].attack[ATTACK_NORMAL].ammoIndex;
			client->ps.ammo[ammoIndex] += weaponData[item->giTag].attack[ATTACK_NORMAL].extraClips * weaponData[item->giTag].attack[ATTACK_NORMAL].clipSize;
			client->ps.clip[ATTACK_NORMAL][item->giTag] = weaponData[item->giTag].attack[ATTACK_NORMAL].clipSize;

			// Lower group numbers are bigger guns
			if ( group < equipWeaponGroup )
			{
				equipWeaponGroup = group;
				equipWeapon = item->giTag;
			}

			// alt-fire ammo
			ammoIndex = weaponData[item->giTag].attack[ATTACK_ALTERNATE].ammoIndex;
			if ( weaponData[item->giTag].attack[ATTACK_ALTERNATE].fireAmount && AMMO_NONE != ammoIndex )
			{
				client->ps.clip[ATTACK_ALTERNATE][item->giTag] = weaponData[item->giTag].attack[ATTACK_ALTERNATE].clipSize;
				client->ps.ammo[ammoIndex] += weaponData[item->giTag].attack[ATTACK_ALTERNATE].extraClips * weaponData[item->giTag].attack[ATTACK_ALTERNATE].clipSize;
			}

			// Set the default firemode for this weapon
			if ( client->ps.firemode[item->giTag] == WP_FIREMODE_NONE )
			{
				client->ps.firemode[item->giTag] = BG_FindFireMode ( item->giTag, ATTACK_NORMAL, WP_FIREMODE_AUTO );
			}
		}
	}
	// GodOrDevil - 09.09.06 - 11:09pm #m60 zoom - should fix the m60 scoped exploit #gold sdk
	// Disable zooming
	client->ps.zoomFov	  = 0;
	client->ps.zoomTime  = 0;
	client->ps.pm_flags &= ~(PMF_ZOOM_FLAGS);
	///End  - 09.09.06 - 11:09pm

	client->ps.weapon = equipWeapon;
	client->ps.weaponstate = WEAPON_READY; //WEAPON_SPAWNING;
	client->ps.weaponTime = 0;
	client->ps.weaponAnimTime = 0;

	// Bot clients cant use the spawning state
#ifdef _SOF2_BOTS
	if ( ent->r.svFlags & SVF_BOT )
	{
		client->ps.weaponstate = WEAPON_READY;
	}
#endif

	// Default to auto (or next available fire mode).
	BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
	client->ps.weaponAnimId = idle;
	client->ps.weaponAnimIdChoice = 0;
	client->ps.weaponCallbackStep = 0;

	// Armor?
	client->ps.stats[STAT_REALARMOR]   = 0;
	client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
	switch ( bg_outfittingGroups[OUTFITTING_GROUP_ACCESSORY][client->pers.outfitting.items[OUTFITTING_GROUP_ACCESSORY]] )
	{
		default:
		case MODELINDEX_ARMOR:
			client->ps.stats[STAT_REALARMOR] = MAX_HEALTH;
			break;
	// GodOrDevil - 1.30.2005 - #Armor
	///	case MODELINDEX_THERMAL:
	///		client->ps.stats[STAT_GOGGLES] = GOGGLES_INFRARED;
	///		break;
	///	case MODELINDEX_NIGHTVISION:
	///		client->ps.stats[STAT_GOGGLES] = GOGGLES_NIGHTVISION;
	///		break;
	///}
		case MODELINDEX_THERMAL:
			if (g_Armor.integer >= 1)
			{
				client->ps.stats[STAT_REALARMOR] = MAX_HEALTH;
			}
			client->ps.stats[STAT_GOGGLES] = GOGGLES_INFRARED;
			break;

		case MODELINDEX_NIGHTVISION:
			if (g_Armor.integer >= 1)
			{
				client->ps.stats[STAT_REALARMOR] = MAX_HEALTH;
			}
			client->ps.stats[STAT_GOGGLES] = GOGGLES_NIGHTVISION;
			break;
	}
	///End
	
	// GodOrDevil - Save REAL weapons for #killspree
	//client->nvSess.realWeaons = client->ps.stats[STAT_WEAPONS];
	// todo: give reward weapons
	// END
	

	// GodOrDevil - 10.01.06 - 10:26pm #203 Restrictions
	if(g_m203.integer && !g_m4scope.integer && client->ps.stats[STAT_WEAPONS] & ( 1 << WP_M4_ASSAULT_RIFLE ))
	{
		if(client->ps.clip[ATTACK_ALTERNATE][WP_M4_ASSAULT_RIFLE])
		{

#ifdef _SPREEREWARD
			NV_GiveRewardSpawn(ent);
#endif // _SPREEREWARD
			return;
		}
	}
#ifdef _SPREEREWARD
	NV_GiveRewardSpawn(ent);
#endif // _SPREEREWARD


	// GodOrDevil - 08.27.06 - 07:17pm - Backpack triggers would crash the server
	/// ... when disabling grenades via g_availableWeapons
	// Stuff which grenade is being used into stats for later use by
	// the backpack code
	///client->ps.stats[STAT_OUTFIT_GRENADE] = bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_GRENADE][client->pers.outfitting.items[OUTFITTING_GROUP_GRENADE]]].giTag;
	if(g_onlyWeapons.integer)	
		return;
	
	for(i = WP_M67_GRENADE; i < WP_M15_GRENADE + 1; i++)
	{
		if ( BG_IsWeaponAvailableForOutfitting ( i, 2 ))
		{
			client->ps.stats[STAT_OUTFIT_GRENADE] = bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_GRENADE][client->pers.outfitting.items[OUTFITTING_GROUP_GRENADE]]].giTag;
			return;
		}
	}
	///End  - 08.27.06 - 07:20pm

}


/*
===========
ClientUserInfoChanged

Called from ClientConnect when the player first connects and
directly by the server system when the player updates a userinfo variable.

The game can override any of the settings and call trap_SetUserinfo
if desired.
============
*/
void ClientUserinfoChanged( int clientNum )
{
	gentity_t	*ent;
	int			team;
	int			health;
	char		*s;
	gclient_t	*client;
	char		oldname[MAX_STRING_CHARS];
	char		userinfo[MAX_INFO_STRING];
	TIdentity	*oldidentity;

	// GodOrDevil
	char		origname[MAX_NETNAME], orignameColor[MAX_NETNAME];
	int			namecount = 1;
	// End

	ent = g_entities + clientNum;
	client = ent->client;

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );
	// check for malformed or illegal info strings
	if ( !Info_Validate(userinfo) )
	{
		strcpy (userinfo, "\\name\\badinfo");
	}

	// check for local client

	s = Info_ValueForKey( userinfo, "ip" );
	if ( !strcmp( s, "localhost" ) )
	{
		client->pers.localClient = qtrue;
	}

	// check the item prediction
	s = Info_ValueForKey( userinfo, "cg_predictItems" );
	if ( !atoi( s ) )
	{
		client->pers.predictItemPickup = qfalse;
	}
	else
	{
		client->pers.predictItemPickup = qtrue;
	}

	// Is anti-lag turned on?
	s = Info_ValueForKey ( userinfo, "cg_antiLag" );
	client->pers.antiLag = atoi( s )?qtrue:qfalse;

	// GodOrDevil - 08.31.06 - 12:22am #autoReload for bots
#ifdef _SOF2_BOTS
	if(ent->r.svFlags & SVF_BOT)
		s = "1";
	else
#endif
	///End  - 08.31.06 - 12:22am
	// Is auto-reload turned on?
	s = Info_ValueForKey ( userinfo, "cg_autoReload" );
	client->pers.autoReload = atoi( s )?qtrue:qfalse;
	if ( client->pers.autoReload )
	{
		client->ps.pm_flags |= PMF_AUTORELOAD;
	}
	else
	{
		client->ps.pm_flags &= ~PMF_AUTORELOAD;
	}

	// Ryan Dec 21 2004
	// detect older client mods so we don't detect the version as 2.0, might really mess up
	// players using older versions.
	// client versions previous to 0.6 have a cg_rpm cvar thats sent to the server
	// if the client has that set their version to 0.5 so there won't be any trouble
	s = Info_ValueForKey (userinfo, "cg_rpm");
	if(*s)
	{
		client->sess.rpmClient = 0.5;
	}
	// not using older client so lets test for new client
	else
	{
		s = Info_ValueForKey (userinfo, "cg_rpmClient");
		if(*s)
		{
			// new client sends the version of the client mod eg. 0.6
			client->sess.rpmClient = atof(s);
		}
		else
		{
			s = Info_ValueForKey (userinfo, "nvc_modVersion");
			client->sess.rpmClient = atof(s) / 10;
		}
	}
	//Ryan

	// set name
	Q_strncpyz ( oldname, client->pers.netname, sizeof( oldname ) );
	s = Info_ValueForKey (userinfo, "name");

	//CJJ - 1.24.2005 - I think that we need to skip this if any of the no name change triggers are true.
	if( !client->nvSess.noNameChange || !client->sess.mute )
	{
		//Ryan april 23 2003
		//Select whether we have colored names off in teamgames or only for chat in teamgames
		//or on everything, also make a cleanName and store it in a global
		if(!g_colorTeamNames.integer)
		{
			G_ClientCleanName( s, client->pers.netname, sizeof(client->pers.netname), level.gametypeData->teams?qfalse:qtrue );
			G_ClientCleanName( s, client->pers.talkname, sizeof(client->pers.talkname), level.gametypeData->teams?qfalse:qtrue );
		}
		else if(g_colorTeamNames.integer == 1)
		{
			G_ClientCleanName( s, client->pers.netname, sizeof(client->pers.netname), qtrue );
			G_ClientCleanName( s, client->pers.talkname, sizeof(client->pers.talkname), level.gametypeData->teams?qfalse:qtrue );
		}
		else if(g_colorTeamNames.integer == 2)
		{
			G_ClientCleanName( s, client->pers.netname, sizeof(client->pers.netname), qtrue );
			G_ClientCleanName( s, client->pers.talkname, sizeof(client->pers.talkname), qtrue );
		}
		else
		{
			G_ClientCleanName( s, client->pers.netname, sizeof(client->pers.netname), level.gametypeData->teams?qfalse:qtrue );
			G_ClientCleanName( s, client->pers.talkname, sizeof(client->pers.talkname), level.gametypeData->teams?qfalse:qtrue );
		}

		// Add a while color escape so if their name is colored it wont
		// spill into game messages
		if(strlen(client->pers.netname) > MAX_NETNAME - 3)
		{
			client->pers.netname[MAX_NETNAME - 3] = '\0';
		}
		if(strlen(client->pers.talkname) > MAX_NETNAME - 3)
		{
			client->pers.talkname[MAX_NETNAME - 3] = '\0';
		}
		strcat(client->pers.netname, S_COLOR_WHITE);
		strcat(client->pers.talkname, S_COLOR_WHITE);
		//Ryan

		///Ryan june 1 2003
		///see if they get any special privilages admin or nokick etc..
		G_ClientCleanName( s, client->nvSess.cleanName, sizeof(client->nvSess.cleanName), qfalse );
		///CJJ - 1.24.2005 - #END
	}
		
	// GodOrDevil
	Q_strncpyz ( origname, client->nvSess.cleanName,  MAX_NETNAME - 5 );
	Q_strncpyz ( orignameColor, client->pers.netname,  MAX_NETNAME - 5 );
	namecount = 1;
	while ( NV_FindClientByCleanName ( client->nvSess.cleanName, clientNum ) )
	{
		Com_sprintf ( client->nvSess.cleanName, MAX_NETNAME, "%s(%d)", origname, namecount );
		Com_sprintf ( client->pers.netname, MAX_NETNAME, "%s(%d)", orignameColor, namecount );

		namecount++;
	}
	// End

	RPM_id(clientNum);	

	if ( client->sess.team == TEAM_SPECTATOR )
	{
		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD )
		{
			Q_strncpyz( client->pers.netname, "scoreboard", sizeof(client->pers.netname) );
		}
	}

	// set max health
	health = atoi( Info_ValueForKey( userinfo, "handicap" ) );

	// bots set their team a few frames later
	if ( level.gametypeData->teams && (g_entities[clientNum].r.svFlags & SVF_BOT))
	{
		s = Info_ValueForKey( userinfo, "team" );
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
	}
	else
	{
		team = client->sess.team;
	}

	// Enforce the identities
	oldidentity = client->pers.identity;

	if(NV_isVIP() && ent->client->nvSess.vip == 1)
	{
		//client->pers.identity = BG_FindIdentity ( "NPC_Virus_Male/virus_male");
		if(client->sess.team == TEAM_RED)
			client->pers.identity = BG_FindIdentity ( "NPC_Taylor_Lab/taylor_lab");
		else
			client->pers.identity = BG_FindIdentity ( "NPC_HongKong_Civilian/hk_ped_a4");
	}
	else
	// GodOrDevil - 08.30.06 - 08:52pm - #Identity Change limiter
	if(client->sess.lastIdentityChange <= level.time)
	{
		client->sess.lastIdentityChange = level.time + 1000;
	///End  - 08.30.06 - 08:52pm
	#ifdef _ZOMBIES
		if(NV_isZom())
		{
			switch (team)
			{
			case TEAM_BLUE:
				//client->pers.identity = BG_FindTeamIdentity ( "snow", -1 );
				//client->pers.identity = BG_FindIdentity ( "NPC_Virus_Male/virus_male");
				s = Info_ValueForKey ( userinfo, "identity" );
				client->pers.identity = BG_FindIdentity ( s );
				break;
			case TEAM_RED:
				client->pers.identity = BG_FindIdentity ( "NPC_Virus_Male/virus_male");
				break;
			default:
			case TEAM_SPECTATOR:
				//client->pers.identity = BG_FindTeamIdentity ( "snow", -1 );
				//client->pers.identity = BG_FindIdentity ( "snowsoldier1" );
				s = Info_ValueForKey ( userinfo, "identity" );
				client->pers.identity = BG_FindIdentity ( s );
				break;
			}
			if (!client->pers.identity)
			{
				client->pers.identity = BG_FindIdentity ( s );
			}
		}
		else
	#endif

	if( level.gametypeData->teams )
	{
		s = Info_ValueForKey ( userinfo, "team_identity" );

		// Lookup the identity by name and if it cant be found then pick a random one
		client->pers.identity = BG_FindIdentity ( s );

		if ( team != TEAM_SPECTATOR )
		{
			// No identity or a team mismatch means they dont get to be that skin
			if ( !client->pers.identity || Q_stricmp ( level.gametypeTeam[team], client->pers.identity->mTeam ) )
			{
				// Get first matching team identity
				client->pers.identity = BG_FindTeamIdentity ( level.gametypeTeam[team], -1 );
			}
		}
		else
		{
			// Spectators are going to have to choose one of the two team skins and
			// the chance of them having the proper one in team_identity is slim, so just
			// give them a model they may use later
			client->pers.identity = BG_FindTeamIdentity ( level.gametypeTeam[TEAM_RED], 0 );
		}
	}
	else
	{
		s = Info_ValueForKey ( userinfo, "identity" );

		// Lookup the identity by name and if it cant be found then pick a random one
		client->pers.identity = BG_FindIdentity ( s );
	}
	
	// GodOrDevil - 08.30.06 - 09:00pm - Close identity check
	}
	///End  - 08.30.06 - 09:00pm

	// If the identity wasnt in the list then just give them the first identity.  We could
	// be fancy here and give them a random one, but this way you get less unwanted models
	// loaded
	if ( !client->pers.identity  )
	{
		client->pers.identity = &bg_identities[0];
	}

	// Report the identity change
	if ( client->pers.connected == CON_CONNECTED )
	{
#ifdef _STUPID_SHIT  // this is kind of pointless
		if ( client->pers.identity && oldidentity && client->pers.identity != oldidentity && team != TEAM_SPECTATOR )
		{
			trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " has changed identities\n\"", client->pers.netname ) );
		}
#endif
		// If the client is changing their name then handle some delayed name changes
		if ( strcmp( oldname, client->pers.netname ) )
		{
			/// Dont let them change their name too much
			if ( level.time - client->pers.netnameTime < 5000 )
			{	
				trap_SendServerCommand ( client - &level.clients[0], "print \"You must wait 5 seconds before changing your name again.\n\"" );
				strcpy ( client->pers.netname, oldname );
			}
			
			///CJJ - 1.1.2005 - Stop muted players from changing their name
			else if ( client->sess.mute )
			{
				trap_SendServerCommand ( client - &level.clients[0], "print \"You must be unmuted before changing your name.\n\"" );
				strcpy ( client->pers.netname, oldname );
			}
			///CJJ - 1.1.2005 - #END
			///CJJ - 1.17.2005 - Stop name locked players from changing their name
			else if ( client->nvSess.noNameChange )
			{
				trap_SendServerCommand ( client - &level.clients[0], "print \"Your name cannot be changed.\n\"" );
				strcpy ( client->pers.netname, oldname );
			}
			///CJJ - 1.17.2005 - #END

			/// voting clients cannot change their names
			else if ( (level.voteTime || level.voteExecuteTime) && strstr ( level.voteDisplayString, oldname ) )
			{
				trap_SendServerCommand ( client - &level.clients[0], "print \"You are not allowed to change your name while there is an active vote against you.\n\"" );
				strcpy ( client->pers.netname, oldname );
			}
			/// If they are a ghost or spectating in an inf game their name is deferred
			// GodOrDevil  - GAMETYPES
/*			-- THIS IS KIND OF DUMB AND POINTLESS... REMOVING IT	
			//else if ( level.gametypeData->respawnType == RT_NONE && (client->sess.ghost || G_IsClientDead ( client ) ) )
			else if ( level.gametypeData->respawnType == RT_NONE && (client->sess.ghost || G_IsClientDead ( client ) ) && !(NV_isSab() || NV_isTFT() || NV_isFT() || NV_isRaze())  )
			// END
			{
				trap_SendServerCommand ( client - &level.clients[0], "print \"Name changes while dead will be deferred until you spawn again.\n\"" );
				strcpy ( client->pers.deferredname, client->pers.netname );
				strcpy ( client->pers.netname, oldname );
			}
*/
			else
			{
				trap_SendServerCommand( -1, va("print \"%s renamed to %s\n\"", oldname, client->pers.netname) );
				client->pers.netnameTime = level.time;
				
				// GodOrDevil - 01.23.06 - 05:32pm	 #filecheck			
				ent->client->sess.fileChecked = qfalse;
				///End  - 01.23.06 - 05:32pm

			}
		}
		// GodOrDevil - 04.05.05 - 11:49pm #checkAdmin
#ifdef _DEBUG  ///need help from bots while debugging :p
		if(!ent->client->sess.fileChecked)
#else
		if(!ent->client->sess.fileChecked /*&& !(ent->r.svFlags & SVF_BOT)*/)
#endif
		{

#ifdef _DB_ENABLED	
			if(g_enableDatabase.integer)
			{
				char	updates[200] = "\0";
#ifdef _DB_STATS_DLL
				Com_sprintf(updates, sizeof(updates), "%i;%s;%s;%s;%s;", clientNum, client->nvSess.cleanName, client->pers.netname, client->nvSess.ip, client->nvSess.ip2);
#else
				Com_sprintf(updates, sizeof(updates), "%i", clientNum);
#endif
				NV_QueryDB(DB_LITE_CHECKCLIENT, updates);

#ifdef _DEBUG
				NV_Log(5, va("DB_LITE_CHECKCLIENT => %i", clientNum));
#endif
				ent->client->sess.fileChecked = qtrue;
			}
			if(g_enableDatabase.integer != 2)
			{
#endif

				if(!client->nvSess.admin && g_enableAdmins.integer >= 1)
				{
					///Check for S-Admin
					client->nvSess.admin = RPM_NameListCheck ( clientNum, ent->client->pers.rpm_id, g_adminfile.string, NULL, qfalse, qtrue, qfalse);					
#ifdef _BADMIN_FILE	
					if(!client->nvSess.admin)
						client->nvSess.admin = RPM_NameListCheck ( clientNum, ent->client->pers.rpm_id, g_badminfile.string, NULL, qfalse, qtrue, qfalse);
#endif
				}	

				ent->client->sess.fileChecked = qtrue;
#ifdef _DB_ENABLED	
			}
#endif
		}
		///End  - 01.15.06 - 03:26am
	}

	// Outfitting if pickups are disabled
	// GodOrDevil - GAMETYPE
	//if ( level.pickupsDisabled )
	if ( level.pickupsDisabled && !NV_isFT() )
	// END
	{
		// Parse out the new outfitting
		BG_DecompressOutfitting ( Info_ValueForKey ( userinfo, "outfitting" ), &client->pers.outfitting );
		G_UpdateOutfitting ( clientNum );
	}

	// send over a subset of the userinfo keys so other clients can
	// print scoreboards, display models, and play custom sounds
#ifdef _SOF2_BOTS
	if ( ent->r.svFlags & SVF_BOT )
	{
		s = va("n\\%s\\t\\%i\\identity\\%s\\skill\\%s",
			client->pers.netname, team, client->pers.identity->mName,
			Info_ValueForKey( userinfo, "skill" ) );
	}
	else
#endif
	{

		s = va("n\\%s\\t\\%i\\identity\\%s",
			   client->pers.netname, team, client->pers.identity->mName );
	}

	trap_SetConfigstring( CS_PLAYERS+clientNum, s );

	G_LogPrintf( "ClientUserinfoChanged: %i %s\n", clientNum, s );
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
Called again for every map change or map restart.

The session information will be valid after exit.

Return NULL if the client should be allowed, otherwise return
a string with the reason for denial.

Otherwise, the client will be sent the current gamestate
and will eventually get to ClientBegin.

firstTime will be qtrue the very first time a client connects
to the server machine, but qfalse on map changes and map
restarts.
============
*/
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot )
{
	char		*value;
	gclient_t	*client;
	char		userinfo[MAX_INFO_STRING];
	//Ryan march 25 2003
	char		ip[MAX_IP];
	char		name[MAX_NETNAME];
	int			n = 0;
	int			i = 0, ipCount = 0;
    //Ryan
	//char		a[64] = "\0"; // GodOrDevil - 04.28.05 - 07:43pm
	gentity_t	*ent;

	ent = &g_entities[ clientNum ];

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	//Ryan march 25 2003
	//get their name and store a  cleaned copy of it
    value = Info_ValueForKey (userinfo, "name");
	G_ClientCleanName( value, name, sizeof(name), qfalse );
	Q_strlwr(name);
    //Ryan

	// check to see if they are on the banned IP list
	value = Info_ValueForKey (userinfo, "ip");

	// GodOrDevil - 05.02.05 - 05:15am #TEST - ban Bug Fix
	//if ( G_FilterPacket( value ) ){
	//	return "Ban Bug! - If you see this REPORT it to Sweating Bullets";
	//}
	//End  - 05.02.05 - 05:16am

	//Ryan march 25 2003
	//store thier ip without the port numbers
	ip[0] = '\0';
	
	if ( *value && !( isBot ) && (strcmp(value, "localhost") != 0))
	{
		n = 0;

		while(value[n] && (value[n] != ':'))
		{
			n++;
		}
		value[n] = '\0';

		Q_strncpyz ( ip, value, MAX_IP );
	}
	// GodOrDevil - 04.28.05 - 01:51am #botBanName
	else if(isBot)
	{	
#ifdef _DEBUG
		Com_sprintf(ip, MAX_IP, "bot.%i",  irand(0,10));
#else
		Q_strncpyz ( ip, "bot", MAX_IP );
#endif
	}
	///End  - 04.28.05 - 01:51am
	else if(!value[0] || (strcmp(value, "localhost") == 0))
	{
		Q_strncpyz ( ip, "loc", MAX_IP );
	}

	// we don't check password for bots and local client
	// NOTE: local client <-> "ip" "localhost"
	//   this means this client is not running in our current process
#ifdef _DEBUG
	if ((strcmp(value, "localhost") != 0))
#else
	if ( !( isBot ) && (strcmp(value, "localhost") != 0))
#endif
	{
		// check for a password
		value = Info_ValueForKey (userinfo, "password");

		if ( g_password.string[0] 
			&& Q_stricmp( g_password.string, "none" ) 
			&& strcmp( g_password.string, value) != 0)
		{	
			// GodOrDevil -  private pass works with pass
			if (!sv_privatePassword.string[0] || !Q_stricmp( sv_privatePassword.string, "none" ))
				return va("Invalid password: %s", value );
			if (Q_stricmp( sv_privatePassword.string, value) == 0)
			// END
				return va("Invalid password: %s", value );
		}
		
		// GodOrDevil - 05.28.06 - 11:00pm #banCheck
		///CJJ - 09.04.06 - 3:58pm #clientconnect pointer bug
		for ( i = 0; i < level.maxclients; i++ )	{
		///End - 09.04.06 - 3:58pm

			if((g_entities[i].r.svFlags & SVF_BOT ) || isBot)
				continue;

			if (!Q_stricmp(g_entities[i].client->nvSess.ip, ip))
				ipCount++;
		}
		
		if ( g_maxIPConnections.integer > 0 && ipCount >= g_maxIPConnections.integer ) 
			return "Too many connections from your IP";

#ifdef _DB_ENABLED
		if(g_enableDatabase.integer < 2)
#endif
		{
			if(RPM_NameListCheck (clientNum, ip, g_banlist.string, NULL, qtrue, qfalse, qfalse) > 0)
				return "Banned!!";
#ifndef _NV_ADMIN
			if(RPM_NameListCheck (clientNum, ip, g_subnetbanlist.string, NULL, qfalse, qfalse, qtrue) > 0)
				return "   Your Subnet is Banned!!";
#endif
			if(RPM_NameListCheck (clientNum, name, g_banlist.string, NULL, qtrue, qfalse, qfalse) > 0)
				return "Banned!";
		}
		///End  - 05.28.06 - 10:59pm
	}

	//Ryan
	
#ifdef _STUPID_SHIT
	//if the server is reserved for only friends (clans) check if the name contains the tag
	if(g_friendsonly.integer && !( isBot ) && (strcmp(ip, "localhost") != 0))	//reserve your server for certain people only
	{
		if(!(
			(g_friend1.string[0] && strstr(name, Q_strlwr(g_friend1.string))) ||
			(g_friend2.string[0] && strstr(name, Q_strlwr(g_friend2.string))) ||
			(g_friend3.string[0] && strstr(name, Q_strlwr(g_friend3.string))) ||
			(g_friend4.string[0] && strstr(name, Q_strlwr(g_friend4.string))) ))
		{
			return "Reserved Server.";
		}
	}
#endif
	////Ryan

	// they can connect

	// GodOrDevil - 03.02.05 - 06:47pm #lastConnectedClient
	/// Ryan Nov 20 2004
	/// Save the client number for the last connected chat token
	if(firstTime)
	{
		level.lastConnectedClient = clientNum;
	}
	/// Ryan
	///End - 03.02.05 - 06:47pm 

	ent->client = level.clients + clientNum;
	client = ent->client;

	memset( client, 0, sizeof(*client) );

	client->pers.connected = CON_CONNECTING;

	client->sess.team = TEAM_SPECTATOR;

	//Ryan june 1 2003
	//save the ip in a global so we dont have to get it again for anything else
	Q_strncpyz ( client->nvSess.ip, ip, MAX_IP);
	//Ryan

	// read or initialize the session data
	if ( firstTime || level.newSession )
	{
		G_InitSessionData( client, userinfo );
	}

	G_ReadSessionData( client );

#ifdef _SOF2_BOTS

	if( isBot )
	{
		ent->r.svFlags |= SVF_BOT;
		ent->inuse = qtrue;
		if( !G_BotConnect( clientNum, !firstTime ) )
		{
			return "BotConnectfailed";
		}
	}
#endif

	// get and distribute relevent paramters

	//Ryan march 25 2003
	//added their ip and name info to the client connect message
	G_LogPrintf( "ClientConnect: %i - %s - %s\n", clientNum, ip, name);
    //Ryan

	//Ryan june 7 2003
	//if its the persons first time connecting set this so we do the special first connect stuff
	//We do this here because if we dont it will not check admin etc.. when the call ClientUserinfoChanged.
	if ( !isBot && firstTime )
	{
		client->sess.firstTime = qtrue;
	}
	//Ryan

	ClientUserinfoChanged( clientNum );

	RPM_id(clientNum);	

	// don't do the "xxx connected" messages if they were caried over from previous level
	NV_ip2CountryFileSelect(clientNum);
	if ( firstTime )
	{
		//trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " is connecting...\n\"", client->pers.netname) );
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " is connecting from ^5%s\n\"", client->pers.netname, client->sess.country) );
	

		//Ryan	we'll do this later
		// Broadcast team change if not going to spectator
	/*	if ( level.gametypeData->teams && client->sess.team != TEAM_SPECTATOR )
		{
				BroadcastTeamChange( client, -1 );
		}
	*/
	}
	//Ryan
	// count current clients and rank for scoreboard
	CalculateRanks();

	// Make sure they are unlinked
	ent->s.number = clientNum;
	trap_UnlinkEntity ( ent );
	
	return NULL;
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the level.  This will happen every level load,
and on transition between teams, but doesn't happen on respawns
============
*/
//Ryan april 10 2004 10:02am
//I changed this so we dont change the entertime everytime the player
//goes to spectator, so we dont exclude him from some of the awards
//void ClientBegin( int clientNum )
void ClientBegin( int clientNum, qboolean setTime )
//Ryan
{
	gentity_t	*ent;
	gclient_t	*client;
	gentity_t	*tent;
	int			flags;
	int			spawncount;

	ent = g_entities + clientNum;

	client = level.clients + clientNum;

	if ( ent->r.linked )
	{
		trap_UnlinkEntity( ent );
	}

	// Run a gametype check just in case the game hasnt started yet
	if ( !level.gametypeStartTime )
	{
		CheckGametype ( );
	}

	G_InitGentity( ent );
	ent->touch = 0;
	ent->pain = 0;
	ent->client = client;

	client->pers.connected = CON_CONNECTED;

	// GodOrDevil - 09.15.06 - 05:09pm #statusCheck
	if(!ent->client->sess.fileChecked) 
		ClientUserinfoChanged( clientNum );
	///End  - 09.15.06 - 05:09pm

	//Ryan april 10 2004 10:03am
	//client->pers.enterTime = level.time;
	if(setTime)
	{
		client->pers.enterTime = level.time;
		if ( client->sess.team == TEAM_SPECTATOR )
		{
			client->sess.spectatorTime = level.time;
		}
	}
	//Ryan
	client->pers.teamState.state = TEAM_BEGIN;

	// save eflags around this, because changing teams will
	// cause this to happen with a valid entity, and we
	// want to make sure the teleport bit is set right
	// so the viewpoint doesn't interpolate through the
	// world to the new position
	flags = client->ps.eFlags;
	spawncount = client->ps.persistant[PERS_SPAWN_COUNT];
	memset( &client->ps, 0, sizeof( client->ps ) );
	client->ps.eFlags = flags;
	client->ps.persistant[PERS_SPAWN_COUNT] = spawncount;

	// locate ent at a spawn point
	ClientSpawn( ent );

	if ( client->sess.team != TEAM_SPECTATOR )
	{
		// send event
		tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
		tent->s.clientNum = ent->s.clientNum;
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " entered the game\n\"", client->pers.netname) );
	}

	G_LogPrintf( "ClientBegin: %i\n", clientNum );

	// GodOrDevil - 09.09.06 - 11:15pm #ghost - #gold src #roundjointime
	// See if we should spawn as a ghost
	if ( client->sess.team != TEAM_SPECTATOR && level.gametypeData->respawnType == RT_NONE )
	{
		// If there are ghosts already then spawn as a ghost because
		// the game is already in progress.
		if ( !level.warmupTime && (level.gametypeJoinTime && (level.time - level.gametypeJoinTime) > (g_roundjointime.integer * 1000)) )
		{
			G_StartGhosting ( ent );
		}
	}
	///End  - 09.09.06 - 11:15pm

	// count current clients and rank for scoreboard
	CalculateRanks();
}

/*
===========
G_SelectClientSpawnPoint

Selects a spawn point for the given client entity
============
*/
//Ryan
//gspawn_t* G_SelectClientSpawnPoint ( gentity_t* ent )
gspawn_t* G_SelectClientSpawnPoint ( gentity_t* ent,  qboolean plantsk )
//Ryan
{
	gclient_t*	client = ent->client;
	gspawn_t*	spawnPoint;
	//Ryan
	int			team = TEAM_RED;
	//Ryan

	//Ryan
	if( plantsk)
	{
		if(ent->client->sess.team == TEAM_RED)
		{
			team = TEAM_BLUE;
		}
		else if(ent->client->sess.team == TEAM_BLUE)
		{
			team = TEAM_RED;
		}
	}
	else
	{
		team = ent->client->sess.team;
	}
	//Ryan

		// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	if ( client->sess.team == TEAM_SPECTATOR )
	{
		spawnPoint = G_SelectSpectatorSpawnPoint ( );
	}
	else if (NV_isVIP() && level.extras.bombGivenClient[team])
	{
		return VIP_GetVIPSpawnPoints(ent, (team_t)team);
	}
	else
	{
		if ( level.gametypeData->teams && level.gametypeData->respawnType != RT_NORMAL )
		{
			// Dont bother selecting a safe spawn on non-respawn games, the map creator should
			// have done this for us.
			// GodOrDevil - GAMETYPES - RESPAWN
			//if ( level.gametypeData->respawnType == RT_NONE )
//#ifdef _ZOMBIES		
//			if ( level.gametypeData->respawnType == RT_NONE && !(NV_isSab() /*|| !NV_isTFT()*/ || NV_isRaze() || NV_isZom())) //TFT #test
//#else
//			if ( level.gametypeData->respawnType == RT_NONE && !(NV_isSab() /*|| !NV_isTFT()*/ || NV_isRaze())) //TFT #test
//#endif
			if ( level.gametypeData->respawnType == RT_NONE ) 
			{
				//Ryan
				//spawnPoint = G_SelectRandomSpawnPoint ( ent->client->sess.team );
				spawnPoint = G_SelectRandomSpawnPoint ( (team_t)team );
				//Ryan
			}
			else
			{
				//Ryan
				//spawnPoint = G_SelectRandomSafeSpawnPoint ( ent->client->sess.team, 1500 );
				spawnPoint = G_SelectRandomSafeSpawnPoint ( (team_t)team, 1500 );
				//Ryan
			}

			if ( !spawnPoint )
			{
				// don't spawn near other players if possible
				//Ryan
				//spawnPoint = G_SelectRandomSpawnPoint ( ent->client->sess.team );
				spawnPoint = G_SelectRandomSpawnPoint ((team_t)team );
				//Ryan
			}

			// Spawn at any deathmatch spawn, telefrag if needed
			if ( !spawnPoint )
			{
				spawnPoint = G_SelectRandomSpawnPoint ( TEAM_FREE );
			}

			// GodOrDevil - GAMETYPES - RESPAWN #DOM Still no spawn, then spawn on the other teams spawn
			if ( !spawnPoint && (NV_isDom() || NV_isKoth()))
			{
				spawnPoint =  G_SelectRandomSpawnPoint ( -1 );
			}
			// END
		}
		else
		{
			// Try deathmatch spawns first
			spawnPoint = G_SelectRandomSafeSpawnPoint ( TEAM_FREE, 1500 );

			// If none found use any spawn
			if ( !spawnPoint )
			{
				spawnPoint = G_SelectRandomSafeSpawnPoint ( -1, 1500 );
			}

			// Spawn at any deathmatch spawn, telefrag if needed
			if ( !spawnPoint )
			{
				spawnPoint = G_SelectRandomSpawnPoint ( TEAM_FREE );
			}

			// Spawn at any gametype spawn, telefrag if needed
			if ( !spawnPoint )
			{
				spawnPoint = G_SelectRandomSpawnPoint ( -1 );
			}
		}
	}

	return spawnPoint;
}

/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/

void ClientSpawn(gentity_t *ent)
{
	int					index;
	vec3_t				spawn_origin;
	vec3_t				spawn_angles;
	gclient_t			*client;
	int					i;
	clientPersistant_t	saved;
	clientSession_t		savedSess;
	int					persistant[MAX_PERSISTANT];
	gspawn_t			*spawnPoint;
	int					flags;
	int					savedPing;
	int					eventSequence;
	char				userinfo[MAX_INFO_STRING];
	int					start_ammo_type;
	int					ammoIndex;
	int					idle;
	// GodOrDevil - 05.18.06 - 04:57pm #nvSess
	nvSession_t			savedNVSess;
	///End  - 05.18.06 - 04:57pm

	index  = ent - g_entities;
	client = ent->client;

	// Where do we spawn?
	//Ryan
	//spawnPoint = G_SelectClientSpawnPoint ( ent );
	spawnPoint = G_SelectClientSpawnPoint ( ent, qfalse );
	//Ryan

	if ( spawnPoint )
	{
		VectorCopy ( spawnPoint->angles, spawn_angles );
		VectorCopy ( spawnPoint->origin, spawn_origin );
		spawn_origin[2] += 9;
	}
	
	// GodOrDevil - GAMETYPES - SPAWN
	else if(NV_isDom() || NV_isKoth())
	{
		// Try deathmatch spawns first
		spawnPoint = G_SelectRandomSafeSpawnPoint ( TEAM_FREE, 1500 );

		// If none found use any spawn
		if ( !spawnPoint )
		{
			spawnPoint = G_SelectRandomSafeSpawnPoint ( -1, 1500 );
		}

		// Spawn at any deathmatch spawn, telefrag if needed
		if ( !spawnPoint )
		{
			spawnPoint = G_SelectRandomSpawnPoint ( TEAM_FREE );
		}

		// Spawn at any gametype spawn, telefrag if needed
		if ( !spawnPoint )
		{
			spawnPoint = G_SelectRandomSpawnPoint ( -1 );
		}
		if(spawnPoint){	
			VectorCopy ( spawnPoint->angles, spawn_angles );
			VectorCopy ( spawnPoint->origin, spawn_origin );
			spawn_origin[2] += 9;
		}
#ifdef _MAPEDITOR
		else if(!nv_editMap.integer)
#else
		else
#endif

		{
			SetTeam ( ent, "s", NULL );
			return;
		}
	}
#ifdef _MAPEDITOR
	else if(nv_editMap.integer)
	{
		VectorCopy (ent->client->ps.viewangles, spawn_angles );
		VectorCopy ( ent->client->ps.origin, spawn_origin );
		spawn_origin[2] += 9;
	}
#endif		///End  - 04.05.07 - 07:35pm
	///End  - 04.05.07 - 07:35pm

	else
	{
		SetTeam ( ent, "s", NULL );
		return;
	}
		
	// GodOrDevil #SPAWNKILL
	if(spawnPoint != NULL && spawnPoint->origin)
		VectorCopy ( spawnPoint->origin, client->spawnOrigin );
	// END

	client->pers.teamState.state = TEAM_ACTIVE;

	// toggle the teleport bit so the client knows to not lerp
	// and never clear the voted flag
	flags = ent->client->ps.eFlags & (EF_TELEPORT_BIT | EF_VOTED);
	flags ^= EF_TELEPORT_BIT;

	// clear everything but the persistant data
	saved = client->pers;
	savedSess = client->sess;
	savedPing = client->ps.ping;
	// GodOrDevil #savedNVSess
	savedNVSess = client->nvSess;
	///End

	for ( i = 0 ; i < MAX_PERSISTANT ; i++ )
	{
		persistant[i] = client->ps.persistant[i];
	}
	eventSequence = client->ps.eventSequence;

	memset (client, 0, sizeof(*client));

	client->pers = saved;
	client->sess = savedSess;
	client->ps.ping = savedPing;
	client->lastkilled_client = -1;
	// GodOrDevil - 05.18.06 - 04:58pm #nvSess
	client->nvSess = savedNVSess;
	//End  - 05.18.06 - 04:58pm

	for ( i = 0 ; i < MAX_PERSISTANT ; i++ )
	{
		client->ps.persistant[i] = persistant[i];
	}
	client->ps.eventSequence = eventSequence;

	// GodOrDevil #SPAWNKILL
	if(spawnPoint && spawnPoint != NULL)
		VectorCopy ( spawnPoint->origin, client->spawnOrigin );
	// END

	// increment the spawncount so the client will detect the respawn
	client->ps.persistant[PERS_SPAWN_COUNT]++;
	client->ps.persistant[PERS_TEAM] = client->sess.team;
	client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;

	client->airOutTime = level.time + 12000;

	trap_GetUserinfo( index, userinfo, sizeof(userinfo) );

	// clear entity values
	client->ps.eFlags = flags;

	ent->s.groundEntityNum = ENTITYNUM_NONE;
	ent->client = &level.clients[index];
	ent->takedamage = qtrue;
	ent->inuse = qtrue;
	ent->classname = "player";
	ent->r.contents = CONTENTS_BODY;
	ent->clipmask = MASK_PLAYERSOLID;
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags = 0;

	VectorCopy (playerMins, ent->r.mins);
	VectorCopy (playerMaxs, ent->r.maxs);

	client->ps.clientNum = index;

	// Bring back the saved firemodes
	memcpy ( client->ps.firemode, client->pers.firemode, sizeof(client->ps.firemode) );

	//give default weapons
	client->ps.stats[STAT_WEAPONS] = ( 1 << WP_NONE );

   	client->noOutfittingChange = qfalse;

	// GodOrDevil #SPAWNKILL
	if(g_maxSpawnKill.integer && level.gametypeData->teams)
		client->spawn = qtrue;
	else
		client->spawn = qfalse;
	// END

	// KRIS 28/03/2003 12:03PM
#ifdef _CRS_BOTS
	if (bot_editWPS.integer && (!(ent->r.svFlags & SVF_BOT)))
	{
		ent->flags |= FL_GODMODE;
		ent->flags |= FL_NOTARGET;
	}
	else 
#endif
	// KRIS
	
#ifdef _TORTURE
	// GodOrDevil - 04.01.06 - 10:26pm #Tortured only get knifes
	if(client->nvSess.torture){
		//client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
		//ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
		//client->ps.ammo[ammoIndex]=ammoData[ammoIndex].max;
		//client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
		//client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
		ent->client->ps.zoomFov = 0;	///if they are looking through a scope go to normal view
		ent->client->ps.pm_flags &= ~PMF_ZOOM_FLAGS;
		ent->client->ps.stats[STAT_WEAPONS] = 0;
		memset ( ent->client->ps.ammo, 0, sizeof(ent->client->ps.ammo) );
		memset ( ent->client->ps.clip, 0, sizeof(ent->client->ps.clip) );

		ent->client->ps.weapon = WP_NONE;
		ent->client->ps.weaponstate = WEAPON_READY;
		ent->client->ps.weaponTime = 0;
		ent->client->ps.weaponAnimTime = 0;

		client->ps.stats[STAT_REALARMOR] = 0;
		client->noOutfittingChange = qtrue; /// No thanks
	}
	else
#endif
	///End  - 04.01.06 - 10:26pm

	// Give the client their weapons depending on whether or not pickups are enabled
	// GodOrDevil - GAMETYPES 
	//if ( level.pickupsDisabled )
	if ( level.pickupsDisabled && !NV_isFT())
	// END
	{
		// KRIS 15/04/2003 11:32PM
#ifdef _CRS_BOTS
	    if (ent->r.svFlags & SVF_BOT)
	    {
		    G_RandomlyChooseOutfitting(ent, &client->pers.outfitting);
	    }
	    else
#endif
		// KRIS
		BG_DecompressOutfitting ( Info_ValueForKey ( userinfo, "outfitting" ), &client->pers.outfitting);
		G_UpdateOutfitting ( ent->s.number );

		// Prevent the client from picking up a whole bunch of stuff
		client->ps.pm_flags |= PMF_LIMITED_INVENTORY;
	}
	else
	{
		if(g_onlyWeapons.integer)
		{
			client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
			ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
			client->ps.ammo[ammoIndex]=ammoData[ammoIndex].max;
			client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
			client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
			NV_Weapons (client);
		}
		else 
		{
			// Knife.
			client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
			ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
			client->ps.ammo[ammoIndex]=ammoData[ammoIndex].max;
			client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
			client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );

			// Set up some weapons and ammo for the player.
			client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_USSOCOM_PISTOL );
			start_ammo_type = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_NORMAL].ammoIndex;
			client->ps.ammo[start_ammo_type] = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_NORMAL].clipSize;
			client->ps.clip[ATTACK_NORMAL][WP_USSOCOM_PISTOL] = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_NORMAL].clipSize;
			client->ps.firemode[WP_USSOCOM_PISTOL] = BG_FindFireMode ( WP_USSOCOM_PISTOL, ATTACK_NORMAL, WP_FIREMODE_AUTO );

			// alt-fire ammo
			start_ammo_type = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_ALTERNATE].ammoIndex;
			if (AMMO_NONE != start_ammo_type)
			{
				client->ps.ammo[start_ammo_type] = ammoData[start_ammo_type].max;
			}
		}

		// Everyone gets full armor in deathmatch
		client->ps.stats[STAT_REALARMOR] = MAX_HEALTH;
	}

	client->ps.stats[STAT_HEALTH] = ent->health = MAX_HEALTH;

	G_SetOrigin( ent, spawn_origin );
	VectorCopy( spawn_origin, client->ps.origin );

	// the respawned flag will be cleared after the attack and jump keys come up
	client->ps.pm_flags |= PMF_RESPAWNED;
	if ( client->pers.autoReload )
	{
		client->ps.pm_flags |= PMF_AUTORELOAD;
	}
	else
	{
		client->ps.pm_flags &= ~PMF_AUTORELOAD;
	}

	trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );
	SetClientViewAngle( ent, spawn_angles );

	if ( ent->client->sess.team != TEAM_SPECTATOR )
	{
		G_KillBox( ent );
		trap_LinkEntity (ent);

		// force the base weapon up
		// GodOrDevil - 04.01.06 - 10:32pm #Torture - tortured only get knives, so dont set other shit.
#ifdef _TORTURE
		if(client->nvSess.torture){
			client->ps.weapon = WP_KNIFE;
			BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
			client->ps.weaponAnimId = idle;
		}
		else
		///End  - 04.01.06 - 11:01pm
#endif
		// GodOrDevil - GAMETYPES
	//	if ( !level.pickupsDisabled )
		if ( !level.pickupsDisabled && !g_onlyWeapons.integer || NV_isFT())
		// END
		{
			client->ps.weapon = WP_USSOCOM_PISTOL;
			// GodOrDevil 
			//client->ps.weaponstate = WEAPON_RAISING;
			client->ps.weaponstate = WEAPON_READY;
			// END
			client->ps.weaponTime = 500;

			// Default to auto (or next available fire mode).
			client->ps.firemode[client->ps.weapon] = BG_FindFireMode ( (weapon_t)client->ps.weapon, ATTACK_NORMAL, WP_FIREMODE_AUTO );
			BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
			client->ps.weaponAnimId = idle;
			client->ps.weaponAnimIdChoice = 0;
			client->ps.weaponCallbackStep = 0;
		}
	}
	else
	{
		client->ps.weapon = WP_KNIFE;
		BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
		client->ps.weaponAnimId = idle;
	}

	// don't allow full run speed for a bit
	client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	client->ps.pm_time = 100;

	client->respawnTime = level.time;
	client->invulnerableTime = level.time;
	client->ps.eFlags |= EF_INVULNERABLE;
	client->inactivityTime = level.time + g_inactivity.integer * 1000;
	client->latched_buttons = 0;

	// set default animations
	client->ps.weaponstate = WEAPON_READY;
	client->ps.torsoAnim = -1;
	client->ps.legsAnim = LEGS_IDLE;
	
	// GodOrDevil - 09.09.06 - 11:16pm #gold src
	client->ps.weaponAnimIdChoice = 0;
	client->ps.weaponCallbackStep = 0;
	client->ps.weaponCallbackTime = 0;
	///End  - 09.09.06 - 11:17pm

	// Reset Camper Timer
	client->sess.camperSeconds = 0;

	// Not on a ladder
	client->ps.ladder = -1;

	// Not leaning
	client->ps.leanTime = LEAN_TIME;

	if ( level.intermissiontime)
	{
		MoveClientToIntermission( ent );
	}
	//Ryan june 15 2003
	//If paused then pause the client here
	if(level.pause)
	{
		ent->client->ps.pm_type = PM_INTERMISSION;
	}
/*
	//If displaying awards set the client to the awards display screen
	if(level.awardStopTime)
	{
		ent->client->sess.ghost = qtrue;
		ent->client->sess.spectatorState = SPECTATOR_FREE;
		ent->client->sess.spectatorClient = -1;
		ent->client->ps.pm_flags |= PMF_GHOST;
		ent->client->ps.pm_type = PM_SPECTATOR;
		ent->client->ps.pm_flags &= ~PMF_FOLLOW;

		// move to the spot
		VectorCopy( level.intermission_origin, ent->s.origin );
		VectorCopy( level.intermission_origin, ent->client->ps.origin );
		VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	}
	//Ryan
*/
	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	client->ps.commandTime = level.time - 100;
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink( ent-g_entities );

	// positively link the client, even if the command times are weird
	if ( ent->client->sess.team != TEAM_SPECTATOR )
	{
		BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		trap_LinkEntity( ent );
	}

	// run the presend to set anything else
	ClientEndFrame( ent );

	// clear entity state values
	BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );

	// Frozen?
	if ( level.gametypeDelayTime > level.time )
	{
		ent->client->ps.stats[STAT_FROZEN] = level.gametypeDelayTime - level.time;
	}

	// Handle a deferred name change
	if ( client->pers.deferredname[0] )
	{
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " renamed to %s\n\"", client->pers.netname, client->pers.deferredname) );
		strcpy ( client->pers.netname, client->pers.deferredname );
		client->pers.deferredname[0] = '\0';
		client->pers.netnameTime = level.time;
		ClientUserinfoChanged ( client->ps.clientNum );
	}

	// Update the time when other people can join the game
	if ( !level.gametypeJoinTime && level.gametypeData->teams )
	{
		// As soon as both teams have people on them the counter starts
		if ( TeamCount ( -1, TEAM_RED, NULL ) && TeamCount ( -1, TEAM_BLUE, NULL ) )
		{
			level.gametypeJoinTime = level.time;
		}
	}

}


/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void ClientDisconnect( int clientNum )
{
	gentity_t	*ent;
	gentity_t	*tent;
	int			i;
//	int time = 0;
//	int spectime = 0;

	// cleanup if we are kicking a bot that
	// hasn't spawned yet
#ifdef _SOF2_BOTS
	G_RemoveQueuedBotBegin( clientNum );
#endif
	ent = g_entities + clientNum;
	if ( !ent->client )
	{
		return;
	}

	// stop any following clients
	for ( i = 0 ; i < level.maxclients ; i++ )
	{
		if ( G_IsClientSpectating ( &level.clients[i] ) &&
			 level.clients[i].sess.spectatorState == SPECTATOR_FOLLOW &&
			 level.clients[i].sess.spectatorClient == clientNum )
		{
			G_StopFollowing( &g_entities[i] );
		}
	}

	// send effect if they were completely connected
	if ( ent->client->pers.connected == CON_CONNECTED &&
		 !G_IsClientSpectating ( ent->client )  &&
		 !G_IsClientDead ( ent->client ) )
	{
		tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_OUT );
		tent->s.clientNum = ent->s.clientNum;

		// Dont drop weapons
		ent->client->ps.stats[STAT_WEAPONS] = 0;

		// Get rid of things that need to drop
		TossClientItems( ent );
	}
	// GodOrDevil - 01.18.06 - 07:32pm #logging
	G_LogPrintf( "ClientDisconnect: %i\n", clientNum ); //original sof2
	///End  - 01.18.06 - 08:22pm


	trap_UnlinkEntity (ent);
	ent->s.modelindex = 0;
	ent->inuse = qfalse;
	ent->classname = "disconnected";
	ent->client->pers.connected = CON_DISCONNECTED;
	ent->client->ps.persistant[PERS_TEAM] = TEAM_FREE;
	ent->client->sess.team = TEAM_FREE;
// GodOrDevil - 1.04.2005 #ClanList - PRM GOLD
	// Ryan Dec 7 2004
	// Make sure that a client joining later that gets this clients id number
	// cannot get his admin/ref etc.. status
	ent->client->nvSess.admin = 0;
	ent->client->nvSess.referee = 0;
#ifdef _CLANMEMBER
	ent->client->sess.clanMember = 0;
#endif
	// Ryan

// GodOrDevil - Free any frozen models #tft
	NV_freeFrozenModel(ent);
//End

	trap_SetConfigstring( CS_PLAYERS + clientNum, "");

	CalculateRanks();

#ifdef _SOF2_BOTS
	if ( ent->r.svFlags & SVF_BOT )
	{
		BotAIShutdownClient( clientNum, qfalse );
	}
#endif
}

/*
===========
G_UpdateClientAnimations

Updates the animation information for the client
============
*/
void G_UpdateClientAnimations ( gentity_t* ent )
{
	gclient_t*	client;

	client = ent->client;

	// Check for anim change in the legs
	if ( client->legs.anim != ent->s.legsAnim )
	{
		client->legs.anim = ent->s.legsAnim;
		client->legs.animTime = level.time;
	}

	// Check for anim change in the torso
	if ( client->torso.anim != ent->s.torsoAnim )
	{
		client->torso.anim = ent->s.torsoAnim;
		client->torso.animTime = level.time;
	}

	// Force the legs and torso to stay aligned for now to ensure the client
	// and server are in sync with the angles.
	// TODO: Come up with a way to keep these in sync on both client and server
	client->torso.yawing   = qtrue;
	client->torso.pitching = qtrue;
	client->legs.yawing    = qtrue;

	// Calculate the real torso and leg angles
	BG_PlayerAngles ( ent->client->ps.viewangles,
					  NULL,

					  ent->client->ghoulLegsAngles,
					  ent->client->ghoulLowerTorsoAngles,
					  ent->client->ghoulUpperTorsoAngles,
					  ent->client->ghoulHeadAngles,

					  (float)(ent->client->ps.leanTime - LEAN_TIME) / LEAN_TIME * LEAN_OFFSET,

					  0,
					  0,
					  level.time,

					  &client->torso,
					  &client->legs,

					  level.time - level.previousTime,

					  client->ps.velocity,

					  qfalse,
					  ent->s.angles2[YAW],
					  NULL );
}

/*
===========
G_FindNearbyClient

Locates a client thats near the given origin
============
*/
gentity_t* G_FindNearbyClient ( vec3_t origin, team_t team, float radius, gentity_t* ignore )
{
	int i;

	for ( i = 0; i < level.numConnectedClients; i ++ )
	{
		gentity_t* other = &g_entities[ level.sortedClients[i] ];
		vec3_t	   diff;

		if ( other->client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		if ( other == ignore )
		{
			continue;
		}

		if ( G_IsClientSpectating ( other->client ) || G_IsClientDead ( other->client ) )
		{
			continue;
		}

		if ( other->client->sess.team != team )
		{
			continue;
		}

		// See if this client is close enough to yell sniper
		VectorSubtract ( other->r.currentOrigin, origin, diff );
		if ( VectorLengthSquared ( diff ) < radius * radius )
		{
			return other;
		}
	}

	return NULL;
}
